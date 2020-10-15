#pragma once
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include "libchemist/ta_helpers/is_tile_bound.hpp"
#include "libchemist/ta_helpers/retile.hpp"
#include <algorithm> // count
#include <tiledarray.h>
#include <utilities/iter_tools.hpp>

namespace libchemist {
namespace detail_ {

/** @brief Wraps comparing the input indices to `slice`
 *
 *  The input indices to `slice`, `lo` and `hi`, must obey:
 *
 *  - mode `i` of `lo` must satisfy `lo[i] <= hi[i]` for all `i`
 *    - equality is only allowed if `lo[i] == hi[i]` for all `i`
 *
 * @param lo
 * @param hi
 * @return
 */
inline bool compare_lo_hi_(const sparse_map::ElementIndex& lo,
                          const sparse_map::ElementIndex& hi) {
    const auto rank = lo.size();

    // Make sure hi is greater than equal to lo, count modes that are equal
    std::size_t nequal = 0;
    for(const auto& [l,h] : utilities::Zip(lo, hi)){
        TA_ASSERT(h >= l);
        if(h == l) ++nequal;
    }

    // The slice is empty i
    const bool is_empty = nequal == rank;

    // Either all of or none of the modes are allowed to be equal. Anything in
    // between reduces the rank, which we're not supporting at the moment.
    TA_ASSERT(nequal == 0 || is_empty);

    return is_empty;
}

} // namespace


/**
 *
 * @tparam TensorType
 * @param t
 * @param lo
 * @param hi
 * @return
 *
 */
template<typename TensorType>
auto slice(const TensorType& t, const sparse_map::ElementIndex& lo,
                                const sparse_map::ElementIndex& hi) {
    const auto& tr        = t.trange();
    const auto rank       = tr.rank();
    const auto annotation = TA::detail::dummy_annotation(rank);

    // Make sure ranks are all the same
    TA_ASSERT(lo.size() == hi.size());
    TA_ASSERT(rank == lo.size());

    // Compare indices and determine if slice is empty
    const bool is_empty = detail_::compare_lo_hi_(lo, hi);
    // TODO: This function really should handle empty slices
    TA_ASSERT(!is_empty);

    // hi is just past the last element we want, hi "minus 1" is the last
    // element we want. This won't work if we're reducing the rank with the
    // slice.
    sparse_map::ElementIndex hi_m1(hi);
    for(auto& x : hi_m1) x -= 1;

    // These are the tiles holding the first and last elements we want
    const auto tlo = get_block_idx(tr, lo);
    const auto thi = get_block_idx(tr, hi_m1);

    // Now we need to determine if lo/hi are the leading/ending elements of
    // their respective tiles
    const bool retile_lo  = !is_tile_lower_bound(tr, lo);
    const bool retile_hi  = !is_tile_upper_bound(tr, hi);

    // If we don't have to retile this is easy, just return the slice
    if(!(retile_hi || retile_lo)) return t(annotation).block(tlo, thi);

    // Getting here means we need to retile before grabbing the block
    auto new_tr = insert_tile_boundaries(tr, lo, hi);
    const auto new_tlo = get_block_idx(new_tr, lo);
    const auto new_thi = get_block_idx(new_tr, hi);
    const TensorType new_t = TA::retile(t, new_tr);
    return new_t(annotation).block(new_tlo, new_thi);
}


} // namespace libchemist
