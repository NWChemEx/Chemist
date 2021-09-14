#pragma once
#include "libchemist/tensor/allclose.hpp"
#include "libchemist/tensor/apply_elementwise.hpp"
#include "libchemist/tensor/creation.hpp"
#include "libchemist/tensor/linear_algebra.hpp"
#include "libchemist/tensor/remove_redundancy.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/types.hpp"
#include <tiledarray.h>
#include <variant>

namespace libchemist::tensor {

extern template class TensorWrapper<type::tensor_variant>;
extern template class TensorWrapper<type::tot_variant>;

} // namespace libchemist::tensor
