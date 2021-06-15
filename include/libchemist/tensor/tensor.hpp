#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/types.hpp"
#include <tiledarray.h>
#include <variant>
namespace libchemist::tensor {

using UniversalTensorWrapper = TensorWrapper<type::all_tensor_variant>;

using SparseTensorWrapper = TensorWrapper<type::tensor_variant>;

using ToTWrapper = TensorWrapper<type::tot_variant>;

} // namespace libchemist::tensor