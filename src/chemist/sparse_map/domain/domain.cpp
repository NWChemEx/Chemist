#include "detail_/tiled_domain_pimpl.hpp"
#include "chemist/sparse_map/domain/domain.hpp"
#include "chemist/ta_helpers/get_block_idx.hpp"
#include <string>

namespace chemist::sparse_map {

Domain<ElementIndex>::Domain(const Domain<TileIndex>& other) : Domain() {
    for(const auto& tidx : other) {
        for(const auto& eidx : other.trange().make_tile_range(tidx)) {
            insert(ElementIndex(eidx.begin(), eidx.end()));
        }
    }
}

Domain<TileIndex>::Domain(const tiled_range_type& trange,
                          std::initializer_list<TileIndex> il) :
  base_type(il) {
    set_trange(trange);
}

Domain<TileIndex>::Domain(const TA::TiledRange& trange,
                          const Domain<ElementIndex>& d) {
    set_trange(trange);
    const auto& erange = trange.elements_range();
    for(const auto& x : d) {
        if(!erange.includes(x)) {
            std::stringstream ss, ss1;
            ss << x;
            ss1 << trange;
            throw std::out_of_range("Initial element index: " + ss.str() +
                                    " is not in the TiledRange: " + ss1.str());
        }
        const auto t_cardinal_index = trange.element_to_tile(x.m_index);
        const auto tidx = trange.tiles_range().idx(t_cardinal_index);
        insert(TileIndex(tidx.begin(), tidx.end()));
    }
}

void Domain<TileIndex>::set_trange(const tiled_range_type& trange) {
    using pimpl_type = detail_::DomainPIMPL<TileIndex>;
    using tpimpl_t   = detail_::TiledDomainPIMPL<pimpl_type>;
    auto ppimpl      = dynamic_cast<tpimpl_t*>(&pimpl_());

    // I'm not sure if it's actually possible to fail this check
    if(ppimpl == nullptr) throw std::runtime_error("PIMPL is incorrect type");

    ppimpl->set_trange(trange);
}

const TA::TiledRange& Domain<TileIndex>::trange() const {
    using pimpl_type = detail_::DomainPIMPL<TileIndex>;
    using tpimpl_t   = detail_::TiledDomainPIMPL<pimpl_type>;
    auto ppimpl      = dynamic_cast<const tpimpl_t*>(&pimpl_());

    // I'm not sure if it's actually possible to fail this check
    if(ppimpl == nullptr) throw std::runtime_error("PIMPL is incorrect type");
    return ppimpl->trange();
}

template class Domain<ElementIndex>;
template class Domain<TileIndex>;

} // namespace chemist::sparse_map
