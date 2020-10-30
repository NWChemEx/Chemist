#pragma once
#include "libchemist/sparse_map/index.hpp"
#include <tiledarray.h>

namespace libchemist::ta_helpers {

/** @brief Determines if a particular element index is the first element of a
 *         tile.
 *
 *  This function will consider the tiles in the provided TiledRange and will
 *  return true if one of the tiles has elements in the range `[elem, x)`, where
 *  `elem` is the provided element and `x` is arbitrary other than it is
 *  lexicographically greater than `elem`.
 *
 * @param[in] tr The TiledRange we are searching for @p elem.
 * @param[in] elem The element whose lower-bound-ness is in question.
 *
 * @return True if @p elem is the lower bound of a tile and false otherwise.
 *
 * @throw None No throw guarantee.
 */
bool is_tile_lower_bound(const TA::TiledRange& tr,
                         const sparse_map::ElementIndex& elem) noexcept;

/** @brief Determines if the provided element is an upper bound of any tile in
 *         the range.
 *
 *  This function will consider the tiles in the provided TiledRange and will
 *  return true if one of the tiles has elements in the range `[x, elem)` where
 *  `elem` is the provided element and `x` is arbitrary other than it is
 *  lexicographically less than `elem`.
 *
 *  @note Following usual C++ convention we define the upper bound as "just past
 *        the last element".
 *
 * @param[in] tr The tiled range we are searching for @p elem.
 * @param[in] elem The element whose upper-bound-ness is in question.
 *
 * @return True if @p elem is the upper bound of a tile in @p tr and false
 *         otherwise
 *
 * @throw std::bad_alloc if there insufficient memory to copy @p elem. Strong
 *                       throw guarantee.
 */
bool is_tile_upper_bound(const TA::TiledRange& tr,
                         const sparse_map::ElementIndex& elem);

} // namespace libchemist
