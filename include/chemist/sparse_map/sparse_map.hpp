#pragma once

#include "chemist/sparse_map/domain/domain.hpp"
#include "chemist/sparse_map/from_sparse_map.hpp"
#include "chemist/sparse_map/index.hpp"
#include "chemist/sparse_map/make_pair_map.hpp"
#include "chemist/sparse_map/sparse_map/sparse_map.hpp"

/** @brief Contains classes and functions related to SparseMap objects.
 *
 */
namespace chemist::sparse_map {

using SparseMapEE = SparseMap<ElementIndex, ElementIndex>;
using SparseMapET = SparseMap<ElementIndex, TileIndex>;
using SparseMapTE = SparseMap<TileIndex, ElementIndex>;
using SparseMapTT = SparseMap<TileIndex, TileIndex>;

} // namespace chemist::sparse_map
