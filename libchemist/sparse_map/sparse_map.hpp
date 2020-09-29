#pragma once

#include "libchemist/sparse_map/domain.hpp"
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map_class.hpp"

namespace libchemist::sparse_map {

using SparseMapEE = SparseMap<ElementIndex, ElementIndex>;
using SparseMapET = SparseMap<ElementIndex, TileIndex>;
using SparseMapTE = SparseMap<TileIndex, ElementIndex>;
using SparseMapTT = SparseMap<TileIndex, TileIndex>;

}
