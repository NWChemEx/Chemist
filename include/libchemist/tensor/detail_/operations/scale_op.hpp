#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/type_traits/type_traits.hpp"
#include <type_traits>
namespace libchemist::tensor::detail_ {

/** @brief Implements scaling of a tensor in the operation layer.
 *
 *  This class is charged with determining what:
 *  ```
 *  <expr> * scalar
 *  ```
 *  or
 *  ```
 *  <scalar> * <expr>
 *  ```
 *  works out to. More specifically given am expression and a scalar this class
 *  is charged with scaling the tensor.
 *
 *  @tparam FloatType The type of the scalar;
 *  @tparam TensorType The type of the expression.
 */
template<typename FloatType, typename TensorType>
class ScaleOp : public OpLayer<ScaleOp<FloatType, TensorType>> {
public:
    /** @brief Makes a new ScaleOp which scales @p t by @p c.
     *
     *  @param[in] c The scalar.
     *  @param[in] t The tensor to scale.
     */
    ScaleOp(FloatType c, TensorType t) : m_c_(c), m_t_(std::move(t)) {}

    /** @brief Evaluates the wrapped expression.
     *
     *  Calling this function will actually scale the tensor.
     *
     *  @tparam ResultType The type of the labeled tensor that this expression
     *                     is being assigned to.
     *  @param[in] r The tensor we assigning the result to.
     *  @return The variant resulting from scaling the tensor.
     */
    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    /// The scalar
    FloatType m_c_;
    /// The expression
    TensorType m_t_;
};

/** @brief Syntactic sugar for scaling a tensor from the left.
 *
 *  @relates ScaleOp
 *
 *  @tparam FloatType The type of the scalar. Assumed to be a floating point
 *                    type.
 *  @tparam TensorType The type of the tensor. Assumed to be part of the
 *                     expression layer.
 *  @tparam <anonymous> Used to disable the overload when @p FloatType is not a
 *                      floating point type.
 *  @param[in] c The scalar.
 *  @param[in] rhs The tensor to scale.
 *
 *  @return The ScaleOp describing the scaling to be done.
 */
template<typename FloatType, typename TensorType,
         typename = std::enable_if_t<std::is_floating_point_v<FloatType>>>
auto operator*(FloatType c, OpLayer<TensorType>& rhs) {
    return ScaleOp<FloatType, TensorType>(c, rhs.downcast());
}

/** @brief Syntactic sugar for scaling a tensor from the right.
 *  @relates ScaleOp
 *
 *  @tparam FloatType The type of the scalar. Assumed to be a floating point
 *                    type.
 *  @tparam TensorType The type of the tensor. Assumed to be part of the
 *                     expression layer.
 *  @tparam <anonymous> Used to disable the overload when @p FloatType is not a
 *                      floating point type.
 *  @param[in] lhs The tensor to scale.
 *  @param[in] c The scalar.
 *
 *  @return The ScaleOp describing the scaling to be done.
 */
template<typename FloatType, typename TensorType,
         typename = std::enable_if_t<std::is_floating_point_v<FloatType>>>
auto operator*(OpLayer<TensorType>& lhs, FloatType c) {
    return ScaleOp<FloatType, TensorType>(c, lhs.downcast());
}

// ---------------------------- Implementations --------------------------------

template<typename FloatType, typename TensorType>
template<typename ResultType>
auto ScaleOp<FloatType, TensorType>::variant(ResultType&& r) {
    auto da_variant        = m_t_.variant(r);
    using da_variant_t     = std::decay_t<decltype(da_variant)>;
    using float_as_variant = std::variant<FloatType>;
    using result_variant   = mult_variant_t<float_as_variant, da_variant_t>;
    auto l = [&](auto&& result) { return result_variant(m_c_ * result); };
    return std::visit(l, da_variant);
}

} // namespace libchemist::tensor::detail_
