#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/detail_/operations/mult_op.hpp"

namespace libchemist::tensor::detail_ {

using t_wrapper   = SparseTensorWrapper;
using tot_wrapper = ToTWrapper;
using labeled_t   = LabeledTensorWrapper<t_wrapper>;
using labeled_tot = LabeledTensorWrapper<tot_wrapper>;

#define MULTKERNEL_T_T_T MultKernel<t_wrapper, t_wrapper, t_wrapper>

typename MULTKERNEL_T_T_T::return_type
MULTKERNEL_T_T_T::eval(labeled_t& result, const labeled_t& lhs,
                                          const labeled_t& rhs) {
    auto lhs_variant       = lhs.variant(result);
    auto rhs_variant       = rhs.variant(result);
    auto result_variant    = result.variant(result);
    using result_variant_t = std::decay_t<decltype(result_variant)>;

    auto l = [&](auto&& result) {
        auto m = [&](auto&& lhs) {
            auto n = [&](auto&& rhs) {
                result = lhs * rhs;
                return result_variant_t(result);
            };
            return std::visit(n, rhs_variant);
        };
        return std::visit(m, lhs_variant);
    };
    return std::visit(l, result_variant);
}

template class MULTKERNEL_T_T_T;

#undef MULTKERNEL_T_T_T

} // namespace libchemist::tensor::detail_
