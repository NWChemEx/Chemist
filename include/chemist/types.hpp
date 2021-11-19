#pragma once
#include "chemist/tensor/tensor.hpp"

/** @file types.hpp
 *
 * @brief Typedefs of the fundamental types used in chemist.
 *
 * The typedefs in this file are the default types used throughout LibChemist.
 * For example, `chemist::type::tensor<double>` is the default type of a
 * tenosr whose elements are of type `double` and `chemist::type::size` is
 * the default type used for indexing and offsets. Components of LibChemist can
 * override the defaults if they need to, but should otherwise just use these
 * typedefs. Types that are primarily used within a component of LibChemist
 * should be defined in that component's `types.hpp` file.
 */
namespace chemist::type {

/// Type used for offsets and indexing
using size = std::size_t;

/// Type used for non-hierarchical tensors
using tensor = chemist::tensor::ScalarTensorWrapper;

/// Type used for hierarchical (i.e. ToTs) tensors
using tensor_of_tensors = chemist::tensor::TensorOfTensorsWrapper;

} // namespace chemist::type
