#include "libchemist/ta_helpers/condense_tensor_range.hpp"
#include "libchemist/ta_helpers/detail_/condense_tensor_range.hpp"

namespace libchemist {
using sparse_map::SparseMap;
using size_type    = typename SparseMap::size_type;
using index_type   = typename SparseMap::key_type;
using index_set    = std::set<size_type>;
using range_type   = std::pair<size_type, size_type>;
using range_vector = std::vector<range_type>;
using range_map    = std::map<index_type, TA::Range>;

namespace {
std::vector<index_type> make_indices(const sparse_map::Domain& domain,
                                     const TA::TiledRange& trange,
                                     const index_set& ind_modes) {
    std::vector<index_type> rv;
    index_type buffer(domain.rank());
    const auto rank = trange.rank();
    for(auto idx : trange.tiles_range()) {
        // Fill buffer with the dependent part of this index
        for(size_type mode = 0, counter = 0; mode < rank; ++mode) {
            if(ind_modes.count(mode)) continue;
            buffer[counter] = idx[mode];
            ++counter;
        }
        // Make sure the dependent part of this index is in the domain
        if(buffer.empty() || domain.count(buffer)) rv.push_back(buffer);
    }
    return rv;
}
}

std::pair<std::map<index_type, range_map>, range_map>
condense_tensor_range(const SparseMap& sm,
                      const TA::TiledRange& trange,
                      const index_set& ind_modes){

    const auto rank = trange.rank();

    TA_ASSERT(rank == ind_modes.size() + sm.dep_rank());

    std::map<index_type, range_map> new_tile_ranges;
    range_map new_ranges;

    for(auto [ind, indices] : sm.indices()) {
        // offsets is a map from a dep mode, tile index pair to the element
        //           range it corresponds to in the new tile
        // bound is a map from a mode in trange to its new bounds in the new
        //       tensor, this will give the total range for the new tile
        auto [offsets, bound] =
            detail_::condense_tensor_range_(indices, trange, ind_modes);
        new_ranges[ind] = TA::Range(bound);

        // For each tile in the domain we need to make a range indicating what
        // elements it has in the new tile, note that in general multiple tiles
        // are being combined to form the new tile so these ranges will be
        // subranges of bound

        // Start by making all the indices in the domain
        auto idx_domain = make_indices(sm.at(ind), trange, ind_modes);

        for(auto idx : idx_domain){
            std::vector<range_type> new_range(rank);
            auto old_range = trange.make_tile_range(idx);
            for(size_type mode = 0, counter = 0; mode < rank; ++mode){
                if(ind_modes.count(mode))
                    new_range[mode] = old_range.dim(mode);
                else{
                    new_range[mode] = offsets[counter][idx[mode]];
                    ++counter;
                }
            }
            new_tile_ranges[ind][idx] = TA::Range(new_range);
        }
    }
    return std::make_pair(new_tile_ranges, new_ranges);
}


}
