#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/condense_tensor_range.hpp"
#include <tiledarray.h>
#include <btas/tensor.h>

namespace libchemist {
namespace detail_ {

/** @brief Copies tiles from a tensor into a new tile.
 *
 * @tparam NewTileType Type of the tile into which the data is being copied.
 *                     Must satisfy TA's Tile API.
 * @tparam OldTileType Type of the tile from which the data is being copied.
 *                     Must satisfy TA's Tile API.
 * @tparam PolicyType
 * @param new_tile
 * @param r
 * @param t
 * @return
 */
template<typename NewTileType, typename OldTileType, typename PolicyType>
auto add_to_inner_tile_(NewTileType new_tile,
                        const std::map<std::vector<std::size_t>, TA::Range>& r,
                        const TA::DistArray<OldTileType, PolicyType>& t) {
    for(auto [tile_idx, new_tile_range] : r) { // Loop over old tiles
        // Iterators to where in the new tile the old tile's contents should go
        auto nt_begin    = new_tile_range.begin();
        auto nt_end      = new_tile_range.end();

        // TODO: Use futures
        OldTileType subtile = t.find(tile_idx); // The old tile

        // Get iterators to the old tile's indices
        const auto& subtile_range = subtile.range();
        auto st_begin             = subtile_range.begin();
        auto st_end               = subtile_range.end();

        // Copy data from old tile to the new tile
        while(nt_begin != nt_end) {
            new_tile(*nt_begin) = subtile(*st_begin);
            ++nt_begin;
            ++st_begin;
        }
    }
    return new_tile;
}

} // namespace detail_

template<typename TileType, typename SparsityType>
auto condense_tensor(const sparse_map::SparseMap& sm,
                     const TA::DistArray<TileType, SparsityType>& t,
                     const TA::TiledRange& outer_trange,
                     const std::set<std::size_t>& ind_modes = {}) {
    // Type of scalars in the original tensor and the inner tensor of new ToT
    using element_type    = typename TileType::value_type;
    // Type of inner tensor of ToT
    using inner_tile_type = TA::Tensor<element_type>;
    // Type of outer tensor of ToT
    using outer_tile_type = TA::Tensor<inner_tile_type>;
    // Type of the resulting tensor
    using result_tensor = TA::DistArray<outer_tile_type, TA::SparsePolicy>;

    const auto& trange = t.trange();
    auto [subtile_ranges, new_tile_ranges] =
      condense_tensor_range(sm, trange, ind_modes);

    auto l = [=](outer_tile_type& tile, const TA::Range& r) {
        outer_tile_type buffer(r);
        element_type norm = 0.0;
        for(const auto& idx : r) {
            std::vector temp_idx(idx.begin(), idx.end());
            const auto& tile_range = new_tile_ranges.at(temp_idx);
            const auto& st_ranges  = subtile_ranges.at(temp_idx);
            inner_tile_type new_tile(tile_range);

            buffer(idx) = detail_::add_to_inner_tile_(new_tile, st_ranges, t);
            norm += buffer(idx).norm();
        }
        tile = buffer;
        return norm;
    };

    return TA::make_array<result_tensor>(t.world(), outer_trange, l);
}

} // namespace libchemist
