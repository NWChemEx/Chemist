#include "libchemist/ta_helpers/retile.hpp"

namespace libchemist {

using sparse_map::ElementIndex;

TA::TiledRange insert_tile_boundaries(
  const TA::TiledRange& tr, const std::vector<ElementIndex>& boundaries) {
    const auto rank = tr.rank();

    // Make the sorted union of the desired boundaries and the current
    // boundaries, asserting all input indices are of the correct rank
    std::set<ElementIndex> bounds2;
    for(const auto& x : boundaries){
        TA_ASSERT(x.size() == rank);
        bounds2.insert(x);
    }

    for(const auto& tile : tr.tiles_range()) {
        const auto trange  = tr.tile(tile);
        const auto lobound = trange.lobound();
        const auto upbound = trange.upbound();
        bounds2.insert(ElementIndex(lobound.begin(), lobound.end()));
        bounds2.insert(ElementIndex(upbound.begin(), upbound.end()));
    }

    // If you think of bounds 2 as a matrix (rows are index number, columns are
    // modes of the element indices) this is the opposite of what we need for
    // making a TiledRange. Here we "transpose" bounds2
    std::vector<std::set<std::size_t>> new_tiling(rank);
    for(const auto& x : bounds2){
        for(std::size_t i = 0; i < rank; ++i) new_tiling[i].insert(x[i]);
    }

    // Now we turn that into a vector of TiledRange1 instances
    std::vector<TA::TiledRange1> new_tr1s;
    for(const auto& x : new_tiling) {
        // TiledRange1 requires the iterator to be random access
        std::vector<std::size_t> copy_x(x.begin(), x.end());
        new_tr1s.emplace_back(TA::TiledRange1(copy_x.begin(), copy_x.end()));
    }
    return TA::TiledRange(new_tr1s.begin(), new_tr1s.end());
}

} // namespace libchemist
