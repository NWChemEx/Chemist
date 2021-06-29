#include "libchemist/tensor/detail_/labeled_tensor_wrapper.hpp"
#include "libchemist/tensor/detail_/operations/mult_op.hpp"

namespace libchemist::tensor::detail_ {

using t_wrapper   = SparseTensorWrapper;
using tot_wrapper = ToTWrapper;
using labeled_t   = LabeledTensorWrapper<t_wrapper>;
using labeled_tot = LabeledTensorWrapper<tot_wrapper>;

auto MultKernel<t_wrapper, t_wrapper, t_wrapper>::eval(labeled_t& result,
                                                       const labeled_t& lhs,
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

} // namespace libchemist::tensor::detail_