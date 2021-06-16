#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
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
template<typename TensorWrapperType>
class LabeledTensorWrapper : public OpLayer<LabeledTensorWrapper<TensorType>> {
public:
    /// Type of the tensor wrapper
    using tensor_wrapper_type = TensorWrapperType;

    /** @brief Creates a LabeledTensorWrapper given a std::variant wrapping a
     *         labeled TensorWrapper.
     *
     *  @param[in] v The std::variant which contains the labeled tensor.
     */
    LabeledTensorWrapper(TensorWrapperType& t) : m_tensor_(t) {}

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

    template<typename ResultType>
    auto evaluate(ResultType&& r);
private:
    std::string m_annotation_;
    TensorWrapperType& m_tensor_;
};

// -------------------------------- Implementations ----------------------------


template<typename TensorWrapperType>
template<typename RHSType>
auto LabeledTensorWrapper<VariantType>::operator=(RHSType&& rhs_tensor) {
    auto l = [rhs_tensor{std::forward<RHSType>(rhs_tensor)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { lhs = rhs; };
        std::visit(m, rhs_tensor.tensor());
    };
    std::visit(l, this->tensor());
    return *this;
}

template<typename TensorWrapperType>
template<typename ResultType>
auto LabeledTensorWrapper<TensorWrapperType>::evaluate(ResultType&&) {
    using labeled_variant_t;
    auto l = [&](auto&& t) { return labeled_variant_t{t(m_annotation_)}; }
    return std::visit(l, m_tensor_.tensor());
}


} // namespace libchemist::tensor::detail_
