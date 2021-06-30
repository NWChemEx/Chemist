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
 *  @note LabeledTensorWrapper only holds a reference to the wrapped tensor. It
 *        is assumed that the wrapped tensor remains valid for the lifetime of
 *        the LabeledTensorWrapper, i.e., unless you know what you're doing,
 *        don't save LabeledTensorWrapper instances to a variable, just leave
 *        them as unnamed temporaries.
 *
 *  @tparam TensorWrapperType The type of the TensorWrapper being annotated.
 */
template<typename TensorWrapperType>
class LabeledTensorWrapper
  : public OpLayer<LabeledTensorWrapper<TensorWrapperType>> {
public:
    /// Type used for annotation
    using annotation_type = std::string;

    /// Type of the tensor wrapper
    using tensor_wrapper_type = std::decay_t<TensorWrapperType>;

    /** @brief Creates a LabeledTensorWrapper given a std::variant wrapping a
     *         labeled TensorWrapper.
     *
     *  @param[in] v The std::variant which contains the labeled tensor.
     */
    LabeledTensorWrapper(annotation_type annotation, const TensorWrapperType& t) :
      m_annotation_(std::move(annotation)), m_tensor_(t) {}

    /** @brief Evaluates the expression given to the assignment operator.
     *
     *  This operation will evaluate @p rhs and assign the result to the wrapped
     *  tensor instance.
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
    template<typename RHSType, typename = enable_if_expression_t<std::decay_t<RHSType>>>
    auto operator=(RHSType&& rhs);

    /** @brief Returns a variant containing the result of annotating the wrapped
     *         tensor.
     *
     *  Until this function is called the LabeledTensorWrapper simply holds the
     *  annotation for the tensor and the wrapped TA::DistArray instance. This
     *  call will actaully annotate the TA::DistArray instance and wrap it in a
     *  variant. The resulting variant can then be combined with other variants
     *  to create an expression.
     *
     *  @param[in] <anonymous> The `LabeledTensorWrapper` we are assigning the
     *                         expression to. This parameter is ignored because
     *                         we defer to TA to handle permutations.
     *  @return A variant containing the result of labeling the wrapped tensor.
     */
    template<typename ResultType>
    auto variant(LabeledTensorWrapper<ResultType>&) const;

    //template<typename ResultType>
    //auto variant(LabeledTensorWrapper<ResultType>&)const;

private:
    /// The annotation associated with the tensor
    std::string m_annotation_;

    /// The tensor associated with the annotation
    const TensorWrapperType& m_tensor_;
};

// -------------------------------- Implementations ----------------------------

#define LABELED_TENSOR_WRAPPER LabeledTensorWrapper<TensorWrapperType>

template<typename TensorWrapperType>
template<typename RHSType, typename>
auto LABELED_TENSOR_WRAPPER::operator=(RHSType&& rhs_tensor) {
    auto rhs_variant = rhs_tensor.variant(*this);
    auto my_variant  = variant(*this);
    auto l           = [rhs_variant{std::move(rhs_variant)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { lhs = rhs; };
        std::visit(m, rhs_variant);
    };
    std::visit(l, my_variant);
    return *this;
}

template<typename TensorWrapperType>
template<typename ResultType>
auto LABELED_TENSOR_WRAPPER::variant(LabeledTensorWrapper<ResultType>&) const {
    constexpr bool is_const_wrapper = std::is_const_v<TensorWrapperType>;
    using variant     = typename tensor_wrapper_type::variant_type;
    //using const_variant = typename clean_wrapper::const_variant_type;
    //using v_t = std::conditional_t<is_const_wrapper, const_variant, variant>;
    using new_variant = labeled_variant_t<variant>;
    auto l            = [&](auto&& t) { return new_variant{t(m_annotation_)}; };
    return std::visit(l, m_tensor_.variant());
}

// template<typename TensorWrapperType>
// template<typename ResultType>
// auto LABELED_TENSOR_WRAPPER::variant(LabeledTensorWrapper<ResultType>&) const {
//     constexpr bool is_const
//     using clean_wrapper = std::decay_t<TensorWrapperType>;
//     using const_variant = typename clean_wrapper::const_variant_type;
//     using labeled_variant = labeled_variant_t<const_variant>;
//     auto l            = [&](auto&& t) { return labeled_variant(t(m_annotation_)); };
//     return std::visit(l, m_tensor_.variant());
// }

#undef LABELED_TENSOR_WRAPPER

} // namespace libchemist::tensor::detail_
