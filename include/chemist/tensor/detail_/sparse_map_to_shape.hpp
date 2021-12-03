#pragma once
#include "chemist/sparse_map/sparse_map.hpp"
#include <tiledarray.h>

namespace chemist::tensor::detail_ {

using idx2mode_type = std::vector<std::size_t>;

/** @brief Determines the non-zero tiles in a TiledArray array given a SparseMap
 *
 *  This function creates a shape object based off of the provided sparse map
 *  and tilings. The shape object is used to inform TiledArray which tiles are
 *  non-zero (and by how much);
 *
 *  @param[in] sm The SparseMap containing a map from independent indices to
 *                non-zero dependent indices.
 *
 *  @param[in] idx2mode Assuming @p sm contains independent indices of rank
 *                      @f$i@f$ and dependent indices of rank @f$d@f$,
 *                      @p idx2mode is an @f$i + d@f$ element vector where the
 *                      @f$j@f$-th element is the mode of @p tr corresponding to
 *                      the @f$j@f$-th mode of @p sm (counting assumes
 *                      concatenation of the independent modes followed by the
 *                      dependent modes).
 *
 *  @param[in] tr The tiling we are creating the shape for.
 *
 *  @return A shape object. Tiles which have been screened out are associated
 *          with hard 0 in the resulting object, whereas retained tiles are
 *          associated with `std::numeric_limist<double>::max()`.
 *
 *  @throw std::runtime_error If the overall rank of @p sm is inconsistent with
 *                            the length of @p idx2mode or the rank of @p tr.
 *                            Strong throw guarantee.
 *  @throw std::runtime_error If any element of @p idx2mode is larger than the
 *                            rank of @p tr. Strong throw guarantee.
 */
TA::SparseShape<float> sparse_map_to_shape(const sparse_map::SparseMapEE& sm,
                                           const idx2mode_type& idx2mode,
                                           const TA::TiledRange& tr);

} // namespace chemist::tensor::detail_
