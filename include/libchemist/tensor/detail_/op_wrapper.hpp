#pragma once
#include "libchemist/tensor/detail_/type_traits.hpp"
#include <variant>

namespace libchemist::tensor::detail_ {

template<typename VariantType>
class OpWrapper {
private:
    template<typename T>
    using disable_if_float_t = std::enable_if_t<!std::is_floating_point_v<T>>;

    template<typename T>
    using enable_if_float_t =
      std::enable_if_t<std::is_floating_point_v<T>, int>;

public:
    using variant_type = VariantType;

    OpWrapper(variant_type v) : m_tensor_(std::move(v)) {}

    const auto& tensor() const { return m_tensor_; }
    auto& tensor() { return m_tensor_; }

    template<typename RHSType>
    auto operator+(RHSType&& rhs) const;

    template<typename RHSType>
    auto operator-(RHSType&& rhs) const;

    template<typename RHSType, typename = disable_if_float_t<RHSType>>
    auto operator*(RHSType&& rhs) const;

    template<typename RHSType, enable_if_float_t<RHSType> = 0>
    auto operator*(const RHSType& rhs) const;

private:
    variant_type m_tensor_;
};

template<typename T, typename VariantType,
         typename = std::enable_if_t<std::is_floating_point_v<T>>>
auto operator*(const T& lhs, const OpWrapper<VariantType>& rhs) {
    return rhs * lhs;
}

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