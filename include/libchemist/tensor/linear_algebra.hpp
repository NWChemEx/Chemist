#pragma once
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

std::pair<type::SparseTensorWrapper, type::SparseTensorWrapper> eigen_solve(
  const type::SparseTensorWrapper& X);

std::pair<type::SparseTensorWrapper, type::SparseTensorWrapper> eigen_solve(
  const type::SparseTensorWrapper& X, const type::SparseTensorWrapper& S);

} // namespace libchemist::tensor
