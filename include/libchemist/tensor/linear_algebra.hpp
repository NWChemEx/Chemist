#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"

namespace libchemist::tensor {

/** @brief Performs a traditional eigen solve on a matrix @p X.
 *
 *  This function will compute the eigen values and eigen vectors of the matrix
 *  @p X.
 *
 *  @param[in] X A symmetric matrix.
 *
 *  @return A pair of tensors, such that first tensor is a vector containing the
 *          eigenvalues and the second tensor is a matrix whose colums are the
 *          eigenvectors.
 */
std::pair<ScalarTensorWrapper, ScalarTensorWrapper> eigen_solve(
  const ScalarTensorWrapper& X);

/** @brief Performs a generalized eigen solve.
 *
 *  @param[in] X The (symmetric) matrix to diagonalize.
 *  @param[in] S The (symmetric) metric matrix.
 *
 *  @return A pair of tensors, such that the first tensor is a vector containing
 *          the eigenvalues, and the second tensor is a matrix whose columsn are
 *          the eigenvectors.
 */
std::pair<ScalarTensorWrapper, ScalarTensorWrapper> eigen_solve(
  const ScalarTensorWrapper& X, const ScalarTensorWrapper& S);

ScalarTensorWrapper cholesky_linv(const ScalarTensorWrapper& M);

/** @brief Raises a Hermitian matrix to a power.
 *
 *  This function returns the matrix which behaves as the matrix @p S raised to
 *  the power @p pow. This is NOT the same as raising each element of @p S to
 *  the power @p pow (unless @p S is diagonal). By "behaves like" we mean that
 *  for integer powers @f$p@f$:
 *
 *  @f[
 *  S^p = \sum_{jk\cdots} S_{ij}S_{jk}\cdots
 *  @f]
 *
 *  where the ellipses contain @f$p-2@f$ more matrix multiplications. Fractional
 *  power @f$p=\frac{1}{q}@f$ is the @f$q@f$-th root of the matrix, and negative
 *  powers are powers/roots of the inverse of @p S.
 *
 *  @param[in] S The matrix we are raising to power @p pow.
 *  @param[in] pow The power we are raising @p S to.
 *
 *  @return A matrix which behaves like @p S raised to the power @p pow.
 */
ScalarTensorWrapper hmatrix_pow(const ScalarTensorWrapper& S, double pow);

} // namespace libchemist::tensor
