#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/types.hpp"
#include <tiledarray.h>
#include <variant>

namespace libchemist::tensor {

extern template class TensorWrapper<type::tensor_variant>;
extern template class TensorWrapper<type::tot_variant>;

} // namespace libchemist::tensor
