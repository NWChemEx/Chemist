#include "libchemist/sparse_map/detail_/sparse_map_base.hpp"
#include "libchemist/sparse_map/sparse_map_class.hpp"

#define SPARSEMAPBASE SparseMapBase<DerivedType, IndIndex, DepIndex>

namespace libchemist::sparse_map::detail_ {

template<typename IndIndex, typename DepIndex>
struct SparseMapPIMPL {
    using my_type     = SparseMapPIMPL<IndIndex, DepIndex>;
    using key_type    = IndIndex;
    using mapped_type = Domain<DepIndex>;
    std::map<key_type, mapped_type> m_sm;

    std::unique_ptr<my_type> clone() const {
        return std::make_unique<my_type>(*this);
    }
};

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase() : m_pimpl_(std::make_unique<pimpl_type>()) {}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase(const SparseMapBase& rhs) :
  m_pimpl_(rhs.pimpl_().clone()) {}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase(SparseMapBase&&) noexcept = default;

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE& SPARSEMAPBASE::operator=(const SparseMapBase& rhs) {
    if(this == &rhs) return *this;
    m_pimpl_.swap(rhs.pimpl_().clone());
    return *this;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE& SPARSEMAPBASE::operator=(SparseMapBase&&) noexcept = default;

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::~SparseMapBase() noexcept = default;

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::size_type SPARSEMAPBASE::size() const noexcept {
    if(!m_pimpl_) return 0;
    return pimpl_().m_sm.size();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
bool SPARSEMAPBASE::count(const key_type& i) const noexcept {
    if(!m_pimpl_) return false;
    return pimpl_().m_sm.count(i);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::size_type SPARSEMAPBASE::ind_rank() const noexcept {
    if(!m_pimpl_ || pimpl_().m_sm.empty()) return 0;
    return pimpl_().m_sm.begin()->first.size();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::size_type SPARSEMAPBASE::dep_rank() const noexcept {
    if(!m_pimpl_ || pimpl_().m_sm.empty()) return 0;
    return pimpl_().m_sm.begin()->second.rank();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::iterator SPARSEMAPBASE::begin() {
    return pimpl_().m_sm.begin();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::const_iterator SPARSEMAPBASE::begin() const {
    return pimpl_().m_sm.begin();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::iterator SPARSEMAPBASE::end() {
    return pimpl_().m_sm.end();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::const_iterator SPARSEMAPBASE::end() const {
    return pimpl_().m_sm.end();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
bool SPARSEMAPBASE::operator==(const SparseMapBase& rhs) const noexcept {
    if(!m_pimpl_) return !rhs.m_pimpl_;
    else if(!rhs.m_pimpl_) return false;
    return pimpl_().m_sm == rhs.pimpl_().m_sm;
}
template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::pimpl_type& SPARSEMAPBASE::pimpl_() {
    if(m_pimpl_) return *m_pimpl_;
    throw std::runtime_error("PIMPL not set. Did you move from this instance?");
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const typename SPARSEMAPBASE::pimpl_type& SPARSEMAPBASE::pimpl_() const {
    if(m_pimpl_) return *m_pimpl_;
    throw std::runtime_error("PIMPL not set. Did you move from this instance?");
}

#undef SPARSEMAPBASE

#define DEFINE_SPARSEMAPBASE(IndIndex, DepIndex) \
   template class SparseMapBase<SparseMap<IndIndex, DepIndex>, \
                                IndIndex, DepIndex>

DEFINE_SPARSEMAPBASE(ElementIndex, ElementIndex);
DEFINE_SPARSEMAPBASE(ElementIndex, TileIndex);
DEFINE_SPARSEMAPBASE(TileIndex, ElementIndex);
DEFINE_SPARSEMAPBASE(TileIndex, TileIndex);

#undef DEFINE_SPARSEMAPBASE

}
