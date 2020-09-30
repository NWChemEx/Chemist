#pragma once
#include "libchemist/sparse_map/detail_/domain_traits.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <memory>                                  // For smart pointers
#include <sde/sde.hpp>                             // For hashing


namespace libchemist::sparse_map {

// Forward declare the Domain class for template instantiation
template<typename T> class Domain;

namespace detail_{

// Forward declare the PIMPL class
template<typename T> class DomainPIMPL;

/** @brief DomainBase contains functionality common to Domain<ElementIndex> and
 *         Domain<TileIndex>.
 *
 *  Template class specialization requires you to redefine all methods for each
 *  specialization. To avoid excessive copy/paste we siphon off methods common
 *  to Domain<ElementIndex> and Domain<TileIndex> and put them into the
 *  DomainBase class. DomainBase instances are not meant to be directly
 *  instantiated, rather users should always declare either a
 *  Domain<ElementIndex> or a Domain<TileIndex> instance.
 *
 *  @tparam DerivedType The type of the Domain using this class as a base class.
 *                      Parts of DomainBase work by CRTP hence the need to know
 *                      what class is being implemented.
 *  @tparam IndexType  The type of the index in the Domain. Should be the same
 *                     as `typename DerivedType::value_type`.
 *
 *  @note This class only implements operations among Domains which store the
 *        same index types. Operations among Domains with different index types
 *        must be implemented in the derived classes.
 *
 *  @note Domains without PIMPLs behave as if they are empty.
 */
template<typename DerivedType, typename IndexType>
class DomainBase {
public:
    /// The type of indices stored in this Domain
    using value_type = IndexType;
private:
    /// Type of an instance of this class
    using my_type     = DomainBase<DerivedType, IndexType>;

    /// Type of a PIMPL containing the data for an instance of this class
    using pimpl_type  = DomainPIMPL<value_type>;

    /// Type of a pointer to a PIMPL
    using pimpl_ptr   = std::unique_ptr<pimpl_type>;

    /// Type of the struct defining types for this class
    using traits_type = DomainTraits<value_type>;
public:
    /// Type used for offset and natural numbers
    using size_type       = typename traits_type::size_type;

    /// Type of a read-only reference to index in the Domain
    using const_reference = typename traits_type::const_reference;

    /// Type of a bidirectional iterator with read-only access to indices
    using const_iterator =
      typename traits_type::template const_iterator<my_type>;

    /** @brief Creates a new empty Domain.
     *
     *  The Domain resulting from default construction contains no indices. Note
     *  that this is not the same as a Domain containing a scalar (which would
     *  have a single rank 0 index in it).
     *
     *  @throw std::bad_alloc if there is not enough memory to allocate the
     *                        PIMPL. Strong throw guarantee.
     */
    DomainBase();

    /** @brief Initializes the Domain with the provided indices.
     *
     *  This ctor can be used to set the initial state of the Domain.
     *
     *  @param[in] il The initial indices to reside in the domain.
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     *  @throw std::runtime_error if the initial values have different ranks.
     *                            Strong throw guarantee.
     */
    DomainBase(std::initializer_list<value_type> il);

    /** @brief Creates a new Domain by copying another one.
     *
     *  @param[in] rhs The Domain we are copying.
     *
     *  @throw std::bad_alloc If there is insufficient memory to copy @p rhs.
     *                        Strong throw guarantee.
     *
     *  @throw std::runtime_error if @p rhs's PIMPL is not set. Strong throw
     *                            guarantee.
     */
    DomainBase(const DomainBase& rhs);

    /** @brief Creates a new Domain by moving from an existing one.
     *
     *  @param[in] rhs The Domain whose state will be used to populate the new
     *                 instance. After this operation @p rhs will have no PIMPL
     *                 and will not be usable until it acquires a new PIMPL.
     *
     *  @throw None No throw guarantee.
     */
    DomainBase(DomainBase&& rhs) noexcept;

    /** @brief Causes the current instance's state to be set to a copy of
     *         another instance's state.
     *
     * @param[in] rhs The instance whose state is being copied.
     *
     * @return The current instance containing a copy of @p rhs's state.
     *
     * @throw std::bad_alloc if there is not enough memory to copy @p rhs's
     *                       state. Strong throw guarantee.
     */
    DomainBase& operator=(const DomainBase& rhs);

    /** @brief Causes the current Domain to take ownership of another Domain's
     *         state.
     *
     *  @param[in] rhs The Domain whose state will be given to the current
     *                 instance. After this operation @p rhs will have no PIMPL
     *                 and will not be usable until it acquires a new PIMPL.
     *
     *  @return The current instance containing @p rhs's state.
     *
     *  @throw None No throw guarantee.
     */
    DomainBase& operator=(DomainBase&& rhs) noexcept;

    /** @brief Default Dtor
     *
     *  @throw None No throw guarantee.
     */
    ~DomainBase() noexcept;

    /** @brief Exchanges the state of two Domain instances.
     *
     *  @param[in,out] rhs The Domain whose state will be swapped with this
     *                     instance's state. After this operation @p rhs will
     *                     contain this instance's state.
     *
     *  @throw None No throw guarantee.
     */
    void swap(DomainBase& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }

    /** @brief Returns an iterator pointing to the first element of the domain.
     *
     *  @return An iterator pointing to the first element in this domain.
     *
     *  @throw None No throw guarantee.
     */
    auto begin() const noexcept { return const_iterator(0, this); }

    /** @brief Returns an iterator pointing to just past the last element in
     *         the domain.
     *
     *  @return An iterator pointing to just past the last element in the
     *          domain.
     *
     *  @throw None No throw guarantee.
     */
    auto end() const noexcept { return const_iterator(size(), this); }

    /** @brief Returns true if the current Domain contains no indices.
     *
     *  A domain is empty when it is default constructed, or when it is moved
     *  from.
     *
     *  @return True if the current instance is empty and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool empty() const noexcept { return begin() == end(); }

    /** @brief Returns the rank of the indices in this Domain.
     *
     *  The rank of an index is the number of components in it. For example,
     *  the index `{0, 0}` is rank 2. Note that this function will return 0 if
     *  the Domain contains a rank 0 index, the Domain is empty, or if the
     *  Domain has no PIMPL.
     *
     *  @return The rank of the indices.
     *
     *  @throw None No throw guarantee.
     */
    size_type rank() const noexcept;

    /** @brief Returns the number of indices in this Domain.
     *
     *  Domains are sets and thus they only contain at most one copy of a given
     *  index. This function can be used to return the number of unique indices
     *  in this Domain. Note that empty domains as well as those without PIMPLs
     *  will have sizes of 0.
     *
     *  @return The number of indices in this Domain.
     *
     *  @throws None No throw guarantee.
     */
    size_type size() const noexcept;

    /** @brief Returns the number of times an index appears in this Domain
     *
     *  Domains are set-like thus a given index can only either be in or not be
     *  in the Domain (*i.e.*, count can only return 0 or 1).
     *
     *  @param[in] idx The index we are looking for.
     *
     *  @return True if the index is in this Domain and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool count(const_reference idx) const noexcept;

    /** @brief Returns the @p i-th index in the Domain.
     *
     *  Domains are ordered sets, which means that any given moment there is a
     *  concept of which element is zeroth, first, second, etc. This function
     *  can be used to retrieve an index by its offset from the beginning of the
     *  Domain.
     *
     *  @param[in] i The offset of the requested index. Must be in the range
     *               [0, size()).
     *
     *  @return A read-only reference to the requested index.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *                           throw guarantee.
     *  @throw std::runtime_error if there is no PIMPL. Strong throw guarantee.
     */
    const_reference operator[](size_type i) const;

    /** @brief Adds the index to the Domain.
     *
     *  This function is used to add the manually specified index to the domain.
     *  If the Domain already contains indices the rank of the new index must
     *  match the rank of the indices already in the Domain. Domains are
     *  set-like so this function is a no-op if the index is already in the
     *  Domain.
     *
     *  @param[in] idx The index we are adding to the Domain. If the domain
     *                 already contains indices, @p idx must be the same rank as
     *                 the already present indices.
     *
     *  @throw std::bad_alloc if there is insufficient memory to add the index
     *                        or create the PIMPL (if it is currently null).
     *                        Strong throw guarantee.
     *
     *  @throw std::runtime_error if the Domain already contains indices and
     *                            @p idx is not the same rank. Strong throw
     *                            guarantee.
     */
    void insert(value_type idx);

    /** @brief Makes this instance the Cartesian product of this Domain and
     *         the provided Domain.
     *
     *  The Cartesian product of a Domain, @f$A@f$, containing rank @f$r_A@f$
     *  indices and a Domain, @f$B@f$, containing rank @f$r_B@f$ indices is a
     *  Domain, @f$C@f$ with rank @f$n_C = n_A + n_B@f$ indices, such that:
     *
     *  @f[
     *  C = \left\lbrace(a_i, b_j) \forall a_i\in A
     *                             \forall b_j in B\right\rbrace,
     *  @f]
     *
     *  where @f$(a_i, b_j)@f$ is the tuple made from concatenating the
     *  @f$i@f$-th index in @f$A@f$ with the @f$j@f$-th index in @f$B@f$. The
     *  number of indices in the new domain is equal to the number of indices in
     *  @f$A@f$ times the number of inidces in @f$B@f$. Note that this means
     *  that the Cartesian product of any domain with an empty domain is also
     *  the empty domain.
     *
     * @param[in] rhs The Domain we are taking the Cartesian product with.
     * @return The current Domain with its state set to the Cartesian product of
     *         its previous state and @p rhs.
     *
     * @throw std::bad_alloc if there is insufficient memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType& operator*=(const DomainBase& rhs);

    /** @brief Returns the Cartesian product of this Domain and the provided
     *         Domain.
     *
     *  The Cartesian product of a Domain, @f$A@f$, containing rank @f$r_A@f$
     *  indices and a Domain, @f$B@f$, containing rank @f$r_B@f$ indices is a
     *  Domain, @f$C@f$ with rank @f$n_C = n_A + n_B@f$ indices, such that:
     *
     *  @f[
     *  C = \left\lbrace(a_i, b_j) \forall a_i\in A
     *                             \forall b_j in B\right\rbrace,
     *  @f]
     *
     *  where @f$(a_i, b_j)@f$ is the tuple made from concatenating the
     *  @f$i@f$-th index in @f$A@f$ with the @f$j@f$-th index in @f$B@f$. The
     *  number of indices in the new domain is equal to the number of indices in
     *  @f$A@f$ times the number of inidces in @f$B@f$. Note that this means
     *  that the Cartesian product of any domain with an empty domain is also
     *  the empty domain.
     *
     * @param[in] rhs The Domain we are taking the Cartesian product with.
     * @return The Domain resulting from the Cartesian product of this Domain
     *         and @p rhs.
     *
     * @throw std::bad_alloc if there is insufficient memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType operator*(const DomainBase& rhs) const;

    /** @brief Sets this Domain to the union of this Domain the provided Domain.
     *
     *  The union of two domains is the a Domain containing all indices which
     *  appear in at least one of the two domains. More formally, given two
     *  Domains, @f$A@f$ and @f$B@f$, which both contain indices of rank
     *  @f$r@f$, the union of @f$A@f$ and @f$B@f$ is another Domain @f$C@f$
     *  such that:
     *
     *  @f[
     *  C = \left\lbrace c_i : c_i \in A \lor c_i \in B \right\rbrace
     *  @f]
     *
     *  hence the indices in @f$C@f$ are also of rank @f$r@f$.
     *
     *
     *  @param[in] rhs The Domain we are taking the union with. Must be empty or
     *                 contain indices with the same rank as this Domain.
     *
     *  @return The current Domain with its state overwritten by the union of
     *          its previous state with @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to store the new
     *                        state. Strong throw guarantee.
     *
     *  @throw std::runtime_error if the indices in @p rhs do not have the same
     *                            rank as the indices in this Domain. Strong
     *                            throw guarantee.
     */
    DerivedType& operator+=(const DomainBase& rhs);

    /** @brief Returns the union of this Domain with the provided Domain.
     *
     *  The union of two domains is the a Domain containing all indices which
     *  appear in at least one of the two domains. More formally, given two
     *  Domains, @f$A@f$ and @f$B@f$, which both contain indices of rank
     *  @f$r@f$, the union of @f$A@f$ and @f$B@f$ is another Domain @f$C@f$
     *  such that:
     *
     *  @f[
     *  C = \left\lbrace c_i : c_i \in A \lor c_i \in B \right\rbrace
     *  @f]
     *
     *  hence the indices in @f$C@f$ are also of rank @f$r@f$.
     *
     *
     *  @param[in] rhs The Domain we are taking the union with. Must be empty or
     *                 contain indices with the same rank as this Domain.
     *
     *  @return The Domain resulting from taking the the union of this Domain
     *          with @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to store the new
     *                        state. Strong throw guarantee.
     *
     *  @throw std::runtime_error if the indices in @p rhs do not have the same
     *                            rank as the indices in this Domain. Strong
     *                            throw guarantee.
     */
    DerivedType operator+(const DomainBase& rhs) const;

    /** @brief Determines if two Domains are the same.
     *
     *  Two Domains are equal if they both contain the same number of indices,
     *  those indices have the same rank, and the contents of the Domains are
     *  the same.
     *
     * @param[in] rhs The domain to compare against.
     *
     * @return True if the Domains contain the same indices and false otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool operator==(const DomainBase& rhs) const noexcept;

    /** @brief Adds a hash of the current domain to the provided hashing object.
     *
     *  @param[in,out] h The object being used for hashing. After this function
     *                   the internal state will be updated with a hash of this
     *                   object.
     *
     *  @throws std::runtime_error if the PIMPL is not set. Strong throw
     *                             guarantee.
     */
    void hash(sde::Hasher& h) const;

    /** @brief Adds a string representation of this Domain to the provided
     *         stream.
     *
     *  @param[in,out] os The stream we are adding this Domain to. After this
     *                    call @p os will contain a string representation of
     *                    this Domain.
     *
     *  @return @p os after adding this Domain to it.
     */
    std::ostream& print(std::ostream& os) const;
protected:
    /// Wraps downcasting this instance to DerivedType
    DerivedType& downcast_() noexcept;

    /// Wraps downcasting this instance to a read-only DerivedType
    const DerivedType& downcast_() const noexcept;

    /// Ensures that the PIMPL is non-Null and returns it
    pimpl_type& pimpl_();

    /// Ensures that the PIMPL is non-Null and returns it
    const pimpl_type& pimpl_() const;
private:
    /// The instance actually holding the Domain's data
    pimpl_ptr m_pimpl_;
}; // class DomainBase

//------------------------------------------------------------------------------
//                      Related Free Functions
//------------------------------------------------------------------------------

/** @brief Determines if two Domains are different.
 *  @relates DomainBase
 *
 *  Two Domains are equal if they both contain the same number of indices,
 *  those indices have the same rank, and the contents of the Domains are
 *  the same.
 *
 *  @tparam DerivedType The type of the class being implemented by DomainBase.
 *  @tparam IndexType The type used to hold the indices.
 *
 *  @param[in] rhs The domain to compare against.
 *
 *  @return True if the Domains are different and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename DerivedType, typename IndexType>
bool operator!=(const DomainBase<DerivedType, IndexType>& lhs,
                const DomainBase<DerivedType, IndexType>& rhs) {
    return !(lhs == rhs);
}

/** @brief Adds a string representation of this Domain to the provided
 *         stream.
 *  @relates DomainBase
 *
 *  @tparam DerivedType The type of the class being implemented by DomainBase
 *  @tparam IndexType The type used to hold the indices.
 *
 *  @param[in,out] os The stream we are adding this Domain to. After this
 *                    call @p os will contain a string representation of
 *                    this Domain.
 *  @param[in] d The domain to print.
 *  @return @p os after adding this Domain to it.
 */
template<typename DerivedType, typename IndexType>
std::ostream& operator<<(std::ostream& os,
                         const DomainBase<DerivedType, IndexType>& d){
    return d.print(os);
}

//------------------------------------------------------------------------------
//                   Forward Declare Template Instantiations
//------------------------------------------------------------------------------

extern template class DomainBase<Domain<ElementIndex>, ElementIndex>;
extern template class DomainBase<Domain<TileIndex>, TileIndex>;

}} // namespace libhcemist::sparse_map::detail_
