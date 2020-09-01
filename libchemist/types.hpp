#pragma once
#include <tiledarray.h>

/** @file types.hpp
 *
 * Typedefs of fundamental types used in libchemist. Non-standard types
 * appearing in interfaces of multiple classes/functions should be defined here
 * and those classes/functions written in terms of these typedefs. This will
 * avoid large refactoring efforts if/when these types need to change.
 */
namespace libchemist::type {

/// Typedef of the tensor class
template<typename T>
using tensor = TA::TSpArray<T>;

/// Typedef of a tensor of tensors
template<typename T>
using tensor_of_tensors =
  TA::DistArray<TA::Tensor<TA::Tensor<T>>, TA::SparsePolicy>;

} // namespace libchemist
