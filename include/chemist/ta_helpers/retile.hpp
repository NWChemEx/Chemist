#pragma once
#include "chemist/sparse_map/index.hpp"
#include "chemist/ta_helpers/ta_headers.hpp"

namespace chemist::ta_helpers {

/** @brief This function retiles a TiledRange so that the provided elements lie
 *         on tile boundaries (if they do not already).
 *
 *  Given a list of element indices and an input TiledRange, this function will
 *  create a new TiledRange object which, in addition to the input tilings, also
 *  has tile boundaries at the specified elements.
 *
 *  @param[in] tr The TiledRange we are adding tile boundaries to.
 *  @param[in] boundaries The elements which will fall on tile boundaries in the
 *                        new TiledRange.
 *
 *  @return A TiledRange whose boundary elements are the union of the boundaries
 *          in @p tr and the elements in @p boundaries.
 */
TA::TiledRange insert_tile_boundaries(
  const TA::TiledRange& tr,
  const std::vector<sparse_map::ElementIndex>& boundaries);

/** @brief Convenience function for calling insert_tile_boundaries when the
 *         desired boundaries are not already in an std::vector.
 *
 *   This function is a thin wrapper around the
 *   `insert_tile_boundaries(TiledRange, std::vector)` overload which, instead
 *   of taking an `std::vector` of `ElementIndex` instances, takes an arbitrary
 *   number of `ElementIndex` instances.
 *
 * @tparam Args Template parameter pack. All types in the parameter pack need to
 *              be implicitly convertible to ElementIndex.
 *
 * @param[in] tr The TiledRange we are adding the tile boundaries to.
 * @param[in] e0 The first new tile boundary to add.
 * @param[in] args The optional second, third, etc. tile boundaries to add/
 *
 * @return The TiledRange instance containing the original boundaries as well as
 *         the boundaries the user specified.
 */
template<typename... Args>
TA::TiledRange insert_tile_boundaries(const TA::TiledRange& tr,
                                      const sparse_map::ElementIndex& e0,
                                      Args&&... args) {
    using vector_type = std::vector<sparse_map::ElementIndex>;
    const vector_type boundaries{e0, std::forward<Args>(args)...};
    return insert_tile_boundaries(tr, boundaries);
}

} // namespace chemist::ta_helpers
