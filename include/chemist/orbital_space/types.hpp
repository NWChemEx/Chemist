#pragma once
#include "chemist/sparse_map/sparse_map.hpp"
#include "chemist/tensor/tensor.hpp"
#include "chemist/types.hpp"

/** @file chemist/orbital_space/types.hpp
 *
 *  Types used throughout the classes in the orbital space component of
 *  chemist.
 */
namespace chemist::orbital_space::type {

using namespace chemist::type;

/// Type of a wrapped, non-hierarchical tensor
using tensor_wrapper = chemist::tensor::ScalarTensorWrapper;

/// Type of a wrapped, hierarchical tensor
using tot_wrapper = chemist::tensor::TensorOfTensorsWrapper;

/** @brief Type of the sparse affiliated with dependent spaces.
 *
 *  Sparse maps are stored from elements to elements. Since tiling can vary from
 *  tensor to tensor this allows us to compute the tile to tile sparse map per
 *  tensor.
 */
using sparse_map = sparse_map::SparseMapEE;

} // namespace chemist::orbital_space::type
