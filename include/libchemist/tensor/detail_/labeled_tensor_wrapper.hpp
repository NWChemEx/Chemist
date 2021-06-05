#pragma once
#include "libchemist/tensor/detail_/op_wrapper.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include <utility> // for std::move
#include <variant> // for std::visit

namespace libchemist::tensor::detail_ {

template<typename VariantType>
class LabeledTensorWrapper {
public:
    /// Type of the variant
    using variant_type = VariantType;

    LabeledTensorWrapper(variant_type v) : m_tensor_(std::move(v)) {}

    template<typename RHSType>
    auto operator+(RHSType&& rhs);

    template<typename RHSType>
    auto operator-(RHSType&& rhs);

    template<typename RHSType>
    auto operator*(RHSType&& rhs);

    template<typename RHSType>
    auto operator=(RHSType&& rhs);

    const auto& tensor() const { return m_tensor_; }
    auto& tensor() { return m_tensor_; }

private:
    variant_type m_tensor_;
};

template<typename VariantType>
template<typename RHSType>
auto LabeledTensorWrapper<VariantType>::operator+(RHSType&& rhs_tensor) {
    using r_variant = typename std::decay_t<RHSType>::variant_type;
    using add_type  = add_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<add_type>;

    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [lhs{std::forward<decltype(lhs)>(lhs)}](auto&& rhs) {
            return op_type(lhs + rhs);
        };
        return std::visit(m, rhs_tensor.tensor());
    };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
template<typename RHSType>
auto LabeledTensorWrapper<VariantType>::operator-(RHSType&& rhs_tensor) {
    using r_variant = typename std::decay_t<RHSType>::variant_type;
    using subt_type = subt_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<subt_type>;

    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [lhs{std::forward<decltype(lhs)>(lhs)}](auto&& rhs) {
            return op_type(lhs - rhs);
        };
        return std::visit(m, rhs_tensor.tensor());
    };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
template<typename RHSType>
auto LabeledTensorWrapper<VariantType>::operator*(RHSType&& rhs_tensor) {
    using r_variant = typename std::decay_t<RHSType>::variant_type;
    using mult_type = mult_variant_t<variant_type, r_variant>;
    using op_type   = OpWrapper<mult_type>;

    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [lhs{std::forward<decltype(lhs)>(lhs)}](auto&& rhs) {
            return op_type(lhs * rhs);
        };
        return std::visit(m, rhs_tensor.tensor());
    };
    return std::visit(l, m_tensor_);
}

template<typename VariantType>
template<typename RHSType>
auto LabeledTensorWrapper<VariantType>::operator=(RHSType&& rhs_tensor) {
    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { lhs = rhs; };
        std::visit(m, rhs_tensor.tensor());
    };
    std::visit(l, m_tensor_);
    return *this;
}

} // namespace libchemist::tensor::detail_