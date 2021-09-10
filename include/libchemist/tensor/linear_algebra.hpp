#pragma once
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

std::pair<type::SparseTensorWrapper, type::SparseTensorWrapper> eigen_solve(
  const type::SparseTensorWrapper& X);

std::pair<type::SparseTensorWrapper, type::SparseTensorWrapper> eigen_solve(
  const type::SparseTensorWrapper& X, const type::SparseTensorWrapper& S);

type::SparseTensorWrapper cholesky_linv(const type::SparseTensorWrapper& M);

type::SparseTensorWrapper hmatrix_pow(const type::SparseTensorWrapper& S,
                                      double pow);

} // namespace libchemist::tensor
