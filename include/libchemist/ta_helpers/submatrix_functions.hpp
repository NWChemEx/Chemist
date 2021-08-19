#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/types.hpp"

namespace libchemist::ta_helpers {

/** @brief Creates a new tensor that from selected tiles of another tensor.
 *
 * @tparam T The type of the tile elements in @p full_matrix.
 *
 * @param[in] full_matrix The tensor from which the submatrix originates.
 * @param[in] mask A mask whose values are used to determine which tiles are
 *                 copied.
 * @return A new tensor that is a submatrix of the original, potentially with
 *         some tiles set to zero based on the masking.
 */
template<typename T>
TA::DistArray<TA::Tensor<T>, TA::SparsePolicy> submatrix(
  const TA::DistArray<TA::Tensor<T>, TA::SparsePolicy>& full_matrix,
  const TA::Tensor<float>& mask);

/** @brief Expands the non-zero tiles of a tensor into a new tensor with a
 *         different TiledRange.
 *
 * @tparam T The type of the tile elements in @p full_matrix.
 *
 * @param[in] submatrix The tensor that is expanded.
 * @param[in] full_trange The TiledRange of the new tensor
 * @param[in] mask A mask whose values are used to determine where the
 *                 submatrix tiles are placed in the new tensor.
 * @return A new tensor that is a with the given TiledRange and whose tiles
 *         are either zero or copied from the submatrix.
 */
template<typename T>
TA::DistArray<TA::Tensor<T>, TA::SparsePolicy> expand_submatrix(
  const TA::DistArray<TA::Tensor<T>, TA::SparsePolicy>& submatrix,
  const TA::TiledRange& full_trange, const TA::Tensor<float>& mask);

extern template TA::DistArray<TA::Tensor<double>, TA::SparsePolicy> submatrix(
  const TA::DistArray<TA::Tensor<double>, TA::SparsePolicy>&,
  const TA::Tensor<float>&);

extern template TA::DistArray<TA::Tensor<double>, TA::SparsePolicy>
expand_submatrix(const TA::DistArray<TA::Tensor<double>, TA::SparsePolicy>&,
                 const TA::TiledRange&, const TA::Tensor<float>&);

} // namespace libchemist::ta_helpers
