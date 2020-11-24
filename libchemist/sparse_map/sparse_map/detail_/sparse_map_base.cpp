#include "libchemist/sparse_map/sparse_map/detail_/sparse_map_base.hpp"
#include "libchemist/sparse_map/sparse_map/detail_/sparse_map_pimpl.hpp"
#include "libchemist/sparse_map/sparse_map/detail_/tiled_sparse_map_pimpl.hpp"
#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"

namespace {

template<typename IndIndex, typename DepIndex>
auto new_pimpl() {
    using libchemist::sparse_map::TileIndex;
    using namespace libchemist::sparse_map::detail_;
    using base_pimpl       = SparseMapPIMPL<IndIndex, DepIndex>;
    using tile_pimpl       = TiledSparseMapPIMPL<base_pimpl>;
    constexpr bool is_tile = std::is_same_v<IndIndex, TileIndex>;
    using final_type = std::conditional_t<is_tile, tile_pimpl, base_pimpl>;
    return std::make_unique<final_type>();
}

} // namespace

#define SPARSEMAPBASE SparseMapBase<DerivedType, IndIndex, DepIndex>

namespace libchemist::sparse_map::detail_ {

//------------------------------------------------------------------------------
//                            CTors
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase() : m_pimpl_(new_pimpl<IndIndex, DepIndex>()) {}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase(il_t il) : SparseMapBase() {
    for(auto&& [k, v] : il)
        for(const auto& dep : v) add_to_domain(k, dep);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase(const SparseMapBase& rhs) : SparseMapBase() {
    if(rhs.m_pimpl_) rhs.pimpl_().clone().swap(m_pimpl_);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::SparseMapBase(SparseMapBase&&) noexcept = default;

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE& SPARSEMAPBASE::operator=(const SparseMapBase& rhs) {
    if(this == &rhs) return *this;
    rhs.pimpl_().clone().swap(m_pimpl_);
    return *this;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE& SPARSEMAPBASE::operator=(SparseMapBase&&) noexcept = default;

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE::~SparseMapBase() noexcept = default;

//------------------------------------------------------------------------------
//                                 Accessors
//------------------------------------------------------------------------------

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::size_type SPARSEMAPBASE::size() const noexcept {
    return m_pimpl_ ? pimpl_().size() : 0;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
bool SPARSEMAPBASE::count(const key_type& i) const noexcept {
    if(m_pimpl_)
        return pimpl_().count(i);
    else
        return false;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::size_type SPARSEMAPBASE::ind_rank() const noexcept {
    if(!m_pimpl_)
        return 0;
    else
        return pimpl_().ind_rank();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::size_type SPARSEMAPBASE::dep_rank() const noexcept {
    if(!m_pimpl_)
        return 0;
    else
        return pimpl_().dep_rank();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
void SPARSEMAPBASE::add_to_domain(const key_type& key, DepIndex value) {
    if(!m_pimpl_) m_pimpl_ = new_pimpl<IndIndex, DepIndex>();
    pimpl_().add_to_domain(key, std::move(value));
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const typename SPARSEMAPBASE::value_type& SPARSEMAPBASE::operator[](
  size_type i) const {
    return pimpl_().at(i);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const typename SPARSEMAPBASE::mapped_type& SPARSEMAPBASE::operator[](
  const key_type& key) const {
    if(!m_pimpl_) {
        std::stringstream ss;
        ss << "Index: " << key << " is not in range [0, 0)";
        throw std::out_of_range(ss.str());
    }
    return pimpl_().at(key);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType SPARSEMAPBASE::direct_product(const SparseMapBase& rhs) const {
    if(!m_pimpl_ || empty()) return DerivedType{};
    if(!rhs.m_pimpl_ || rhs.empty()) { return DerivedType{}; }
    DerivedType rv(downcast_());
    rv.pimpl_().direct_product_assign(rhs.pimpl_());
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType SPARSEMAPBASE::operator*(const SparseMapBase& rhs) const {
    DerivedType rv(downcast_());
    rv *= rhs;
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType& SPARSEMAPBASE::operator*=(const SparseMapBase& rhs) {
    if(!m_pimpl_ || !rhs.m_pimpl_) {
        m_pimpl_ = new_pimpl<IndIndex, DepIndex>();
        return downcast_();
    }
    pimpl_() *= rhs.pimpl_();
    return downcast_();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType& SPARSEMAPBASE::operator^=(const SparseMapBase& rhs) {
    if(!m_pimpl_ || !rhs.m_pimpl_) {
        m_pimpl_ = new_pimpl<IndIndex, DepIndex>();
        return downcast_();
    }
    pimpl_() ^= rhs.pimpl_();
    return downcast_();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType SPARSEMAPBASE::operator^(const SparseMapBase& rhs) const {
    DerivedType rv(downcast_());
    rv ^= rhs;
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SparseMap<DepIndex, IndIndex> SPARSEMAPBASE::inverse() const {
    SparseMap<DepIndex, IndIndex> rv;
    if(empty()) return rv;

    for(const auto& [ind, domain] : *this) {
        for(const auto& dep : domain) rv.add_to_domain(dep, ind);
    }
    if constexpr(std::is_same_v<IndIndex, TileIndex>) {
        auto tr = downcast_().trange();
        if(tr != TA::TiledRange{}) rv.set_domain_trange(tr);
    }
    if constexpr(std::is_same_v<DepIndex, TileIndex>) {
        auto tr = begin()->second.trange();
        if(tr != TA::TiledRange{}) rv.set_trange(tr);
    }
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType& SPARSEMAPBASE::operator+=(const SparseMapBase& rhs) {
    if(!m_pimpl_) {
        if(!rhs.m_pimpl_)
            m_pimpl_ = new_pimpl<IndIndex, DepIndex>();
        else
            rhs.pimpl_().clone().swap(m_pimpl_);
        return downcast_();
    } else if(!rhs.m_pimpl_)
        return downcast_();
    pimpl_() += rhs.pimpl_();
    return downcast_();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType SPARSEMAPBASE::operator+(const SparseMapBase& rhs) const {
    auto rv = DerivedType(downcast_());
    rv += rhs;
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
bool SPARSEMAPBASE::operator==(const SparseMapBase& rhs) const noexcept {
    if(!m_pimpl_)
        return !rhs.m_pimpl_ || rhs.empty();
    else if(!rhs.m_pimpl_)
        return empty();
    return pimpl_() == rhs.pimpl_();
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
void SPARSEMAPBASE::hash(sde::Hasher& h) const {
    if(m_pimpl_)
        pimpl_().hash(h);
    else
        h(nullptr);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
std::ostream& SPARSEMAPBASE::print(std::ostream& os) const {
    os << pimpl_();
    return os;
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

//------------------------------------------------------------------------------
//                            Private Methods
//------------------------------------------------------------------------------
template<typename IndIndex, typename DepIndex, typename NewDepIndex>
SparseMap<IndIndex, NewDepIndex> chain_guts(
  const SparseMap<IndIndex, DepIndex>& lhs,
  const SparseMap<DepIndex, NewDepIndex>& rhs) {
    if(lhs.dep_rank() != rhs.ind_rank())
        throw std::runtime_error(
          "Incompatible index ranks between chained maps");
    SparseMap<IndIndex, NewDepIndex> rv;
    if constexpr(std::is_same_v<IndIndex, TileIndex>) {
        rv.set_trange(lhs.trange());
    }
    for(const auto& [lind, ldom] : lhs) {
        for(const auto& ldep : ldom) {
            if(rhs.count(ldep)) {
                for(const auto& rdep : rhs.at(ldep)) {
                    rv.add_to_domain(lind, rdep);
                }
            }
        }
    }
    if(rv.empty()) return rv;
    if constexpr(std::is_same_v<NewDepIndex, TileIndex>) {
        auto tr = rhs.begin()->second.trange();
        if(tr != TA::TiledRange{}) rv.set_domain_trange(tr);
    }
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SparseMap<IndIndex, ElementIndex> SPARSEMAPBASE::chain_(
  const SparseMap<DepIndex, ElementIndex>& sm) const {
    return chain_guts(downcast_(), sm);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SparseMap<IndIndex, TileIndex> SPARSEMAPBASE::chain_(
  const SparseMap<DepIndex, TileIndex>& sm) const {
    return chain_guts(downcast_(), sm);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType& SPARSEMAPBASE::downcast_() {
    return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const DerivedType& SPARSEMAPBASE::downcast_() const {
    return static_cast<const DerivedType&>(*this);
}

#undef SPARSEMAPBASE

//------------------------------------------------------------------------------
//                          Template Instantiations
//------------------------------------------------------------------------------

#define DEFINE_SPARSEMAPBASE(IndIndex, DepIndex)                          \
    template class SparseMapBase<SparseMap<IndIndex, DepIndex>, IndIndex, \
                                 DepIndex>

DEFINE_SPARSEMAPBASE(ElementIndex, ElementIndex);
DEFINE_SPARSEMAPBASE(ElementIndex, TileIndex);
DEFINE_SPARSEMAPBASE(TileIndex, ElementIndex);
DEFINE_SPARSEMAPBASE(TileIndex, TileIndex);

#undef DEFINE_SPARSEMAPBASE

} // namespace libchemist::sparse_map::detail_
