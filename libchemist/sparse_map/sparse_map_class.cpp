#include "libchemist/sparse_map/sparse_map_class.hpp"

namespace libchemist::sparse_map {

#define SPARSEMAPEE SparseMap<ElementIndex, ElementIndex>
#define SPARSEMAPET SparseMap<ElementIndex, TileIndex>
#define SPARSEMAPTE SparseMap<TileIndex, ElementIndex>
#define SPARSEMAPTT SparseMap<TileIndex, TileIndex>

SPARSEMAPEE::SparseMap(const SPARSEMAPET& other) {
    for(const auto& [oeidx, d] : other){
        for(const auto& itidx : d){
            for(const auto& ieidx : d.trange().make_tile_range(itidx)){
                const ElementIndex temp(ieidx.begin(), ieidx.end());
                add_to_domain(oeidx, temp);
            }
        }
    }
}

SPARSEMAPEE::SparseMap(const SPARSEMAPTE& other) {
    const auto& trange = other.trange();
    for(const auto& [otidx, d] : other) {
        for(const auto& oeidx : trange.make_tile_range(otidx)){
            for(const auto& ieidx : d){
                const ElementIndex temp(oeidx.begin(), oeidx.end());
                add_to_domain(temp, ieidx);
            }
        }
    }
}

SPARSEMAPEE::SparseMap(const SPARSEMAPTT& other) {
    const auto& trange = other.trange();
    for(const auto& [otidx, d] : other){
        for(const auto& oeidx : trange.make_tile_range(otidx)){
            const ElementIndex otemp(oeidx.begin(), oeidx.end());
            for(const auto& itidx : d){
                for(const auto& ieidx : d.trange().make_tile_range(itidx)){
                    const ElementIndex itemp(ieidx.begin(), ieidx.end());
                    add_to_domain(otemp, itemp);
                }
            }
        }
    }
}

SPARSEMAPTE::SparseMap(const SPARSEMAPTT& other) : m_trange_(other.trange()) {
    for(const auto& [otidx, d] : other) {
        const auto& itrange = d.trange();
        for(const auto& itidx : d) {
            for(const auto& ieidx : itrange.make_tile_range(itidx)) {
                ElementIndex new_idx(ieidx.begin(), ieidx.end());
                add_to_domain(otidx, std::move(new_idx));
            }
        }
    }
}

#undef SPARSEMAPEE
#undef SPARSEMAPET
#undef SPARSEMAPTE
#undef SPARSEMAPTT

template class SparseMap<ElementIndex, ElementIndex>;
template class SparseMap<ElementIndex, TileIndex>;
template class SparseMap<TileIndex, ElementIndex>;
template class SparseMap<TileIndex, TileIndex>;

} // namespace libchemist::sparse_map
