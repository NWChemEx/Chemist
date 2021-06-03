#pragma once
#include "libchemist/types.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"

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
type::tensor<T> submatrix(const type::tensor<T>& full_matrix,
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
type::tensor<T> expand_submatrix(const type::tensor<T>& submatrix,
                                 const TA::TiledRange& full_trange,
                                 const TA::Tensor<float>& mask);

extern template type::tensor<double> submatrix(const type::tensor<double>&,
                                               const TA::Tensor<float>&);

extern template type::tensor<double> expand_submatrix(const type::tensor<double>&,
                                                      const TA::TiledRange&,
                                                      const TA::Tensor<float>&);

} // libchemist::ta_helpers