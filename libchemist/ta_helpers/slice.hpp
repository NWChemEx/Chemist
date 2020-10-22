#pragma once
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include "libchemist/ta_helpers/is_tile_bound.hpp"
#include "libchemist/ta_helpers/retile.hpp"
#include <algorithm> // count
#include <tiledarray.h>
#include <utilities/iter_tools.hpp>

namespace libchemist::ta_helpers {
namespace detail_ {

/** @brief Wraps determining if a slice is empty.
 *
 *  This function's primary goal is to determine if a slice is empty. In doing
 *  so it also ensures that the input indices, `lo` and `hi`, obey:
 *
 *  - the rank of `lo` must be the same as `hi`
 *  - mode `i` of `lo` must satisfy `lo[i] <= hi[i]` for all `i`
 *    - equality is only allowed if `lo[i] == hi[i]` for all `i`
 *
 *  @param[in] lo The first index in the slice.
 *  @param[in] hi The first index not in the slice.
 *
 *  @return True if the slice is empty and false otherwise.
 *
 *  @throws None All checks performed in this function are assertions, which
 *               will terminate execution if they fail. No throw guarantee.
 */
inline bool is_empty_slice_(const sparse_map::ElementIndex& lo,
                            const sparse_map::ElementIndex& hi) {
    const auto rank = lo.size();

    // lo and hi must have same rank
    TA_ASSERT(rank == hi.size());
    // Make sure hi is greater than equal to lo, count modes that are equal
    std::size_t nequal = 0;
    for(const auto& [l,h] : utilities::Zip(lo, hi)){
        TA_ASSERT(h >= l);
        if(h == l) ++nequal;
    }

    // The slice is empty if all modes are equal
    const bool is_empty = nequal == rank;

    // Either all of or none of the modes are allowed to be equal. Anything in
    // between reduces the rank, which we're not supporting at the moment.
    TA_ASSERT(nequal == 0 || is_empty);

    return is_empty;
}

/** @brief Wraps converting the lower and upper bounds of a slice to tile
 *         indices.
 *
 *  This function takes the first and last elements in a slice and converts them
 *  to the indices of the first tile in the slice and the first tile **NOT** in
 *  the slice.
 *
 *  This function is code factorization for the `slice` function.
 *
 * @param[in] tr The TiledRange used to map element indices to tile indices.
 * @param[in] lo The first index in the slice.
 * @param[in] hi_m1 The last index in the slice.
 * @return A pair containing the tile index of the first tile in the slice and
 *         the tile index of the fist tile not in the slice.
 *
 * @throw std::bad_alloc if there is insufficient memory to create the returns.
 *                       Strong throw guarantee.
 */
inline auto get_slice_tile_indices_(const TA::TiledRange& tr,
                                    const sparse_map::ElementIndex& lo,
                                    const sparse_map::ElementIndex& hi_m1) {
    const auto temp    = get_block_idx(tr, lo);
    const auto thi_m1  = get_block_idx(tr, hi_m1);

    // This is the first tile we don't want
    sparse_map::TileIndex tol(temp.begin(), temp.end());
    sparse_map::TileIndex thi(thi_m1.begin(), thi_m1.end());
    for(auto& x : thi.m_index) ++x;
    return std::make_pair(tol, thi);
}

} // namespace detail_

/** @brief Returns a slice of a tensor as specified by two element indices.
 *
 *  @note At present, this function can **NOT** be used to retrieve an empty
 *        slice or a slice which reduces the rank of @p t (sometimes called a
 *        "chip").
 *
 * @tparam TensorType The type of the tensor from which the slice is being
 *                    taken.
 * @param[in] t The tensor from which the slice is being taken.
 * @param lo The first index inside the slice. Modes of @p lo must satisfy
 *           `hi[i] >= lo[i]` for all modes `i` (equality if and only if all
 *           modes are equal).
 * @param hi The first index outside the slice. Modes of @p hi must satisfy
 *           `hi[i] >= lo[i]` for all modes `i` (equality if and only if all
 *           modes are equal).
 * @return The requested slice of the tensor.
 */
template<typename TensorType>
auto slice(const TensorType& t, const sparse_map::ElementIndex& lo,
                                const sparse_map::ElementIndex& hi) {
    const auto& tr        = t.trange();
    const auto rank       = tr.rank();
    const auto annotation = TA::detail::dummy_annotation(rank);

    // We rely on error checking done in is_empty_slice_ for everything except
    // making sure lo and hi are the same rank as the tensor, and that lo and
    // hi are in the tensor
    TA_ASSERT(rank == lo.size());
    const bool is_empty = detail_::is_empty_slice_(lo, hi);
    TA_ASSERT(!is_empty); // TODO: This function should handle empty slices
    TA_ASSERT(tr.elements_range().includes(lo));

    // hi is just past the last element we want, hi "minus 1" is the last
    // element we want.
    sparse_map::ElementIndex hi_m1(hi);
    for(auto& x : hi_m1) x -= 1;
    TA_ASSERT(tr.elements_range().includes(hi_m1));

    // Now we need to determine if lo/hi are the leading/ending elements of
    // their respective tiles
    const bool lo_good  = is_tile_lower_bound(tr, lo);
    const bool hi_good  = is_tile_upper_bound(tr, hi);

    // If we don't have to retile this is easy, just return the slice
    if(lo_good && hi_good){
        auto [tlo, thi] = detail_::get_slice_tile_indices_(tr, lo, hi_m1);
        TensorType rv;
        rv(annotation) = t(annotation).block(tlo, thi);
        return rv;
    }

    // Getting here means we need to retile before grabbing the block
    auto new_tr            = insert_tile_boundaries(tr, lo, hi);
    const TensorType new_t = TA::retile(t, new_tr);
    auto [tlo, thi]        = detail_::get_slice_tile_indices_(new_tr, lo, hi_m1);
    TensorType rv;
    rv(annotation) = new_t(annotation).block(tlo, thi);
    return rv;
}


} // namespace libchemist
