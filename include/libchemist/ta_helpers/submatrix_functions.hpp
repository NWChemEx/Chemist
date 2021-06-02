#pragma once
#include "libchemist/types.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"

namespace libchemist::ta_helpers {

template<typename T>
using tensor_type = type::tensor<T>;
using mask_type   = TA::Tensor<float>;
using trange_type = TA::TiledRange;

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
tensor_type<T> submatrix(const tensor_type<T> full_matrix,
                         const mask_type mask);

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
tensor_type<T> expand_submatrix(const tensor_type<T> submatrix,
                                const trange_type full_trange,
                                const mask_type mask);

extern template tensor_type<double> submatrix(const tensor_type<double>,
                                              const mask_type);

extern template tensor_type<double> expand_submatrix(const tensor_type<double>,
                                                     const trange_type,
                                                     const mask_type);

} // libchemist::ta_helpers