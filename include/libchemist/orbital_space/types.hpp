#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/tensor/tensor.hpp"
#include "libchemist/types.hpp"

/** @file libchemist/orbital_space/types.hpp
 *
 *  Types used throughout the classes in the orbital space component of
 *  libchemist.
 */
namespace libchemist::orbital_space::type {

using namespace libchemist::type;

/// Type of a wrapped, non-hierarchical tensor
using tensor_wrapper = libchemist::tensor::ScalarTensorWrapper;

/// Type of a wrapped, hierarchical tensor
using tot_wrapper = libchemist::tensor::TensorOfTensorsWrapper;

/** @brief Type of the sparse affiliated with dependent spaces.
 *
 *  Sparse maps are stored from elements to elements. Since tiling can vary from
 *  tensor to tensor this allows us to compute the tile to tile sparse map per
 *  tensor.
 */
using sparse_map = sparse_map::SparseMapEE;

} // namespace libchemist::orbital_space::type
