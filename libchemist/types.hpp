#pragma once
#include <tiledarray.h>

/** @file types.hpp
 *
 * Typedefs of fundamental types used in libchemist, just the tensor
 * class currently.
 */
namespace libchemist {
namespace type {

/// Typedef of the tensor class
template<typename T>
using tensor = TA::TSpArray<T>;

/// Typedef of the tensor of tensors tile
template<typename T>
using tot_tile = TA::Tensor<TA::Tensor<T>>;

/// Typedef of the tensor of tensor class
template<typename T>
using tensor_of_tensors = TA::DistArray<tot_tile<T>, TA::SparsePolicy>;


} // namespace type
} // namespace libchemist
