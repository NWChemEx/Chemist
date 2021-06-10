#pragma once
#include "libchemist/tensor/detail_/type_traits.hpp"
#include <variant>

namespace libchemist::tensor::detail_ {

/** @brief Wraps the result of a tensor operation.
 *
 *  This is the common type for the wrapper which results from a tensor
 *  expression. It supports further expression chaining to create even more
 *  complicated expressions.
 *
 *  @tparam VariantType The type of the std::variant wrapped in this class. The
 *                      types in the std::variant will be labeled tensor
 *                      instances, or the types resulting from combining labeled
 *                      tensors with operations.
 */
template<typename VariantType>
class OpWrapper {
private:
    /** @brief Type trait which is used to disable an overload if @p T is a
     *         floating point type.
     *
     *  This type trait can be used via SFINAE to selectively disable overloads
     *  when a template type parameter is a floating point type.
     *
     *  @tparam T The type which is being examined for whether or not it is a
     *          floating-point type.
     */
    template<typename T>
    using disable_if_float_t = std::enable_if_t<!std::is_floating_point_v<T>>;

    /** @brief Type trait which is used to enable an overload if @p T is a
     *         floating point type.
     *
     *  This type trait can be used via SFINAE to selectively enable overloads
     *  when a template type parameter is a floating point type. If @p T is a
     *  floating point type this type trait evaluates to the type integer.
     *
     *  @tparam T The type which is being examined for whether or not it is a
     *          floating-point type.
     */
    template<typename T>
    using enable_if_float_t =
      std::enable_if_t<std::is_floating_point_v<T>, int>;

public:
    /// Type of the variant which is wrapped in this OpWrapper
    using variant_type = VariantType;

    /** @brief Creates a new OpWrapper which wraps the provided std::variant.
     *
     *  @param[in] v The variant containg a tensor expression we are wrapping.
     */
    OpWrapper(variant_type v) : m_tensor_(std::move(v)) {}

    /** @brief Returns the std::varaint in this OpWrapper instance.
     *
     *  This member is used for getting a read-only reference to the
     *  std::variant wrapped in the OpWrapper.
     *
     *  @return A read-only reference to the wrapped std::variant.
     */
    const auto& tensor() const { return m_tensor_; }

    /** @brief Returns the std::varaint in this OpWrapper instance.
     *
     *  This member is used for getting a read/write reference to the
     *  std::variant wrapped in the OpWrapper.
     *
     *  @return A read/write reference to the wrapped std::variant.
     */
    auto& tensor() { return m_tensor_; }

    /** @brief Adds @p rhs to the wrapped tensor expression.
     *
     *  @tparam RHSType The type of the tensor expression we are adding to the
     *                  wrapped tensor expression.
     *
     *  @param[in] rhs The tensor expression being added to the wrapped tensor
     *                 expression.
     *  @return An OpWrapper wrapping the result of adding the wrapped tensor
     *          expression to @p rhs.
     */
    template<typename RHSType>
    auto operator+(RHSType&& rhs) const;

    /** @brief Subtracts @p rhs from the wrapped tensor expression.
     *
     *  @tparam RHSType The type of the tensor expression we are subtracting
     *                  from the wrapped tensor expression.
     *
     *  @param[in] rhs The tensor expression being subtracted from the wrapped
     *                 tensor expression.
     *  @return An OpWrapper wrapping the result of subtracting @p rhs from the
     *          wrapped tensor expression.
     */
    template<typename RHSType>
    auto operator-(RHSType&& rhs) const;

    /** @brief Multiples @p rhs with the wrapped tensor expression.
     *
     *  @tparam RHSType The type of the tensor expression we are multiplying
     *                  with the wrapped tensor expression.
     *  @tparam <anonymous> A type used to disable this overload when @p RHSType
     *                      is a floating-point type.
     *
     *  @param[in] rhs The tensor expression being multiplied with the wrapped
     *                 tensor expression.
     *  @return An OpWrapper wrapping the result of right multiplying the
     *          wrapped tensor expression by @p rhs.
     */
    template<typename RHSType, typename = disable_if_float_t<RHSType>>
    auto operator*(RHSType&& rhs) const;

    /** @brief Scales the wrapped expression by @p rhs.
     *
     *  @tparam RHSType The type of scalar we are multiplying the wrapped tensor
     *                  expression by.
     * @tparam <anonymous> An unnamed integer used to enable this overload when
     *                     @p RHSType is a floating-point type.
     *
     *  @param[in] rhs The floating-point value we are scaling this expression
     *                 by.
     *  @return An OpWrapper wrapping the result of scaling the wrapped tensor
     *          expression by @p rhs.
     */
    template<typename RHSType, enable_if_float_t<RHSType> = 0>
    auto operator*(const RHSType& rhs) const;

private:
    /// A std::variant holding the current tensor expression
    variant_type m_tensor_;
};

/** @brief Allows a floating point value to be right-multiplied by an OpWrapper.
 *
 *  @relates OpWrapper
 *
 *  @tparam T The type of the floating-point value.
 *  @tparam VariantType The type of the std::variant in the OpWrapper
 *  @tparam <anonymous> Type used to selectively enable this function only when
 *                      @p T is a floating point type.
 *
 *  @param[in] lhs The floating point value we are multiplying @p rhs by.
 *  @param[in] rhs The tensor expression we are scaling by @p lhs.
 *
 *  @return The result of left-multiplying @p rhs by @p lhs.
 */
template<typename T, typename VariantType,
         typename = std::enable_if_t<std::is_floating_point_v<T>>>
auto operator*(const T& lhs, const OpWrapper<VariantType>& rhs) {
    return rhs * lhs;
}

// ------------------------- Implementations -----------------------------------

template<typename VariantType>
template<typename RHSType>
auto OpWrapper<VariantType>::operator+(RHSType&& rhs_tensor) const {
    using r_variant = typename std::decay_t<RHSType>::variant_type;
    using add_type  = add_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<add_type>;

    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [lhs{std::forward<decltype(lhs)>(lhs)}](auto&& rhs) {
            return op_type(lhs + rhs);
        };
        return std::visit(m, rhs_tensor.tensor());
    };
    return std::visit(l, tensor());
}

template<typename VariantType>
template<typename RHSType>
auto OpWrapper<VariantType>::operator-(RHSType&& rhs_tensor) const {
    using r_variant = typename std::decay_t<RHSType>::variant_type;
    using subt_type = subt_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<subt_type>;

    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [lhs{std::forward<decltype(lhs)>(lhs)}](auto&& rhs) {
            return op_type(lhs - rhs);
        };
        return std::visit(m, rhs_tensor.tensor());
    };
    return std::visit(l, tensor());
}

template<typename VariantType>
template<typename RHSType, typename>
auto OpWrapper<VariantType>::operator*(RHSType&& rhs_tensor) const {
    using r_variant = typename std::decay_t<RHSType>::variant_type;
    using mult_type = mult_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<mult_type>;

    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [lhs{std::forward<decltype(lhs)>(lhs)}](auto&& rhs) {
            return op_type(lhs * rhs);
        };
        return std::visit(m, rhs_tensor.tensor());
    };
    return std::visit(l, tensor());
}

template<typename VariantType>
template<typename RHSType,
         std::enable_if_t<std::is_floating_point_v<RHSType>, int>>
auto OpWrapper<VariantType>::operator*(const RHSType& rhs) const {
    using r_variant = std::variant<std::decay_t<RHSType>>;
    using mult_type = mult_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<mult_type>;

    auto l = [&](auto&& lhs) { return op_type(lhs * rhs); };
    return std::visit(l, tensor());
}

} // namespace libchemist::tensor::detail_