#include "libchemist/sparse_map/sparse_map_class.hpp"

namespace libchemist::sparse_map {

//index_set_map SparseMap::indices() const {
//    index_set_map rv;
//    for(const auto& [k, v] : *this){
//        rv[k] = indices(k);
//    }
//    return rv;
//}
//
//index_set_array SparseMap::indices(key_type ind) const {
//    index_set_array rv;
//    rv.reserve(dep_rank());
//    for(size_type i = 0; i < dep_rank(); ++i)
//        rv.push_back(indices(ind, i));
//    return rv;
//}
//
//index_set SparseMap::indices(key_type ind, size_type mode) const {
//    index_set rv;
//    for(const auto& dep : at(ind))
//        rv.insert(dep.m_index[mode]);
//    return rv;
//}
//
//void SparseMap::add_to_domain(key_type ind, key_type dep) {
//    if(m_pimpl_->m_sm.empty()){
//        m_pimpl_->m_sm.emplace(std::move(ind), Domain{std::move(dep)});
//        return;
//    }
//    check_rank_(ind.size(), false);
//    check_rank_(dep.size(), true);
//    m_pimpl_->m_sm[ind].insert(dep);
//}

//------------------------------------------------------------------------------
//                            Template Instantiations
//------------------------------------------------------------------------------

template class SparseMap<ElementIndex, ElementIndex>;
template class SparseMap<ElementIndex, TileIndex>;
template class SparseMap<TileIndex, ElementIndex>;
template class SparseMap<TileIndex, TileIndex>;

} // namespace libchemist::sparse_map
