#include "chemist/ta_helpers/get_block_idx.hpp"
#include "chemist/ta_helpers/is_tile_bound.hpp"
#include <algorithm> // equal

namespace chemist::ta_helpers {

using sparse_map::ElementIndex;

bool is_tile_lower_bound(const TA::TiledRange& tr,
                         const ElementIndex& elem) noexcept {
    TA_ASSERT(elem.size() == tr.rank()); // Doesn't throw, just crashes

    // Make sure the element is actually in the TiledRange
    if(!tr.elements_range().includes(elem)) return false;

    // It is in the range so get the tile it belongs to
    const auto tidx  = get_block_idx(tr, elem);
    const auto& tile = tr.tile(tidx);

    // Now compare to the lower bound of the tile
    const auto lo = tile.lobound();
    return std::equal(lo.begin(), lo.end(), elem.begin());
}

bool is_tile_upper_bound(const TA::TiledRange& tr, const ElementIndex& elem) {
    using size_type = typename ElementIndex::value_type;

    // TODO: Make no throw guarantee by taking a copy of elem and manipulating
    //       the copy.

    TA_ASSERT(elem.size() == tr.rank());

    // If this index is an upper bound, the last element in the tile it would be
    // an upper bound of would have offsets along mode i equal to elem[i] - 1
    std::vector<size_type> elem_m1(elem.begin(), elem.end());
    for(auto& x : elem_m1) {
        if(x == 0) return false; // Tiles have to have elements
        --x;
    }

    // Make sure that elem - 1 is in the tiled range
    if(!tr.elements_range().includes(elem_m1)) return false;

    const auto tidx  = get_block_idx(tr, ElementIndex(elem_m1));
    const auto& tile = tr.tile(tidx);
    const auto& hi   = tile.upbound();
    return std::equal(hi.begin(), hi.end(), elem.begin());
}

} // namespace chemist::ta_helpers
