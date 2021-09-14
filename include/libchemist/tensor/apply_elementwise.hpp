#pragma once
#include "libchemist/tensor/types.hpp"
#include <functional>

namespace libchemist::tensor {

type::SparseTensorWrapper apply_elementwise(
  const type::SparseTensorWrapper& input,
  const std::function<double(double)>& fxn);

}
