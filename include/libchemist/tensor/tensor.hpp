#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/types.hpp"
#include <tiledarray.h>
#include <variant>
namespace libchemist::tensor {

using UniversalTensorWrapper = TensorWrapper<all_tensor_variant_t>;

using SparseTensorWrapper = TensorWrapper<tensor_variant_t>;

using ToTWrapper = TensorWrapper<tot_variant_t>;

} // namespace libchemist::tensor