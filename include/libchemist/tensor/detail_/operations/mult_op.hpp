#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include "libchemist/types.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace libchemist::tensor::detail_ {
template<typename T>
class LabeledTensorWrapper;

template<typename ResultType, typename LHSType, typename RHSType>
struct MultKernel {
    using labeled_result = LabeledTensorWrapper<ResultType>;
    using labeled_lhs    = LabeledTensorWrapper<LHSType>;
    using labeled_rhs    = LabeledTensorWrapper<RHSType>;
    using result_variant =
      decltype(std::declval<ResultType>().variant(std::declval<ResultType>()));
    using return_type = std::decay_t<return_type>;
    static return_type eval(labeled_rhs& result, const labeled_lhs& lhs,
                            const labeled_rhs& rhs);
};

template<typename LHSType, typename RHSType>
class MultOp : public OpLayer<MultOp<LHSType, RHSType>> {
public:
    MultOp(LHSType lhs, RHSType rhs) :
      m_lhs_(std::move(lhs)), m_rhs_(std::move(rhs)) {}

    template<typename ResultType>
    auto variant(ResultType& r);

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

extern template MultKernel<SparseTensorWrapper, SparseTensorWrapper,
                           SparseTensorWrapper>;

} // namespace libchemist::tensor::detail_
