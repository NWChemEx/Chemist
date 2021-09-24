#pragma once

#include "libchemist/sparse_map/domain/domain.hpp"
#include "libchemist/sparse_map/from_sparse_map.hpp"
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
#include "libchemist/sparse_map/sparsify.hpp"

/** @brief Contains classes and functions related to SparseMap objects.
 *
 */
namespace libchemist::sparse_map {

using SparseMapEE = SparseMap<ElementIndex, ElementIndex>;
using SparseMapET = SparseMap<ElementIndex, TileIndex>;
using SparseMapTE = SparseMap<TileIndex, ElementIndex>;
using SparseMapTT = SparseMap<TileIndex, TileIndex>;

} // namespace libchemist::sparse_map
