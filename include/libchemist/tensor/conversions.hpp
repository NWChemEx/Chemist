#pragma once
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

/** @brief Flattens a tensor out into an std::vector.
 *
 *  This function will replicate the provided tensor and return a local copy of
 *  the data, flattened into a vector.
 *
 *  @return The tensor vectorized into an std::vector.
 */
std::vector<double> to_vector(const type::SparseTensorWrapper& t);

} // namespace libchemist::tensor
