#pragma once
#include <tiledarray.h>
#include <variant>

namespace libchemist::tensor {
template<typename T>
class TensorWrapper;

namespace type {

namespace detail_ {
/// Typedef of the tensor of tensors tile
template<typename T>
using tot_tile = TA::Tensor<TA::Tensor<T>>;

/// Typedef of the tensor class
template<typename T>
using tensor = TA::TSpArray<T>;

/// Typedef of the tensor of tensor class
template<typename T>
using tensor_of_tensors = TA::DistArray<detail_::tot_tile<T>, TA::SparsePolicy>;

} // namespace detail_

/// Type of a variant with all possible non-hierarchal tensor types in it
using tensor_variant = std::variant<detail_::tensor<double>>;

/// Type of a variant with all possible hierarchal tensor_types in it
using tot_variant = std::variant<detail_::tensor_of_tensors<double>>;

/// Type of a TensorWrapper suitable for non-hierarchical tensors
using SparseTensorWrapper = TensorWrapper<tensor_variant>;

/// Type of a TensorWrapper suitable for hierarchical tensors
using ToTWrapper = TensorWrapper<tot_variant>;

} // namespace type
} // namespace libchemist::tensor
