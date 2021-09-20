#pragma once
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

/** @brief Elminates linear dependencies in the provided transformation.
 *
 *  Given a matrix of column vectors, @p C, and their inner products, @p S, such
 *  that the inner product of vector @f$i@f$ with vector @f$j@f$ is given by
 *  @f$S_{ij}@f$. This function will return a new set of linearly-independent
 *  vectors by diagonalizing @p S and dropping vectors associated with eigen
 *  values below @p thresh.
 *
 *  @param[in] C A set of column vectors which may have some linear
 *               dependencies.
 *  @param[in] S The overlap of the column vectors.
 *  @param[in] thresh Vectors associated with eigen values below this value will
 *                    be dropped.
 *
 *  @return The new, linearly-independent, vectors.
 */
type::SparseTensorWrapper remove_redundancy(const type::SparseTensorWrapper& C,
                                            const type::SparseTensorWrapper& S,
                                            double thresh = 1.0E-8);

} // namespace libchemist::tensor