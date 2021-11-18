#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"

/** @file creation.hpp
 *
 *  A collection of routines for quickly creating tensors from other tensors.
 */

namespace libchemist::tensor {

/** @brief Concatentaes two tensors together along the specified dimension.
 *
 *  This function joins two tensors together by making concatenating them
 *  together along the specified mode. For a rank @f$N@f$ tensor, this means
 *  that the shape of the remaining N-1 modes must be identical.
 *
 *
 *  @note Concatenation is in general non-commutative, i.e.,
 *        `concatenate(A, B, dim)` will not necessarilly be the same as
 *        `concatenate(B, A, dim)`.
 *
 *  @param[in] lhs The tensor which will serve as the base tensor.
 *  @param[in] rhs The tensor which will be added on to @p lhs. Must be the same
 *                 rank as @p lhs and all modes aside from mode @p dim must have
 *                 the same shape as the corresponding mode in @p lhs.
 *  @param[in] dim The 0-based offset indicating which mode of @p lhs and
 *                 @p rhs will be concatenated. Must be in the range
 *                 [0, rank(lhs)).
 *
 *  @throw std::runtime_error if the shapes of the tensors are not compatible
 *                            for the concatenation or if @p dim is not a valid
 *                            mode offset. Strong throw guarantee.
 */
ScalarTensorWrapper concatenate(const ScalarTensorWrapper& lhs,
                                const ScalarTensorWrapper& rhs,
                                std::size_t dim);

/** @brief Concatenates two ToTs along the specified dimension.
 *
 *  This function is not yet implemented.
 *
 */
TensorOfTensorsWrapper concatenate(const TensorOfTensorsWrapper& lhs,
                                   const TensorOfTensorsWrapper& rhs,
                                   std::size_t dim);

/** @brief Grabs the diagonal of a matrix and returns it as a vector.
 *
 *  It sometimes happens that we have a matrix, but we only need the diagonal of
 *  that matrix. This function wraps the process of copying the diagonal of that
 *  matrix into a vector.
 *
 *  @return A TensorWrapper containing just the diagonal elements of the tensor.
 */
ScalarTensorWrapper grab_diagonal(const ScalarTensorWrapper& t);

} // namespace libchemist::tensor
