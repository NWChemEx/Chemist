#pragma once
#include "libchemist/sparse_map/domain/domain.hpp"
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map/detail_/sparse_map_traits.hpp"
#include <memory>

namespace libchemist::sparse_map {

// Forward declare SparseMap class (needed for template instantiations)
template<typename IndIndex, typename DepIndex>
class SparseMap;

namespace detail_ {

// Forward declare the SparseMapPIMPL class
template<typename IndIndex, typename DepIndex>
class SparseMapPIMPL;

template<typename DerivedType, typename IndIndex, typename DepIndex>
class SparseMapBase {
private:
    /// Type of an instance of this class
    using my_type = SparseMapBase<DerivedType, IndIndex, DepIndex>;

    /// Type of the PIMPL
    using pimpl_type = SparseMapPIMPL<IndIndex, DepIndex>;

    /// Type of a pointer to a PIMPL instance
    using pimpl_ptr = std::unique_ptr<pimpl_type>;

    /// Type of the traits class
    using traits_t = SparseMapTraits<DerivedType>;

    /// Type of an initializer list which can be forwarded to a Domain
    using domain_il = std::initializer_list<DepIndex>;

    /// Type of an initializer list for a SparseMap
    using il_t = std::initializer_list<std::pair<IndIndex, domain_il>>;

public:
    /// Type used for offsets and counting
    using size_type = typename traits_t::size_type;

    /// Type of the independent index
    using key_type = typename traits_t::key_type;

    /// Type of a domain of dependent indices
    using mapped_type = typename traits_t::mapped_type;

    /// Type of the independent-index-domain pairs
    using value_type = typename traits_t::value_type;

    /// Type of a bidirectional iterator with read-only access to indices
    using const_iterator =
      typename traits_t::template const_iterator<DerivedType>;

    /** @brief Constructs a new empty SparseMap.
     *
     *  This constructor is used to create a new SparseMap which contains no
     *  independent elements (and thus no dependent elements either).
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        PIMPL. Strong throw guarantee.
     */
    SparseMapBase();

    /** @brief Sets the value of
     *
     *  This ctor allows you to specify at construction the independent indices
     *  and the domains they map to. The format is:
     *
     *  ```
     *  {{i0, {d00, d01, ...}}, {i1, {d10, d11, ...}}, ...}
     *  ```
     *
     *  where `i0`, and `i1` are independent indices `d00`, `d01` are the first
     *  two dependent indices associated with `i0` and `d10` and `d11` are the
     *  first two dependent indices associated with `i1`. In practice this ctor
     *  is intended primarily for use with unit testing as that is one of the
     *  few scenarios where one can just lay out the contents of the sparse map
     *  to create.
     *
     *  @param[in] il The initial state of the sparse map.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        initial state. Strong throw guarantee.
     */
    SparseMapBase(il_t il);

    /** @brief Creates a new SparseMap by copying another instance.
     *
     *  @param[in] rhs The SparseMap we are copying.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *                        Strong throw guarantee.
     */
    SparseMapBase(const SparseMapBase& rhs);

    /** @brief Creates a new SparseMap instance by taking ownership of another
     *         instance's state.
     *
     *  @param[in] rhs The instance whose state is being taken. After this call
     *                 @p rhs will contain no PIMPL and thus will behave like an
     *                 empty SparseMap.
     *
     *  @throw None No throw guarantee.
     */
    SparseMapBase(SparseMapBase&& rhs) noexcept;

    /** @brief Sets this instance's state to a copy of @p rhs's state.
     *
     *  @param[in] rhs The instance whose state is being copied.
     *
     *  @return The current instance after setting its state to a copy of
     *          @p rhs's state.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *                        Strong throw guarantee.
     */
    SparseMapBase& operator=(const SparseMapBase& rhs);

    /** @brief Takes ownership of another instance's state.
     *
     *  @param[in] rhs The instance whose state is being transferred. After this
     *                 call @p rhs will have no PIMPL and will behave like an
     *                 empty SparseMap.
     *
     *  @return The current instance containing @p rhs's state.
     *
     *  @throw None No throw guarantee.
     */
    SparseMapBase& operator=(SparseMapBase&& rhs) noexcept;

    /** @brief Default dtor
     *
     *  @throw None No throw guarantee.
     */
    ~SparseMapBase() noexcept;

    /** @brief Exchanges the state of two SparseMaps
     *
     *  @param[in,out] rhs The instance whose state is being swapped with. After
     *                     this operation @p rhs will contain this instance's
     *                     original state.
     *
     *  @throw None no throw guarantee.
     */
    void swap(SparseMapBase& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }

    /** @brief Returns the number of independent indices in this SparseMap.
     *
     *  Each independent index in this SparseMap is paired with a Domain. This
     *  function can be used to retrieve the number of independent indices in
     *  this SparseMap, which is also the number of Domains and the number of
     *  independent-index-domain pairs.
     *
     *  @return The number of independent indices in this SparseMap.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept;

    /** @brief Determines if the SparseMap is empty.
     *
     *  The SparseMap is empty if it contains no independent indices. Of note a
     *  sparse map with an independent rank 0 index contains one index and is
     *  not empty.
     *
     *  @return True if the SparseMap is empty and false otherwise.
     *
     *  @throw None no throw guarantee.
     *
     *  @note this is implemented by checking if size equals zero so that it can
     *        be no-throw.
     */
    bool empty() const noexcept { return size() == 0; }

    /** @brief Determines if the SparseMap contains the specified independent
     *         index.
     *
     *  @param[in] i The index we are looking for.
     *
     *  @return True if the index is in the SparseMap and false otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool count(const key_type& i) const noexcept;

    /** @brief Returns the rank of the independent indices.
     *
     *  All independent indices in the SparseMap must have the same rank
     *  (*i.e.*, number of modes). This function can be used to determine
     *  what the rank of the independent indices are. Note that if the SparseMap
     *  is empty we return a rank of 0; hence both empty sparse maps and
     *  SparseMaps with independent indices of rank 0 return 0 (use `empty()` to
     *  distinguish between the two cases).
     *
     *  @return The rank of the independent indices.
     *
     *  @throw None No throw guarantee.
     */
    size_type ind_rank() const noexcept;

    /** @brief The number of independent modes associated with each Domain.
     *
     *  Each independent index maps to a Domain. Each Domain contains indices of
     *  the same rank. For a given SparseMap all of the mapped to Domains must
     *  contain indices of the same rank (or be empty). This function will
     *  return the rank of the indices in the Domains. The return is 0 if this
     *  SparseMap is empty, all the Domains are empty, or all Domains contain a
     *  rank 0 index.
     *
     *  @return The rank of the indices in the Domains this SparseMap maps to.
     *
     *  @throw None No throw guarantee.
     */
    size_type dep_rank() const noexcept;

    /** @brief Adds a dependent index to an independent index's Domain.
     *
     *  This function adds an dependent index to and independent index's Domain.
     *  This function will create the Domain if the independent index is not
     *  already in the SparseMap. If the dependent index is already in the
     *  independent index's Domain this is a no-op.
     *
     *  @param[in] key The independent index to which @p value should be added.
     *                 If this SparseMap is non-empty the rank of @p key must be
     *                 equal to `ind_rank()`.
     *  @param[in] value The dependent index being added to @p key's domain. If
     *                 @p key's Domain is non-empty the rank of @p value must be
     *                 equal to `dep_rank()`
     *
     *  @throw std::bad_alloc if there is insufficient memory to add the new
     *                        state. Strong throw guarantee.
     *  @throw std::runtime_error if this SparseMap is non-empty and the rank of
     *                            @p key is not equal to `ind_rank()` or if the
     *                            Domain associated with @p key is non-empty and
     *                            the rank of @p value is not equal to
     *                            `dep_rank()`. Strong throw guarantee.
     */
    void add_to_domain(const key_type& key, DepIndex value);

    const value_type& operator[](size_type i) const;

    /** @brief Returns the Domain associated with the specified independent
     *         index.
     *
     *  This function can be used to retrieve the Domain associated with a
     *  independent index. Since this is a const method an exception will be
     *  raised if the independent index is not in the SparseMap (as it can not
     *  be added).
     *
     * @param[in] key The independent index whose Domain we want. The rank of
     *                @p key must be equal to `ind_rank()`.
     *
     * @return The Domain associated with @p key in a read-only manner.
     *
     * @throw std::out_of_range if @p key is not in the SparseMap. Strong throw
     *                          guarantee.
     * @throw std::runtime_error if the rank of @p key is not equal to
     *                           `ind_rank()`. Strong throw guarantee.
     */
    const mapped_type& operator[](const key_type& key) const;

    /** @brief Returns the Domain associated with the specified independent
     *         index.
     *
     *  This function can be used to retrieve the Domain associated with a
     *  independent index.
     *
     * @param[in] key The independent index whose Domain we want. The rank of
     *                @p key must be equal to `ind_rank()`.
     *
     * @return The Domain associated with @p key in a read-only manner.
     *
     * @throw std::out_of_range if @p key is not in the SparseMap. Strong throw
     *                          guarantee.
     * @throw std::runtime_error if the rank of @p key is not equal to
     *                           `ind_rank()`. Strong throw guarantee.
     */
    const mapped_type& at(const key_type& key) const { return (*this)[key]; }

    /** @brief Returns an iterator which points to the first
     *         independent-index-Domain pair.
     *
     *  @return An iterator pointing to the first element of this map. Elements
     *          of the SparseMap can not be modified through the iterator.
     *
     *  @throw std::runtime_error if there is no PIMPL. Strong throw guarantee.
     */
    const_iterator begin() const { return const_iterator(0, &downcast_()); }

    /** @brief Returns an iterator just past the end of the SparseMap.
     *
     *  @return An iterator pointing to just past the end of the SparseMap.
     *  @throw std::runtime_error if the map has no PIMPL. Strong throw
     * guarantee.
     */
    const_iterator end() const { return const_iterator(size(), &downcast_()); }

    /** @brief Returns the direct product of this SparseMap and another
     *         SparseMap.
     *
     *  Given a SparseMap @f$A@f$ with @f$i@f$-th element @f$(a_i, \alpha_i)@f$
     *  (@f$a_i@f$ is the independent index and @f$\alpha_i@f$ is the Domain
     *  associated with @f$a_i@f$) and a SparseMap @f$B@f$ with @f$j@f$-th
     *  element @f$(b_j, \beta_j)@f$ this function computes a new SparseMap
     *  @f$C@f$ which we say is the direct product of @f$A@f$ with  @f$B@f$.
     *  @f$C@f$ is given by:
     *
     *  @f[
     *  C = \left\lbrace (a_ib_j, \alpha_i\beta_j) \forall (a_i, \alpha_i) \in A
     *                                             \forall (b_j, \beta_j) \in B
     *      \right\rbrace
     *  @f]
     *
     * @param[in] rhs The SparseMap we are taking the direct product with.
     *
     * @return The direct product of this SparseMap's state with @p rhs.
     *
     * @throw std::bad_alloc if there is not enough memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType direct_product(const SparseMapBase& rhs) const;

    /** @brief Creates a SparseMap with domains given by the Cartesian product
     *         of the Domains in this SparseMap with the Domains in @p rhs.
     *
     *  Given a SparseMap @f$A@f$ which maps independent index @f$i@f$ to
     *  @f$a_i@f$ and a SparseMap @f$B@f$ which maps independent index @f$i@f$
     *  to @f$b_i@f$. This function computes a third SparseMap, @f$C@f$, where
     *  the Domain @f$c_i@f$-th element is the Cartesian product of @f$a_i@f$
     *  with @f$b_i@f$.
     *
     * @param[in] rhs The SparseMap we are taking the Cartesian product with.
     *
     * @return The SparseMap resulting from the Cartesian product of this
     *         SparseMap's domains with @p rhs's Domains.
     *
     * @throw std::bad_alloc if there is not enough memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType operator*(const SparseMapBase& rhs) const;

    /** @brief Sets this SparseMap to the SparseMap with domains given by the
     *         Cartesian product of the Domains previously in this SparseMap
     *         with the Domains in @p rhs.
     *
     *  Given a SparseMap @f$A@f$ which maps independent index @f$i@f$ to
     *  @f$a_i@f$ and a SparseMap @f$B@f$ which maps independent index @f$i@f$
     *  to @f$b_i@f$. This function computes a third SparseMap, @f$C@f$, where
     *  the Domain @f$c_i@f$-th element is the Cartesian product of @f$a_i@f$
     *  with @f$b_i@f$.
     *
     * @param[in] rhs The SparseMap we are taking the Cartesian product with.
     *
     * @return The current SparseMap with Domains set to the resultof the
     *         Cartesian product of this SparseMap's previous domains with
     *         @p rhs's Domains.
     *
     * @throw std::bad_alloc if there is not enough memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType& operator*=(const SparseMapBase& rhs);

    /** @brief Sets this to the union of this and another map.
     *
     *  Given two SparseMaps sm1(f -> g) and sm2(f -> g), the union is formed by
     *  mapping f_i to any element in g which f_i is mapped to by either sm1 or
     *  sm2.
     *
     *  Requires that either one of the sets is empty or both sets have the same
     *  rank in independent and dependent indices.
     *
     * @param[in] rhs The SparseMap to take the union with this instance.
     *
     * @return The current instance set to the union of the two maps.
     *
     * @throw std::runtime_error if neither map is empty and the rank of the
     *                           independent/dependent indices of this instance
     *                           are not equal to the rank of the independent/
     *                           dependent indices of @p rhs.
     */
    DerivedType& operator+=(const SparseMapBase& rhs);

    /** @brief Creates a SparseMap which is the union of this and another map.
     *
     *  Given two SparseMaps sm1(f -> g) and sm2(f -> g), the union is formed by
     *  mapping f_i to any element in g which f_i is mapped to by either sm1 or
     *  sm2.
     *
     *  Requires that either one of the sets is empty or both sets have the same
     *  rank in independent and dependent indices.
     *
     * @param[in] rhs The SparseMap to take the union with this instance.
     *
     * @return The union of the two maps.
     *
     * @throw std::runtime_error if neither map is empty and the rank of the
     *                           independent/dependent indices of this instance
     *                           are not equal to the rank of the independent/
     *                           dependent indices of @p rhs.
     */
    DerivedType operator+(const SparseMapBase& rhs) const;

    /** @brief Makes this the intersection of this SparseMap and another map.
     *
     *  Given two SparseMaps sm1(f -> g) and sm2(f -> g), the intersection is
     *  formed by mapping f_i to any element in g which f_i is mapped to by both
     *  sm1 and sm2. Note that if the ranks of the independent indices (or the
     *  dependent indices) are different between the two maps the intersection
     *  is empty. Similarly the intersection of any map with an empty map is
     *  also empty.
     *
     * @param[in] sm The SparseMap to take the intersection with this instance.
     *
     * @return The current SparseMap set to the intersection of the two maps.
     *
     * @throw std::bad_alloc if there is insufficient memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType& operator^=(const SparseMapBase& rhs);

    /** @brief Computes the intersection of this SparseMap and another map.
     *
     *  Given two SparseMaps sm1(f -> g) and sm2(f -> g), the intersection is
     *  formed by mapping f_i to any element in g which f_i is mapped to by both
     *  sm1 and sm2. Note that if the ranks of the independent indices (or the
     *  dependent indices) are different between the two maps the intersection
     *  is empty. Similarly the intersection of any map with an empty map is
     *  also empty.
     *
     * @param[in] sm The SparseMap to take the intersection with this instance.
     *
     * @return The intersection of the two maps.
     *
     * @throw std::bad_alloc if there is insufficient memory to store the new
     *                       state. Strong throw guarantee.
     */
    DerivedType operator^(const SparseMapBase& rhs) const;

    /** @brief Constructs the inverse SparseMap.
     *
     *  Creates a SparseMap which maps from the dependent indices to the
     *  independent indices of the current map.
     *
     *  sparse_map.inverse().inverse() == sparse_map
     *
     * @return The inverse of the SparseMap.
     *
     * @throw std::bad_alloc if there is insufficient memory to create the
     *                       returned value. Strong throw guarantee.
     */
    SparseMap<DepIndex, IndIndex> inverse() const;

    /** @brief Creates a SparseMap from chaining two maps together.
     *
     *  Given the SparseMaps sm1(f -> g) and sm2(g -> h),
     *  the chained map from (f -> h) is formed by mapping each element
     *  f_i to a set of {g_i} using sm1, and then to any element in h which is
     * mapped to from an element of {g_i} by sm2.
     *
     *  Requires that the maps share a common set of indices, g, as the
     * dependent and independent indices respectively.
     *
     * @param[in] sm The SparseMap to chain with this instance.
     * @return The chained map.
     * @throw std::runtime_error if the rank of the dependent indices of this
     * instance is not equal to the rank of the independent indices of \p sm.
     */
    template<typename RHpluginplaypIdx>
    SparseMap<IndIndex, RHpluginplaypIdx> chain(
      const SparseMap<DepIndex, RHpluginplaypIdx>& sm) const {
        return chain_(sm);
    }

    /** @brief Determines if two SparseMaps are identical.
     *
     *  Two SparseMaps are the same if they:
     *  - map from the same type of independent/dependent index
     *    - *e.g.* independent indices ar both ElementalIndex and dependent
     *      indices are both TileIndex
     *  - contain the same number of independent-indices
     *  - the set of independent indices is the same, and
     *  - each independent index maps to the same Domain
     *
     *  @param[in] rhs The other SparseMap to compare to.
     *
     *  @return True if this SparseMap is the same as @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const SparseMapBase& rhs) const noexcept;

    /** @brief Adds a string representation of the SparseMap to the stream.
     *
     * @param[in,out] os The stream we are adding the string representation to.
     *                   After this call the stream will contain the string
     *                   representation of the SparseMap.
     *
     * @return @p os with this SparseMap added to it.
     */
    std::ostream& print(std::ostream& os) const;

    /** @brief Adds this SparseMap's state to a hash.
     *
     *  @param[in,out] h The object hashing the SparseMap. After this call the
     *                   internal hash of @p h will be updated to include this
     *                   SparseMap's state.
     */
    void hash(pluginplay::Hasher& h) const;

    // TODO: actual implementation, documentation and testing
    template<typename Archive,
             typename = std::enable_if_t<madness::is_output_archive_v<Archive>>>
    void serialize(Archive ar) const {}

    template<typename Archive,
             typename = std::enable_if_t<madness::is_input_archive_v<Archive>>>
    void serialize(Archive ar) {}

protected:
    /// Ensures the instance has a PIMPL and returns it
    pimpl_type& pimpl_();

    /// Ensures the instance has a PIMPL and returns it in a read-only state
    const pimpl_type& pimpl_() const;

private:
    /// Implements chain when RHpluginplaypIdx == ElementIndex
    SparseMap<IndIndex, ElementIndex> chain_(
      const SparseMap<DepIndex, ElementIndex>& sm) const;

    /// Implements chain when RHpluginplaypIdx == TileIndex
    SparseMap<IndIndex, TileIndex> chain_(
      const SparseMap<DepIndex, TileIndex>& sm) const;

    /// Returns this instance downcasted to the derived class
    DerivedType& downcast_();

    /// Returns this instance as a read-only instance of the derived class
    const DerivedType& downcast_() const;

    /// The instance holding the SparseMap's state
    pimpl_ptr m_pimpl_;
}; // class SparseMapBase

//------------------------------------------------------------------------------
//                               Related Functions
//------------------------------------------------------------------------------

/** @brief Adds a string representation of the SparseMap to the stream.
 *  @related SparseMapBase
 *
 *  This is a convenience function for calling SparseMapBase::print on a stream.
 *
 *  @tparam DerivedType The type of the class being implemented by SparseMapBase
 *  @tparam IndIndex The type of the independent indices.
 *  @tparam DepIndex The type of the dependent indices.
 *
 *  @param[in,out] os The stream we are adding the string representation to.
 *                   After this call the stream will contain the string
 *                   representation of the SparseMap.
 *  @param[in] smb The SparseMap we are printing.
 *
 *  @return @p os with this SparseMap added to it.
 */
template<typename DerivedType, typename IndIndex, typename DepIndex>
std::ostream& operator<<(
  std::ostream& os, const SparseMapBase<DerivedType, IndIndex, DepIndex>& smb) {
    return smb.print(os);
}

/** @brief Determines if two SparseMaps are different.
 *  @relates SparseMapBase
 *
 *  Two SparseMaps are the same if they:
 *  - map from the same type of independent/dependent index
 *    - *e.g.* independent indices ar both ElementalIndex and dependent
 *      indices are both TileIndex
 *  - contain the same number of independent-indices
 *  - the set of independent indices is the same, and
 *  - each independent index maps to the same Domain
 *
 *  @tparam DerivedType Type of the class implemented by the SparseMapBase
 *  @tparam IndIndex Type of the independent indices
 *  @tparam DepIndex Type of the dependent indices
 *
 *  @param[in] lhs The SparseMap on the right side of the operator
 *  @param[in] rhs The SparseMap on the left side of the operator
 *
 *  @return False if this SparseMap is the same as @p rhs and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename DerivedType, typename IndIndex, typename DepIndex>
bool operator!=(const SparseMapBase<DerivedType, IndIndex, DepIndex>& lhs,
                const SparseMapBase<DerivedType, IndIndex, DepIndex>& rhs) {
    return !(lhs == rhs);
}

//------------------------------------------------------------------------------
//              Forward Declare Template Instantiations
//------------------------------------------------------------------------------

#define EXTERN_DECLARE_SPARSEMAPBASE(IndIndex, DepIndex)               \
    extern template class SparseMapBase<SparseMap<IndIndex, DepIndex>, \
                                        IndIndex, DepIndex>

EXTERN_DECLARE_SPARSEMAPBASE(ElementIndex, ElementIndex);
EXTERN_DECLARE_SPARSEMAPBASE(ElementIndex, TileIndex);
EXTERN_DECLARE_SPARSEMAPBASE(TileIndex, ElementIndex);
EXTERN_DECLARE_SPARSEMAPBASE(TileIndex, TileIndex);

#undef EXTERN_DECLARE_SPARSEMAPBASE

} // namespace detail_
} // namespace libchemist::sparse_map
