#pragma once
#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
#include "libchemist/types.hpp"

namespace libchemist::sparse_map {

/** @brief Zeros out tiles of a transformation based on the provided sparse map.
 *
 * Given a transformation @f$C_{\mu i}@f$ and a sparse map @p sm,
 * @f$L\left(i\rightarrow\mu\right)@f$, this function will return a version of
 * @p C where which only includes tiles that are non-zero according to @p sm.
 *
 * @note This function assumes that the independent indices in @p sm index
 *       columns of @p C and the dependent indices index rows of @p C.
 *
 * @param[in] C The dense transformation we are applying the sparse map to.
 * @param[in] sm The sparse map defining which blocks of @p C survive.
 *
 * @return A sparse form of C. It should be noted that the return is still a
 *         tensor, it is not a tensor of tensors.
 *
 */
type::tensor sparsify_basis(const type::tensor& C,
                            const SparseMap<ElementIndex, ElementIndex>& sm);

} // namespace libchemist::sparse_map
