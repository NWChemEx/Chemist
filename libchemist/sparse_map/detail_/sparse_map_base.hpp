#pragma once
#include "libchemist/sparse_map/domain.hpp"
#include "libchemist/sparse_map/index.hpp"
#include <memory>

namespace libchemist::sparse_map {

// Forward declare SparseMap class (needed for template instantiations)
template<typename IndIndex, typename DepIndex> class SparseMap;

namespace detail_ {

// Forward declare the SparseMapPIMPL class
template<typename IndIndex, typename DepIndex> class SparseMapPIMPL;


template<typename DerivedType, typename IndIndex, typename DepIndex>
class SparseMapBase {
private:
    using my_type    = SparseMapBase<DerivedType, IndIndex, DepIndex>;
    using pimpl_type = SparseMapPIMPL<IndIndex, DepIndex>;
    using pimpl_ptr  = std::unique_ptr<pimpl_type>;
    using stl_set_t  = std::map<IndIndex, Domain<DepIndex>>;
    using domain_il  = std::initializer_list<DepIndex>;
    using il_t       = std::initializer_list<std::pair<IndIndex, domain_il>>;
public:
    using size_type   = std::size_t;

    using key_type    = IndIndex;
    using mapped_type = Domain<DepIndex>;
    using iterator    = typename stl_set_t::iterator;

    using const_iterator = typename stl_set_t::const_iterator;

    SparseMapBase();
    SparseMapBase(il_t il);
    SparseMapBase(const SparseMapBase& rhs);
    SparseMapBase(SparseMapBase&& rhs) noexcept;
    SparseMapBase& operator=(const SparseMapBase& rhs);
    SparseMapBase& operator=(SparseMapBase&& rhs) noexcept;
    ~SparseMapBase() noexcept;

    void swap(DerivedType& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }

    size_type size() const noexcept;
    bool empty() const noexcept { return begin() == end(); }
    bool count(const key_type& i) const noexcept;
    size_type ind_rank() const noexcept;

    /** @brief The number of independent modes associated with each Domain.
     *
     *  The
     * @return
     */
    size_type dep_rank() const noexcept;


    void add_to_domain(const key_type& key, DepIndex value);

    mapped_type& operator[](const key_type& key);
    const mapped_type& operator[](const key_type& key) const;

    template<typename...Args>
    auto& at(Args&&...args);

    template<typename...Args>
    const auto& at(Args&&...args) const;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;


    DerivedType operator*(const DerivedType& rhs) const;
    SparseMapBase& operator*=(const SparseMapBase& rhs);

    /** @brief Constructs the inverse SparseMap.
     *
     *  Creates a SparseMap which maps from the dependent indices to the
     *  independent indices of the current map.
     *
     *  sparse_map.inverse().inverse() == sparse_map
     *
     * @return The inverse of the SparseMap.
     */
    SparseMapBase<DerivedType, DepIndex, IndIndex> inverse() const;

    /** @brief Creates a SparseMap from chaining two maps together.
     *
     *  Given the SparseMaps sm1(f -> g) and sm2(g -> h),
     *  the chained map from (f -> h) is formed by mapping each element
     *  f_i to a set of {g_i} using sm1, and then to any element in h which is mapped to from
     *  an element of {g_i} by sm2.
     *
     *  Requires that the maps share a common set of indices, g, as the dependent and
     *  independent indices respectively.
     *
     * @param[in] sm The SparseMap to chain with this instance.
     * @return The chained map.
     * @throw std::runtime_error if the rank of the dependent indices of this instance
     *                           is not equal to the rank of the independent indices
     *                           of \p sm.
     */
    //SparseMapBase chain(const SparseMapBase& sm) const;

    /** @brief Creates a SparseMap which is the union of two maps.
     *
     *  Given two SparseMaps sm1(f -> g) and sm2(f -> g), the union is formed by
     *  mapping f_i to any element in g which f_i is mapped to by either sm1 or sm2.
     *
     *  Requires that either one of the sets is empty or both sets have the same
     *  rank in independent and dependent indices.
     *
     * @param[in] sm The SparseMap to take the union with this instance.
     * @return The union of the two maps.
     * @throw std::runtime_error if neither map is empty and the rank of the dependent
     *                           indices of this instance is not equal to the rank of
     *                           the independent indices of \p sm.
     */
    SparseMapBase map_union(const SparseMapBase& sm) const;

    /** @brief
     *
     *  Given two SparseMaps sm1(f -> g) and sm2(f -> g), the intersection is formed by
     *  mapping f_i to any element in g which f_i is mapped to by both sm1 and sm2.
     *
     * @param[in] sm The SparseMap to take the intersection with this instance.
     * @return The intersection of the two maps.
     * @throw std::runtime_error if neither map is empty and the rank of the dependent
     *                           indices of this instance is not equal to the rank of
     *                           the independent indices of \p sm.
     */
    SparseMapBase intersection(const SparseMapBase& sm) const;
    bool operator==(const SparseMapBase& rhs) const noexcept;

    std::ostream& print(std::ostream& os) const;
    void hash(sde::Hasher& h) const;
protected:
    pimpl_type& pimpl_();
    const pimpl_type& pimpl_() const;
private:
    mapped_type& at_(const key_type& key);
    const mapped_type& at_(const key_type& key) const;

    DerivedType& downcast_();
    const DerivedType& downcast_() const;

    /** @brief Code factorization for ensuring an input index has the correct
     *         rank.
     *
     *  This function is used internally to ensure that a user-provided index
     *  has a rank consistent with the indices already in the SparseMap
     *  instance.
     *
     *  @param[in] idx_rank The rank of the incoming index. Will be compared
     *                      against ind_rank()/dep_rank() if @p dep is
     *                      false/true.
     *  @param[in] dep True if we should compare to the rank of the independent
     *                 indices and false otherwise.
     *  @throws std::runtime_error if the rank of the index is not the same as
     *                             the rank of the indices already in this
     *                             instance. Strong throw guarantee.
     */
    void check_rank_(size_type idx_rank, bool dep) const;

    pimpl_ptr m_pimpl_;
};

template<typename DerivedType, typename IndIndex, typename DepIndex>
std::ostream& operator<<(
  std::ostream& os, const SparseMapBase<DerivedType, IndIndex, DepIndex>& smb) {
    return smb.print(os);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
bool operator!=(const SparseMapBase<DerivedType, IndIndex, DepIndex>& lhs,
                const SparseMapBase<DerivedType, IndIndex, DepIndex>& rhs) {
    return !(lhs == rhs);
}

//------------------------------------------------------------------------------
//                           Inline Implementations
//------------------------------------------------------------------------------

#define SPARSEMAPBASE SparseMapBase<DerivedType, IndIndex, DepIndex>

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase(il_t il) :
  SparseMapBase() {
    for(auto&& [k, v] : il) (*this)[k] = mapped_type(v);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
template<typename...Args>
auto& SPARSEMAPBASE::at(Args&&...args) {
    return at_(key_type(std::forward<Args>(args)...));
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
template<typename...Args>
const auto& SPARSEMAPBASE::at(Args&&...args) const{
    return at_(key_type(std::forward<Args>(args)...));
}

#undef SPARSEMAPBASE

//------------------------------------------------------------------------------
//              Forward Declare Template Instantiations
//------------------------------------------------------------------------------

#define EXTERN_DECLARE_SPARSEMAPBASE(IndIndex, DepIndex) \
    extern template class SparseMapBase<SparseMap<IndIndex, DepIndex>, \
                                        IndIndex, DepIndex>

EXTERN_DECLARE_SPARSEMAPBASE(ElementIndex, ElementIndex);
EXTERN_DECLARE_SPARSEMAPBASE(ElementIndex, TileIndex);
EXTERN_DECLARE_SPARSEMAPBASE(TileIndex, ElementIndex);
EXTERN_DECLARE_SPARSEMAPBASE(TileIndex, TileIndex);

#undef EXTERN_DECLARE_SPARSEMAPBASE

} // namespace detail_
} // namespace libchemist::sparse_map
