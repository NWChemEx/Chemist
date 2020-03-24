#include "libchemist/ta_helpers/condense_tensor_range.hpp>

namespace libchemist::detail_{

using size_type = typename CondenseTensorRangeTypes<>::size_type;
using index_set = typename CondenseTensorRangeTypes<>::index_set;
using range_type = typename CondenseTensorRangeTypes<>::range_type;
using range_map  = typename CondenseTensorRangeTypes<>::range_map;
using return1 = typename CondenseTensorRangeTypes<>::return1;

return1 condense_tensor_range_(const index_set& indices,
                               const TA::TiledRange1& trange) {

    range_map rv;
    size_type offset = 0; // The current upperbound of the condensed range

    for(auto idx : indices){ // Loop over tile indices

        // Get the old low and hi bounds for this tile
        const auto& tile = tr1.tile(idx);
        const auto lo = tile.lobound(0);
        const auto hi = tile.upbound(0);

        // The shift to make lo equal to offset
        const auto shift = lo - offset;

        // Record the tile's new range
        rv.emplace(idx, std::make_pair(lo - shift, hi - shift));

        // Increment offset by the number of elements in the tile
        offset += hi - lo;
    }

    return std::make_pair(rv, range_type{0, offset});
}

auto condense_tensor_range_(const std::vector<index_set>& indices,
                            const TA::TiledRange& trange,
                            const index_set& ind_modes) {

    const auto rank = trange.rank();
    TA_ASSERT(rank ==indices.size() + ind_modes.size());

    std::vector<range_type> bounds;
    std::vector<range_map> offsets;
    bounds.reserve(rank);
    offsets.reserve(rank);

    for(size_type mode = 0, counter = 0; mode < rank; ++mode) {

        const auto& dim = trange.dim(mode);

        if(!ind_modes.count(mode)) {
            auto [o, b] = condense_tensor_range_(indices[counter], dim);
            bounds.emplace_back(std::move(b));
            offsets.emplace_back(std::move(o));
            ++counter;
        }
        else{
            range_type r{0, dim.volume()};
            bounds.push_back(r);
        }
    }
    return std::make_pair(offsets, bounds);
}

} // namespace libchemist::detail_
