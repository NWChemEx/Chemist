#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace libchemist::tensor::detail_ {
template<typename T>
class LabeledTensorWrapper;


template<typename ResultType, typename LHSType, typename RHSType>
struct MultKernel;

#define REGISTER_MULT_KERNEL(Result, LHS, RHS)\
template<>\
struct MultKernel<Result, LHS, RHS> {\
    using labeled_result = LabeledTensorWrapper<Result> &;\
    using labeled_lhs    = const LabeledTensorWrapper<LHS>&;\
    using labeled_rhs    = const LabeledTensorWrapper<RHS>&;\
    using return_type    = labeled_variant_t<variant_type_t<Result>>;\
    static return_type eval(labeled_result, labeled_lhs, labeled_rhs);\
};\
extern template class MultKernel<Result, LHS, RHS>

REGISTER_MULT_KERNEL(SparseTensorWrapper, SparseTensorWrapper, SparseTensorWrapper);

#undef REGISTER_MULT_KERNEL

template<typename LHSType, typename RHSType>
class MultOp : public OpLayer<MultOp<LHSType, RHSType>> {
public:
    MultOp(LHSType lhs, RHSType rhs) :
      m_lhs_(std::move(lhs)), m_rhs_(std::move(rhs)) {}

    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    LHSType m_lhs_;
    RHSType m_rhs_;
};

template<typename LHSType, typename RHSType,
         typename = enable_if_expression_t<std::decay_t<LHSType>>,
         typename = enable_if_expression_t<std::decay_t<RHSType>>>
auto operator*(LHSType&& lhs, RHSType&& rhs) {
    using clean_lhs_t = std::decay_t<LHSType>;
    using clean_rhs_t = std::decay_t<RHSType>;
    using return_t    = MultOp<clean_lhs_t, clean_rhs_t>;
    return return_t(std::forward<LHSType>(lhs), std::forward<RHSType>(rhs));
}

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto MultOp<LHSType, RHSType>::variant(ResultType&& r) {
    using lhs_type    = typename std::decay_t<LHSType>::tensor_wrapper_type;
    using rhs_type    = typename std::decay_t<RHSType>::tensor_wrapper_type;
    using result_type = typename std::decay_t<ResultType>::tensor_wrapper_type;

    return MultKernel<result_type, lhs_type, rhs_type>::eval(r, m_lhs_, m_rhs_);
}

} // namespace libchemist::tensor::detail_
