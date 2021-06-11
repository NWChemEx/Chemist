#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/tensor/tensor.hpp"

/** @file libchemist/orbital_space/types.hpp
 *
 *  Types used throughout the classes in the orbital space component of
 *  libchemist.
 */
namespace libchemist::orbital_space::type {

/// Type used for indexing and offsets
using size = std::size_t;

/** @brief Type of a wrapped tensor.
 *
 *   When we use tensor wrappers in the orbital space components we use them
 *   because the function/algorithm works with any tensor. If it does not we
 *   explicitly specialize for each tensor type that it does work for.
 */
using tensor_wrapper = tensor::UnieversalTensorWrapper;

/** @brief Type of the sparse affiliated with dependent spaces.
 *
 *  Sparse maps are stored from elements to elements. Since tiling can vary from
 *  tensor to tensor this allows us to compute the tile to tile sparse map per
 *  tensor.
 */
using sparse_map = sparse_map::SparseMapEE;

} // namespace libchemist::orbital_space::type