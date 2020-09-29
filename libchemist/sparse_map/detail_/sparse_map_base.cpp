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
    rhs.pimpl_().clone().swap(m_pimpl_);
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
void SPARSEMAPBASE::add_to_domain(const key_type& key, DepIndex value) {
    pimpl_().m_sm[key].insert(std::move(value));
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::mapped_type&
SPARSEMAPBASE::operator[](const key_type& key) {
    return pimpl_().m_sm[key];
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const typename SPARSEMAPBASE::mapped_type&
SPARSEMAPBASE::operator[](const key_type& key) const {
    return pimpl_().m_sm.at(key);
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
DerivedType SPARSEMAPBASE::operator*(const DerivedType& rhs) const {
    DerivedType rv(downcast_());
    rv *= rhs;
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE& SPARSEMAPBASE::operator*=(const SparseMapBase& rhs) {
    auto new_pimpl = pimpl_().clone();
    return *this;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SparseMapBase<DerivedType, DepIndex, IndIndex>
SPARSEMAPBASE::inverse() const {
    SparseMapBase<DerivedType, DepIndex, IndIndex> rv;
    for (const auto& x : *this) {
        for (const auto y : x.second) {
            rv.add_to_domain(y,x.first);
        }
    }
    return rv;
}

//template<typename DerivedType, typename IndIndex, typename DepIndex>
//SPARSEMAPBASE SPARSEMAPBASE::chain(const SparseMapBase& sm) const {
//    if (this->dep_rank() != sm.ind_rank())
//        throw std::runtime_error("Incompatible index ranks between chained maps");
//    SparseMapBase rv;
//    for (const auto& x : *this) {
//        for (const auto y : x.second) {
//            if (sm.count(y)) {
//                for (const auto z : sm.at(y)) {
//                    rv.add_to_domain(x.first, z);
//                }
//            }
//        }
//    }
//    return rv;
//}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE SPARSEMAPBASE::map_union(const SparseMapBase& sm) const {
    if (!this->empty() && !sm.empty() &&
        (this->dep_rank() != sm.dep_rank() || this->ind_rank() != sm.ind_rank()))
        throw std::runtime_error("Incompatible index ranks between maps for union");
    SparseMapBase rv = *this;
    for (const auto& x : sm) {
        for (const auto y : x.second) {
            rv.add_to_domain(x.first,y);
        }
    }
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
SPARSEMAPBASE SPARSEMAPBASE::intersection(const SparseMapBase& sm) const {
    if (!this->empty() && !sm.empty() &&
        (this->dep_rank() != sm.dep_rank() || this->ind_rank() != sm.ind_rank()))
        throw std::runtime_error("Incompatible index ranks between maps for intersection");
    SparseMapBase rv;
    for (const auto& x : *this) {
        if (sm.count(x.first)) {
            for (const auto y : x.second) {
                if (sm.at(x.first).count(y)) {
                    rv.add_to_domain(x.first,y);
                }
            }
        }
    }
    return rv;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
bool SPARSEMAPBASE::operator==(const SparseMapBase& rhs) const noexcept {
    if(!m_pimpl_) return !rhs.m_pimpl_;
    else if(!rhs.m_pimpl_) return false;
    return pimpl_().m_sm == rhs.pimpl_().m_sm;
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
void SPARSEMAPBASE::hash(sde::Hasher& h) const {
    h(pimpl_().m_sm);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
std::ostream& SPARSEMAPBASE::print(std::ostream& os) const {
    using utilities::printing::operator<<;
    os << pimpl_().m_sm;
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

template<typename DerivedType, typename IndIndex, typename DepIndex>
typename SPARSEMAPBASE::mapped_type& SPARSEMAPBASE::at_(const key_type& key) {
    return pimpl_().m_sm.at(key);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const typename SPARSEMAPBASE::mapped_type&
SPARSEMAPBASE::at_(const key_type& key) const {
    return pimpl_().m_sm.at(key);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
DerivedType& SPARSEMAPBASE::downcast_() {
    return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
const DerivedType& SPARSEMAPBASE::downcast_() const {
    return static_cast<const DerivedType&>(*this);
}

template<typename DerivedType, typename IndIndex, typename DepIndex>
void SPARSEMAPBASE::check_rank_(size_type idx_rank, bool dep) const {
    const auto corr_rank = dep ? dep_rank() : ind_rank();
    if(corr_rank == idx_rank) return;
    using namespace std::string_literals;
    const std::string iden = dep ? "Dependent" : "Independent";
    throw std::runtime_error(
      iden + " indices have ranks of "s + std::to_string(corr_rank) +
      " but idx with rank " + std::to_string(idx_rank) + " was provided."s
    );
}

#undef SPARSEMAPBASE

//------------------------------------------------------------------------------
//                          Template Instantiations
//------------------------------------------------------------------------------

#define DEFINE_SPARSEMAPBASE(IndIndex, DepIndex) \
   template class SparseMapBase<SparseMap<IndIndex, DepIndex>, \
                                IndIndex, DepIndex>

DEFINE_SPARSEMAPBASE(ElementIndex, ElementIndex);
DEFINE_SPARSEMAPBASE(ElementIndex, TileIndex);
DEFINE_SPARSEMAPBASE(TileIndex, ElementIndex);
DEFINE_SPARSEMAPBASE(TileIndex, TileIndex);

#undef DEFINE_SPARSEMAPBASE

}
