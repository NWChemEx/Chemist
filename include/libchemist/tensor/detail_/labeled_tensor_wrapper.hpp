#pragma once
#include "libchemist/tensor/detail_/op_wrapper.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include <utility> // for std::move
#include <variant> // for std::visit

namespace libchemist::tensor::detail_ {

/** @brief Wraps a labeled tensor.
 *
 *  Typically the first step of the Einstein-based DSL is to annotate tensors
 *  with dummy indices. The pair of a tensor and an annotation is called a
 *  labeled tensor. This class wraps the instances that can emerge from labeling
 *  the tensor in TensorWrapper instance.
 *
 *  @tparam VariantType An std::variant whose types are the types resulting from
 *                      labeling a TensorWrapper instance.
 */
template<typename VariantType>
class LabeledTensorWrapper : public OpWrapper<VariantType> {
public:
    /// Type of the variant
    using variant_type = VariantType;

    LabeledTensorWrapper(variant_type v);

    template<typename RHSType>
    auto operator=(RHSType&& rhs);
};

template<typename VariantType>
LabeledTensorWrapper<VariantType>::LabeledTensorWrapper(variant_type v) :
  OpWrapper<VariantType>(std::move(v)) {}

template<typename VariantType>
template<typename RHSType>
auto LabeledTensorWrapper<VariantType>::operator=(RHSType&& rhs_tensor) {
    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { lhs = rhs; };
        std::visit(m, rhs_tensor.tensor());
    };
    std::visit(l, this->tensor());
    return *this;
}

} // namespace libchemist::tensor::detail_