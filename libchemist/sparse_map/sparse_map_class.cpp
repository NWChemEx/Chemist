#include "libchemist/sparse_map/sparse_map_class.hpp"

#define SPARSEMAP SparseMap<IndIndex, DepIndex>

namespace libchemist::sparse_map {

index_set_map SparseMap::indices() const {
    index_set_map rv;
    for(const auto& [k, v] : *this){
        rv[k] = indices(k);
    }
    return rv;
}

index_set_array SparseMap::indices(key_type ind) const {
    index_set_array rv;
    rv.reserve(dep_rank());
    for(size_type i = 0; i < dep_rank(); ++i)
        rv.push_back(indices(ind, i));
    return rv;
}

index_set SparseMap::indices(key_type ind, size_type mode) const {
    index_set rv;
    for(const auto& dep : at(ind))
        rv.insert(dep.m_index[mode]);
    return rv;
}
mapped_type& SparseMap::at_(const key_type& ind) {
    return m_pimpl_->m_sm.at(ind);
}

const mapped_type& SparseMap::at_(const key_type& ind) const {
    return m_pimpl_->m_sm.at(ind);
}



void SparseMap::add_to_domain(key_type ind, key_type dep) {
    if(m_pimpl_->m_sm.empty()){
        m_pimpl_->m_sm.emplace(std::move(ind), Domain{std::move(dep)});
        return;
    }
    check_rank_(ind.size(), false);
    check_rank_(dep.size(), true);
    m_pimpl_->m_sm[ind].insert(dep);
}

SparseMap SparseMap::operator*(const SparseMap& rhs) const {
    SparseMap rv;
    const auto& lsm = m_pimpl_->m_sm;
    const auto& rsm = rhs.m_pimpl_->m_sm;
    auto& osm = rv.m_pimpl_->m_sm;
    for(const auto& [k, v] : lsm)
        if(rsm.count(k))
            osm[k] = lsm.at(k) * rsm.at(k);
    return rv;
}

SparseMap& SparseMap::operator*=(const SparseMap& rhs) {
    auto temp = (*this) * rhs;
    swap(temp);
    return *this;
}



void SparseMap::hash(sde::Hasher& h) const {
    h(m_pimpl_->m_sm);
}

std::ostream& SparseMap::print(std::ostream& os) const {
    using utilities::printing::operator<<;
    os << m_pimpl_->m_sm;
    return os;
}

void SparseMap::check_rank_(size_type idx_rank, bool dep) const {
    const auto corr_rank = dep ? dep_rank() : ind_rank();
    if(corr_rank == idx_rank) return;
    using namespace std::string_literals;
    const std::string iden = dep ? "Dependent" : "Independent";
    throw std::runtime_error(
      iden + " indices have ranks of "s + std::to_string(corr_rank) +
      " but idx with rank " + std::to_string(idx_rank) + " was provided."s
    );
}

SparseMap SparseMap::inverse() const {
    SparseMap rv;
    for (const auto& x : *this) {
        for (const auto y : x.second) {
            rv.add_to_domain(y,x.first);
        }
    }
    return rv;
}

SparseMap SparseMap::chain(const SparseMap& sm) const {
    if (this->dep_rank() != sm.ind_rank())
        throw std::runtime_error("Incompatible index ranks between chained maps");
    SparseMap rv;
    for (const auto& x : *this) {
        for (const auto y : x.second) {
            if (sm.count(y)) {
                for (const auto z : sm.at(y)) {
                    rv.add_to_domain(x.first, z);
                }
            }
        }
    }
    return rv;
}

SparseMap SparseMap::map_union(const SparseMap& sm) const {
    if (!this->empty() && !sm.empty() &&
        (this->dep_rank() != sm.dep_rank() || this->ind_rank() != sm.ind_rank()))
        throw std::runtime_error("Incompatible index ranks between maps for union");
    SparseMap rv = *this;
    for (const auto& x : sm) {
        for (const auto y : x.second) {
            rv.add_to_domain(x.first,y);
        }
    }
    return rv;
}

SparseMap SparseMap::intersection(const SparseMap& sm) const {
    if (!this->empty() && !sm.empty() &&
        (this->dep_rank() != sm.dep_rank() || this->ind_rank() != sm.ind_rank()))
        throw std::runtime_error("Incompatible index ranks between maps for intersection");
    SparseMap rv;
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

} // namespace libchemist::sparse_map
