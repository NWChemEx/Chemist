#pragma once
#include "libchemist/sparse_map/domain.hpp"
#include <utility>

namespace libchemist::sparse_map {
namespace detail_ {
class SparseMapPIMPL;
}

/** @brief Stores the sparsity relation from one index space to another
 *
 *  The SparseMap class assumes that the modes of a tensor can be classified as
 *  either independent or dependent.
 *
 */
class SparseMap {
private:
    template<typename T>
    using il_type =
      std::initializer_list<std::pair<T, std::initializer_list<T>>>;

    using stl_map_t = std::map<std::vector<std::size_t>, Domain>;
public:
    using mapped_type     = Domain;
    using key_type        = typename mapped_type::value_type;
    using value_type      = std::pair<const key_type, mapped_type>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using iterator        = typename stl_map_t::iterator;
    using const_iterator  = typename stl_map_t::const_iterator;
    using size_type       = typename mapped_type::size_type;

    SparseMap();
    SparseMap(const SparseMap& rhs);
    SparseMap(SparseMap&& rhs) noexcept;
    SparseMap& operator=(const SparseMap& rhs);
    SparseMap& operator=(SparseMap&& rhs) noexcept;
    SparseMap(il_type<key_type> il);
    ~SparseMap() noexcept;

    void swap(SparseMap& rhs) noexcept { m_pimpl_.swap(rhs.m_pimpl_); }

    size_type size() const noexcept;
    bool empty() const noexcept { return begin() == end(); }
    bool count(const key_type& i) const noexcept;
    size_type ind_rank() const noexcept;
    size_type dep_rank() const noexcept;

    mapped_type& operator[](const key_type& i);
    const mapped_type& operator[](const key_type& i)const;
    mapped_type& at(const key_type& i);
    const mapped_type& at(const key_type& i) const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;

    /** @brief Adds single rank indices to the domain of a single rank index.
     *
     *  This is a convenience function for creating a SparseMap from single rank
     *  indices to single rank indices, which does not require you to wrap the
     *  input indices in an std::vector.
     *
     *  @param[in] ind The value of the independent index.
     *  @param[in] dep The value of the dependent index to add to @p ind 's
     *                 domain.
     */
    void add_to_domain(size_type ind, size_type dep);

    /** @brief Adds multi-rank indices to the domain of a single rank index.
     *
     *  This is a convenience function for creating a SparseMap from single rank
     *  indices to (possibly) multi-rank indices. It is designed so that you do
     *  not have to wrap the input indices in an std::vector.
     *
     *  @param[in] ind The value of the independent index.
     *  @param[in] dep The value of the dependent index to add to @p ind 's
     *                 domain.
     */
    void add_to_domain(size_type ind, key_type dep);
    void add_to_domain(key_type ind, key_type dep);
    template<typename BeginItr, typename EndItr>
    void add_to_domain(key_type ind, BeginItr first, EndItr last);

    SparseMap operator*(const SparseMap& rhs) const;
    SparseMap operator+(const SparseMap& rhs) const;

    bool operator==(const SparseMap& rhs) const noexcept;
    bool operator!=(const SparseMap& rhs) const noexcept;

    void hash(sde::Hasher& h) const;

    std::ostream& print(std::ostream& os) const;

private:

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

    std::unique_ptr<detail_::SparseMapPIMPL> m_pimpl_;
}; // class SparseMap

inline std::ostream& operator<<(std::ostream& os, const SparseMap& sm) {
    return sm.print(os);
}

//------------------------------------------------------------------------------
// Inline Implementations
//------------------------------------------------------------------------------

inline SparseMap::SparseMap(il_type<key_type> il) : SparseMap() {
    for(auto&& [k, v] : il) add_to_domain(k, v.begin(), v.end());
}

inline
typename SparseMap::mapped_type& SparseMap::operator[](const key_type& i) {
    return at(i);
}

inline const typename SparseMap::mapped_type&
SparseMap::operator[](const key_type& i) const{
    return at(i);
}

inline void SparseMap::add_to_domain(size_type ind, size_type dep) {
    add_to_domain(key_type{ind}, key_type{dep});
}

inline void SparseMap::add_to_domain(size_type ind, key_type dep){
    add_to_domain(key_type{ind}, std::move(dep));
}

template<typename BeginItr, typename EndItr>
void SparseMap::add_to_domain(key_type ind, BeginItr first, EndItr last){
    if(first == last) throw std::runtime_error("Range can not be empty");
    std::for_each(first, last, [=](auto&& i){ add_to_domain(ind, i); });
}

inline bool SparseMap::operator!=(const SparseMap& rhs) const noexcept {
    return !((*this) == rhs);
}

} // namespace mp2