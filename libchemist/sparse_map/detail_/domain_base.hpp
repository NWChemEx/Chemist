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

    bool count(const_reference idx) const noexcept;

    const_reference operator[](size_type i) const;

    void insert(value_type idx);

    DomainBase& operator*=(const DomainBase& rhs);
    DerivedType operator*(const DerivedType& rhs) const;
    DomainBase& operator+=(const DomainBase& rhs);
    DerivedType operator+(const DerivedType& rhs) const;

    bool operator==(const DomainBase& rhs) const noexcept;

    void hash(sde::Hasher& h) const;
    std::ostream& print(std::ostream& os) const;

protected:
    DerivedType& downcast_() noexcept;
    const DerivedType& downcast_() const noexcept;
    pimpl_type& pimpl_();
    const pimpl_type& pimpl_() const;
private:
    pimpl_ptr m_pimpl_;
}; // class DomainBase

//------------------------------------------------------------------------------
//                      Related Free Functions
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndexType>
bool operator!=(const DomainBase<DerivedType, IndexType>& lhs,
                const DomainBase<DerivedType, IndexType>& rhs) {
    return !(lhs == rhs);
}

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
