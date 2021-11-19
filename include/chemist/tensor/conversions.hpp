#pragma once
#include "chemist/tensor/tensor_wrapper.hpp"

namespace chemist::tensor {

/** @brief Flattens a tensor out into an std::vector.
 *
 *  This function will replicate the provided tensor (assuming it is distributed
 *  to begin with) and then flatten it into a vector. The flattening will
 *  occur in row-major format.
 *
 *  @note This function is primarily intended for use with low rank @p t, e.g.
 *        in taking actions based on the value of an eigenvalue. Direct access
 *        of high-rank tensors should be avoided.
 *
 *  @param[in] t The tensor to vectorize.
 *
 *  @return The tensor vectorized into an std::vector.
 */
std::vector<double> to_vector(const ScalarTensorWrapper& t);

} // namespace chemist::tensor
