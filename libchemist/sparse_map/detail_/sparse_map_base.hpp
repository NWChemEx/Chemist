#pragma once
#include "libchemist/sparse_map/index.hpp"
#include <memory>

namespace libchemist::sparse_map {

template<typename IndexType> class Domain;
template<typename IndIndex, typename DepIndex> class SparseMap;

namespace detail_ {

template<typename IndIndex, typename DepIndex> class SparseMapPIMPL;

template<typename DerivedType, typename IndIndex, typename DepIndex>
class SparseMapBase {
private:
    using my_type    = SparseMapBase<DerivedType, IndIndex, DepIndex>;
    using pimpl_type = SparseMapPIMPL<IndIndex, DepIndex>;
    using pimpl_ptr  = std::unique_ptr<pimpl_type>;
    using stl_set_t  = std::map<IndIndex, Domain<DepIndex>>;
public:
    using size_type = std::size_t;

    using key_type  = IndIndex;

    using iterator  = typename stl_set_t::iterator;

    using const_iterator = typename stl_set_t::const_iterator;

    SparseMapBase();
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

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    bool operator==(const SparseMapBase& rhs) const noexcept;
protected:
    pimpl_type& pimpl_();
    const pimpl_type& pimpl_()const;
private:
    pimpl_ptr m_pimpl_;
};

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
