#pragma once
#include <tiledarray.h>
#include <map>
#include <set>
#include <utility>
#include <vector>

/** @file detail_/condense_tensor_range.hpp
 *
 *  This header exposes routines used within condense_tensor_range, but which
 *  are not considered part of condense_tensor_range's public API. They are
 *  exposed via this header primarily for testing purposes.
 */

namespace libchemist::detail_{

/** @brief Wraps the types needed for the implementations of
 *         `condense_tensor_range`
 *
 *  The intermediates needed by `condense_tensor_range` are some pretty nested
 *  STL containers. We typedef these types within the CondenseTensorRangeTypes
 *  to make reading (and typing) the following definitions easier.
 *
 *  @tparam SizeType Should be an unsigned integral type. Defaults to
 *                   `std::size_t`
 */
template<typename SizeType = std::size_t>
struct CondenseTensorRangeTypes {
    /// Helper typedef of the template type parameter
    using size_type = SizeType;
    /// Type of a set of one-dimensional indices
    using index_set  = std::set<SizeType>;
    /// Type for the one-dimensional starting and stopping indices of a range
    using range_type = std::pair<SizeType, SizeType>;
    /// Map type from a one-dimensional index to a range
    using range_map  = std::map<SizeType, range_type>;
    /// Return type of condense_tensor_range_(index_set, TiledRange1)
    using return1    = std::pair<range_map, range_type>;
    /// Return type of condense_tensor_range_(index_array, TiledRange, index_set)
    using return2 = std::pair<std::vector<range_map>, std::vector<range_type>>;
};

/** @brief Determines the element ranges for the selected tiles in a condensed
 *         tensor.
 *
 * Given a tiled range and a list of tiles to condense into a new range, this
 * function will create a map from the original tile index to the element range
 * in the new condensed tile. The ranges are of the form [lo, high) where lo and
 * high respectively are the new beginning and the new just-past-the-ending
 * element indices. For example if our full tiled range is
 * `{{0}, {1, 2}, {3, 4, 5}}` (sub groupings denote tilings) and we want to
 * condense tiles 0 and 2, this function returns a map (shown in JSON):
 *
 * @code
 * {0 : (0, 1)}, {2 : (1, 4)}
 * @endcode
 *
 * For convenience we also return the limits of the new range, which would be
 * [0, 4) for the above example.
 *
 * This function only considers a single mode of the tensor's range. See the
 * `condense_tensor_range_(, TA::TiledRange)` overload for considering a range
 * of modes.
 *
 * @param[in] indices The set of block indices in the new range.
 * @param[in] trange The range the block indices will be taken from.
 *
 * @return The map from block index to elements in the new block of the
 *         condensed tensor as well as the range of the new block.
 */
typename CondenseTensorRangeTypes<>::return1 condense_tensor_range_(
  const typename CondenseTensorRangeTypes<>::index_set& indices,
  const TA::TiledRange1& trange);

/** @brief Determines new element ranges resulting from condensing the specified
 *         tiles.
 *
 * @param indices
 * @param trange
 * @return
 */
typename CondenseTensorRangeTypes<>::return2
condense_tensor_range_(const std::vector<std::set<std::size_t>>& indices,
                       const TA::TiledRange& trange,
                       const std::set<std::size_t>& ind_modes = {});

} // namespace libchemist::detail_
