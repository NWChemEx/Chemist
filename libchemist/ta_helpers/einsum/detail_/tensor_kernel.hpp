#pragma once
#include "libchemist/ta_helpers/einsum/detail_/detail_.hpp"
#include "libchemist/ta_helpers/einsum/detail_/index_map.hpp"
#include "libchemist/ta_helpers/einsum/types.hpp"
#include <tiledarray.h>

namespace libchemist::ta_helpers::einsum::detail_ {

/** @brief Extracts the lower- and upper-bounds out of a series of TiledRange1s
 *
 *  Given a map from string indices to TiledRange1 instances, this function will
 *  create a map from string indices to pairs, such that the first element of
 *  the pair is the first element of the range and the second element of the
 *  pair is just past the last element of the range. Indices in the input ranges
 *  and the output ranges refer to tiles NOT elements.
 *
 *  This function is mainly pulled out of tensor_kernel for unit-testing
 *  purposes (to make sure I correctly understood TA's API)
 *
 *  @param[in] ranges The map from string indices to TiledRange1 instances.
 *  @return A map from string indices to lower and upperbounds for the range of
 *          tiles.
 */
inline auto get_block_ranges(
  const std::map<types::index, TA::TiledRange1>& ranges) {
    types::assoc_range block_ranges;
    for(auto&& [k, v] : ranges) { block_ranges[k] = v.tiles_range(); }
    return block_ranges;
}

inline auto get_block_range(
  const types::assoc_index& block_idx,
  const std::map<types::index, TA::TiledRange1>& ranges) {
    types::assoc_range block_range;
    for(auto&& [k, v] : ranges) block_range[k] = v.tile(block_idx.at(k));
    return block_range;
}

template<typename T>
auto collapse_range(const types::index_set& result_vars, T&& idx,
                    types::assoc_range block_ranges) {
    using size_type = typename types::index_set::size_type;
    for(size_type i = 0; i < result_vars.size(); ++i) {
        const auto idx_i                = idx[i];
        block_ranges.at(result_vars[i]) = std::make_pair(idx_i, idx_i + 1);
    }
    return block_ranges;
}

template<typename LHSType, typename RHSType, typename BlockKernelType>
auto tensor_kernel(const IndexMap& indices,
                   const std::map<types::index, TA::TiledRange1>& ranges,
                   LHSType&& lhs, RHSType&& rhs,
                   BlockKernelType&& block_kernel) {
    // Make the result
    using tensor_type = std::decay_t<LHSType>;
    auto trange1s     = indices.select_result(ranges);
    TA::TiledRange trange(trange1s.begin(), trange1s.end());
    tensor_type rv(lhs.world(), trange);
    rv.fill(0.0);
    // Get each index's range in terms of blocks
    auto block_ranges = get_block_ranges(ranges);

    // TiledRange of the result tensor
    const auto result_range = trange.tiles_range();
    const auto n_blocks     = result_range.volume();

    for(auto block_i = 0; block_i < n_blocks; ++block_i) {
        if(!rv.is_local(block_i)) continue;

        // Collapse the non-summed range
        auto idx                = result_range.idx(block_i);
        const auto& result_vars = indices.result_vars();
        block_ranges =
          collapse_range(result_vars, idx, std::move(block_ranges));

        // Make associative index for this block
        auto block_idx = initial_index(block_ranges);

        // There's at least one block (or we wouldn't have entered for-loop)
        do {
            // Get this block's element ranges
            auto block_range = get_block_range(block_idx, ranges);

            auto&& [oidx, lidx, ridx] = indices.select(block_idx);
            if(lhs.shape().is_zero(lidx) || rhs.shape().is_zero(ridx)) continue;
            auto ltile = lhs.find(lidx).get();
            auto rtile = rhs.find(ridx).get();
            auto otile = block_kernel(indices, block_range, ltile, rtile);
            rv.find(oidx).get().add_to(otile);

        } while(!detail_::increment_index(block_idx, block_ranges));
    }
    lhs.world().gop.fence();
    return rv;
}
} // namespace libchemist::ta_helpers::einsum::detail_
