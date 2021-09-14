#pragma once
#include "libchemist/tensor/types.hpp"
#include <functional>

namespace libchemist::tensor {

/** @brief Creates a new tensor by applying a function elementwise to an
 *         existing tensor.
 *
 *  This function is a convenience function for creating a new tensor whose
 *  initial elements can be determined solely based on the element values of
 *  another tensor.
 *
 * @param[in] input The tensor supplying the input values @p op will be called
 *                  with.
 * @param[in] op The function to apply to each element of @p input to generate
 *               the resulting tensor.
 * @return A new tensor having the same shape and distribution as @p input. The
 *         @f$i@f$-th element of the new tensor is the result of `op(input[i])`,
 *         where `input[i]` is the @f$i@f$-th element of @p input.
 */
type::SparseTensorWrapper apply_elementwise(
  const type::SparseTensorWrapper& input,
  const std::function<double(double)>& fxn);

/** @brief Modifies an existing tensor by applying a function elementwise to its
 *         values.
 *
 *  This function is a convenience function for modifying an existing tensor
 *  by an elementwise in-place operation.
 *
 * @param[in] input The tensor supplying the input values to be changed in-place
 *                  by @p op.
 * @param[in] op The function to apply to each element of @p input to modify
 *               the values in-place.
 */
void apply_elementwise_inplace(type::SparseTensorWrapper& input,
                               const std::function<void(double&)>& fxn);

} // namespace libchemist::tensor
