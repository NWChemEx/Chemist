#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include <type_traits>
namespace libchemist::tensor::detail_ {

template<typename FloatType, typename TensorType>
class ScaleOp : public OpLayer<ScaleOp<FloatType, TensorType>> {
public:
    ScaleOp(FloatType c, TensorType& t) : m_c_(c), m_t_(t) {}

    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    FloatType m_c_;
    TensorType& m_t_;
};

template<typename FloatType, typename TensorType,
typename = std::enable_if_t<std::is_floating_point_v<FloatType>>>
auto operator*(FloatType c, OpLayer<TensorType>& rhs) {
    return ScaleOp<FloatType, TensorType>(c, rhs.downcast());
}

template<typename FloatType, typename TensorType,
typename = std::enable_if_t<std::is_floating_point_v<FloatType>>>
auto operator*(OpLayer<TensorType>& lhs, FloatType c) {
    return ScaleOp<FloatType, TensorType>(c, lhs.downcast());
}

// ---------------------------- Implementations --------------------------------

template<typename FloatType, typename TensorType>
template<typename ResultType>
auto ScaleOp<FloatType, TensorType>::variant(ResultType&& r) {
    auto da_variant = m_t_.variant(r);
    using da_variant_t = std::decay_t<decltype(da_variant)>;
    using float_as_variant = std::variant<FloatType>;
    using result_variant = mult_variant_t<float_as_variant, da_variant_t>;
    auto l = [&](auto&& result) { return result_variant(m_c_ * result); };
    return std::visit(l, da_variant);
}

}
