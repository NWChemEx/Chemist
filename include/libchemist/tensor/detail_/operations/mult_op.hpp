#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"
#include "libchemist/types.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace libchemist::tensor::detail_ {

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
typename=enable_if_expression_t<std::decay_t<LHSType>>,
typename= enable_if_expression_t<std::decay_t<RHSType>>>
auto operator*(LHSType&& lhs, RHSType&& rhs) {
    using clean_lhs_t = std::decay_t<LHSType>;
    using clean_rhs_t = std::decay_t<RHSType>;
    using return_t = MultOp<clean_lhs_t, clean_rhs_t>;
    return return_t(std::forward<LHSType>(lhs), std::forward<RHSType>(rhs));
}

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto MultOp<LHSType, RHSType>::variant(ResultType&& r) {
    auto lhs_variant    = m_lhs_.variant(r);
    auto rhs_variant    = m_rhs_.variant(r);
    auto result_variant = r.variant(r);
    // TODO ensure LHSType and RHSType are LabeledTensorWrapper instances
    using result_variant_t = std::decay_t<decltype(result_variant)>;
    auto l                 = [&](auto&& result) {
        using r_t               = std::decay_t<decltype(result.array())>;
        constexpr bool r_is_tot = is_tot_v<r_t>;
        auto m                  = [&](auto&& lhs) {
            using l_t               = std::decay_t<decltype(lhs.array())>;
            constexpr bool l_is_tot = is_tot_v<l_t>;
            auto n                  = [&](auto&& rhs) {
                using rhs_t               = std::decay_t<decltype(rhs.array())>;
                constexpr bool rhs_is_tot = is_tot_v<rhs_t>;
                if constexpr(!r_is_tot && !l_is_tot && !rhs_is_tot) {
                    result = lhs * rhs;
                } else if constexpr(rhs_is_tot && !l_is_tot) {
                    TA::expressions::einsum(result, rhs, lhs);
                } else {
                    TA::expressions::einsum(result, lhs, rhs);
                }
                return result_variant_t(result);
            };
            return std::visit(n, rhs_variant);
        };
        return std::visit(m, lhs_variant);
    };
    return std::visit(l, result_variant);
}

} // namespace libchemist::tensor::detail_
