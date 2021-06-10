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

    /** @brief Creates a LabeledTensorWrapper given a std::variant wrapping a
     *         labeled TensorWrapper.
     *
     *  @param[in] v The std::variant which contains the labeled tensor.
     */
    LabeledTensorWrapper(variant_type v);

    /** @brief Evaluates the expression given to the assignment operator.
     *
     *  @tparam RHSType The type of the expression. Can be either a
     *                  LabeledTensorWrapper specialization or an OpWrapper
     *                  specialization.
     *
     *  @param[in] rhs The expression we are assigning to the labeled tensor we
     *                 are wrapping.
     *
     *  @return The current LabeledTensorWrapper instance, after assigning
     *          @p rhs to it for operator chaining purposes.
     */
    template<typename RHSType>
    auto operator=(RHSType&& rhs);
};

// -------------------------------- Implementations ----------------------------

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