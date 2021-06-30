#pragma once
#include "libchemist/tensor/type_traits/is_expression.hpp"
#include "libchemist/tensor/type_traits/variant_type.hpp"
#include <tiledarray.h>
#include <utilities/type_traits/variant/variant_product.hpp>

namespace libchemist::tensor {
template<typename VariantType>
class TensorWrapper;


/** @brief Determines if a TA::DistArray type is a tensor-of-tensors
 *
 *  This type trait determines if @p T is a tensor-of-tensors. It simply
 *  wraps the call to the respective type trait in TiledArray.
 *
 *  @tparam T Assumed to be a specialization of TA::DistArray.
 */
template<typename T>
static constexpr bool is_tot_v =
  TA::detail::is_tensor_of_tensor_v<typename T::value_type>;

template<typename T>
struct ConstVariant;

template<typename...Args>
struct ConstVariant<std::variant<Args...>> {
  using type = std::variant<const std::decay_t<Args>...>;
};

template<typename T>
using const_variant_t = typename ConstVariant<T>::type;

template<typename T>
struct CleanVariant;

template<typename...Args>
struct CleanVariant<std::variant<Args...>> {
  using type = std::variant<std::decay_t<Args>...>;
};

template<typename T>
using clean_variant_t = typename CleanVariant<T>::type;

/** @brief Type resulting from labeling/annotating a tensor with string indices
 *
 *  This type trait is used to get the type resulting from labeling/annotating a
 *  tensor with string indices.
 *
 *  @tparam TensorType The type of the tensor we are labeling.
 */
template<typename TensorType>
using labeled_tensor_t = decltype(std::declval<TensorType>()("i,j"));

namespace detail_ {

/** @brief Primary template for deducing the type of a std::variant resulting
 *         from labeling/annotating the tensors in the input variant type.
 *
 *  The primary template is chosen when @p T is not an std::variant and will
 *  lead to a compilation error because it is not defined.
 *
 *  @param T When this template is selected @p T will be a type other than
 *           std::variant.
 */
template<typename T>
struct labeled_variant;

/** @brief Specializes labeled_variant for std::variant.
 *
 *  This specialization determines the type that results from
 *  labeling/annotating every tensor in the input variant. The resulting variant
 *  type is stored as the member type `type`.
 *
 *  @tparam Args The types in the std::variant. The types are assumed to be
 *               TA::DistArray instantiations.
 */
template<typename... Args>
struct labeled_variant<std::variant<Args...>> {
    using type = std::variant<labeled_tensor_t<Args>...>;
};

template<typename T>
struct const_labeled_variant;

template<typename... Args>
struct const_labeled_variant<std::variant<Args...>> {
    using type = std::variant<labeled_tensor_t<const Args>...>;
};

} // namespace detail_

/** @brief Public API for getting a variant of labeled tensors
 *
 *  This is the public API for turning a variant of tensors into a variant of
 *  labeled tensors. For the i-th type in @p TensorVariant the i-th type in the
 *  resulting variant will be the type which results from calling
 *  `operator()(std::string)` on a tensor of that type.
 *
 *  @tparam TensorVariant A variant of tensors.
 */
template<typename TensorVariant>
using labeled_variant_t =
  typename detail_::labeled_variant<TensorVariant>::type;

template<typename TensorVariant>
using const_labeled_variant_t =
  typename detail_::const_labeled_variant<TensorVariant>::type;

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

} // namespace libchemist::tensor
