#pragma once
#include <tiledarray.h>

/** @file types.hpp
 *
 * @brief Typedefs of the fundamental types used in libchemist.
 *
 * The typedefs in this file are the default types used throughout LibChemist.
 * For example, `libchemist::type::tensor<double>` is the default type of a
 * tenosr whose elements are of type `double` and `libchemist::type::size` is
 * the default type used for indexing and offsets. Components of LibChemist can
 * override the defaults if they need to, but should otherwise just use these
 * typedefs. Types that are primarily used within a component of LibChemist
 * should be defined in that component's `types.hpp` file.
 */
namespace libchemist::type {
namespace detail_ {

/// Typedef of the tensor of tensors tile
template<typename T>
using tot_tile = TA::Tensor<TA::Tensor<T>>;

} // namespace detail_

/// Type used for offsets and indexing
using size = std::size_t;

/// Typedef of the tensor class
template<typename T>
using tensor = TA::TSpArray<T>;

/// Typedef of the tensor of tensor class
template<typename T>
using tensor_of_tensors = TA::DistArray<detail_::tot_tile<T>, TA::SparsePolicy>;

} // namespace libchemist::type
