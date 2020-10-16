//#include "libchemist/sparse_map/domain.hpp"
//#include "libchemist/ta_helpers/get_block_idx.hpp"
//#include <string>
//
//namespace libchemist::sparse_map {
//
//Domain<ElementIndex>::Domain(const Domain<TileIndex>& other) : Domain() {
//    for(const auto& tidx : other){
//        for(const auto& eidx : other.trange().make_tile_range(tidx)){
//            insert(ElementIndex(eidx.begin(), eidx.end()));
//        }
//    }
//}
//
//Domain<TileIndex>::Domain(const tiled_range_type& trange,
//                          std::initializer_list<TileIndex> il) :
//  base_type(il), m_trange_(trange) {
//
//    for(const auto& x: *this)
//        if(!trange.tiles_range().includes(x)) {
//            std::stringstream ss, ss1;
//            ss  << x;
//            ss1 << trange;
//            throw std::out_of_range("Initial tile index: " + ss.str() +
//                                    " is not in the TiledRange: " + ss1.str());
//        }
//}
//
//Domain<TileIndex>::Domain(const TA::TiledRange& trange,
//                          const Domain<ElementIndex>& d) : m_trange_(trange) {
//    const auto& erange = trange.elements_range();
//    for(const auto& x : d){
//        if(!erange.includes(x)){
//            std::stringstream ss, ss1;
//            ss  << x;
//            ss1 << trange;
//            throw std::out_of_range("Initial element index: " + ss.str() +
//                                    " is not in the TiledRange: " + ss1.str());
//        }
//        const auto t_cardinal_index = trange.element_to_tile(x.m_index);
//        const auto tidx = trange.tiles_range().idx(t_cardinal_index);
//        insert(TileIndex(tidx.begin(), tidx.end()));
//    }
//}
//
//void Domain<TileIndex>::set_trange(const tiled_range_type& trange) {
//    // If we are empty any trange is fine
//    if(empty()) {
//        m_trange_.emplace(trange);
//        return;
//    }
//
//    // Otherwise the indices in this Domain must be in the trange
//    for(const auto& idx : *this){
//        if(!trange.tiles_range().includes(idx)) {
//            std::stringstream ss, ss1;
//            ss << trange;
//            ss1 << idx;
//            throw std::out_of_range("The provided TiledRange: " + ss.str() +
//                                     "is incompatible with the index: " +
//                                     ss1.str() + " already in this Domain");
//        }
//    }
//
//    m_trange_.emplace(trange);
//}
//
//template class Domain<ElementIndex>;
//template class Domain<TileIndex>;
//
//} // namespace libchemist::sparse_map
