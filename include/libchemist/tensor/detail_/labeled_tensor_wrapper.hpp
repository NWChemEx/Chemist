#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/operations/operations.hpp"
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
class LabeledTensorWrapper
  : public OpLayer<LabeledTensorWrapper<TensorWrapperType>> {
public:
    /// Type used for annotation
    using annotation_type = std::string;

    /// Type of the tensor wrapper
    using tensor_wrapper_type = TensorWrapperType;

    /** @brief Creates a LabeledTensorWrapper given a std::variant wrapping a
     *         labeled TensorWrapper.
     *
     *  @param[in] v The std::variant which contains the labeled tensor.
     */
    LabeledTensorWrapper(annotation_type annotation, TensorWrapperType& t) :
      m_annotation_(std::move(annotation)), m_tensor_(t) {}

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
    auto evaluate(ResultType&&);

private:
    /// The annotation associated with the tensor
    std::string m_annotation_;

    /// The tensor associated with the annotation
    TensorWrapperType& m_tensor_;
};

// -------------------------------- Implementations ----------------------------

template<typename TensorWrapperType>
template<typename RHSType>
auto LabeledTensorWrapper<TensorWrapperType>::operator=(RHSType&& rhs_tensor) {
    auto rhs_variant = rhs_tensor.evaluate(*this);
    auto my_variant  = evaluate(*this);
    auto l           = [rhs_variant{std::move(rhs_variant)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { lhs = rhs; };
        std::visit(m, rhs_variant);
    };
    std::visit(l, my_variant);
    return *this;
}

template<typename TensorWrapperType>
template<typename ResultType>
auto LabeledTensorWrapper<TensorWrapperType>::evaluate(ResultType&&) {
    using variant     = typename TensorWrapperType::variant_type;
    using new_variant = labeled_variant_t<variant>;
    auto l            = [&](auto&& t) { return new_variant{t(m_annotation_)}; };
    return std::visit(l, m_tensor_.tensor());
}

} // namespace libchemist::tensor::detail_
