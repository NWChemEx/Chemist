#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"

namespace libchemist::tensor {

std::pair<ScalarTensorWrapper, ScalarTensorWrapper> eigen_solve(
  const ScalarTensorWrapper& X);

std::pair<ScalarTensorWrapper, ScalarTensorWrapper> eigen_solve(
  const ScalarTensorWrapper& X, const ScalarTensorWrapper& S);

ScalarTensorWrapper cholesky_linv(const ScalarTensorWrapper& M);

ScalarTensorWrapper hmatrix_pow(const ScalarTensorWrapper& S, double pow);

} // namespace libchemist::tensor
