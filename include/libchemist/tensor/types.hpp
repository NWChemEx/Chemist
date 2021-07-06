#pragma once
#include "libchemist/types.hpp"
#include <tiledarray.h>
#include <variant>

namespace libchemist::tensor {
template<typename T>
class TensorWrapper;

namespace type {

/// Type of a variant with all possible non-hierarchal tensor types in it
using tensor_variant = std::variant<libchemist::type::tensor<double>>;

/// Type of a variant with all possible hierarchal tensor_types in it
using tot_variant = std::variant<libchemist::type::tensor_of_tensors<double>>;

/// Type of a TensorWrapper suitable for non-hierarchical tensors
using SparseTensorWrapper = TensorWrapper<tensor_variant>;

/// Type of a TensorWrapper suitable for hierarchical tensors
using ToTWrapper = TensorWrapper<tot_variant>;

} // namespace type
} // namespace libchemist::tensor
