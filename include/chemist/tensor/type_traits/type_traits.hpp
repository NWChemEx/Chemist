#pragma once
#include "chemist/tensor/type_traits/is_expression.hpp"
#include "chemist/tensor/type_traits/labeled_variant.hpp"
#include "chemist/tensor/type_traits/tensor_traits.hpp"
#include "chemist/tensor/type_traits/variant_type.hpp"
#include <tiledarray.h>
#include <utilities/type_traits/variant/variant_product.hpp>

namespace chemist::tensor {

/** @brief Determines the type that results from adding two tensors together.
 *
 *  @tparam LType The type of the tensor on the left side of the plus sign
 *  @tparam RType The type of the tensor on the right side of the plus sign
 */
template<typename LType, typename RType>
using add_expr_t = decltype(std::declval<LType>() + std::declval<RType>());

/** @brief Determines the type that results from subtracting two tensors.
 *
 *  @tparam LType The type of the tensor on the left side of the minus sign
 *  @tparam RType The type of the tensor on the right side of the minus sign
 */
template<typename LType, typename RType>
using subt_expr_t = decltype(std::declval<LType>() - std::declval<RType>());

/** @brief Determines the type that results from multiplying two tensors
 *         together.
 *
 *  @tparam LType The type of the tensor on the left side of the times sign
 *  @tparam RType The type of the tensor on the right side of the times sign
 */
template<typename LType, typename RType>
using mult_expr_t = decltype(std::declval<LType>() * std::declval<RType>());

/** @brief Typedef of a variant resulting from adding two variants together.
 *
 *  Given a std::variant with `n` types and a std::variant with `m` types this
 *  typedef defines an std::variant with `n*m` types. Thinking of the resulting
 *  std::variant as a row-major matrix, the `i,j`-th type is that resulting from
 *  adding the `i`-th type in @p LType with the `j`-th type in @p RType.
 *
 *  @tparam LType The variant on the left-side of the addition sign.
 *  @tparam RType The variant on the right-side of the addition sign.
 */
template<typename LType, typename RType>
using add_variant_t =
  utilities::type_traits::variant::product_t<add_expr_t, LType, RType>;

/** @brief Typedef of a variant resulting from subtracting two variants.
 *
 *  Given a std::variant with `n` types and a std::variant with `m` types this
 *  typedef defines an std::variant with `n*m` types. Thinking of the resulting
 *  std::variant as a row-major matrix, the `i,j`-th type is that resulting from
 *  subtracting the `j`-th type in @p RType from the `i`-th type in @p LType.
 *
 *  @tparam LType The variant on the left-side of the minus sign.
 *  @tparam RType The variant on the right-side of the minus sign.
 */
template<typename LType, typename RType>
using subt_variant_t =
  utilities::type_traits::variant::product_t<subt_expr_t, LType, RType>;

/** @brief Typedef of a variant resulting from multiplying two variants
 *         together.
 *
 *  Given a std::variant with `n` types and a std::variant with `m` types this
 *  typedef defines an std::variant with `n*m` types. Thinking of the resulting
 *  std::variant as a row-major matrix, the `i,j`-th type is that resulting from
 *  multiplying the `i`-th type in @p LType with the `j`-th type in @p RType.
 *
 *  @tparam LType The variant on the left-side of the multiplication sign.
 *  @tparam RType The variant on the right-side of the multiplication sign.
 */
template<typename LType, typename RType>
using mult_variant_t =
  utilities::type_traits::variant::product_t<mult_expr_t, LType, RType>;

} // namespace chemist::tensor
