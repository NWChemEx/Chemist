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
    // TODO ensure LHSType and RHSType are LabeledTensorWrapper instances and
    //      not operations
    using result_variant_t = std::decay_t<decltype(result_variant)>;
    auto l                 = [&](auto&& result) {
        auto m                  = [&](auto&& lhs) {
            auto n                  = [&](auto&& rhs) {
                using r_t               = std::decay_t<decltype(result.array())>;
                constexpr bool r_is_tot = is_tot_v<r_t>;
                using l_t               = std::decay_t<decltype(lhs.array())>;
                constexpr bool l_is_tot = is_tot_v<l_t>;
                using rhs_t               = std::decay_t<decltype(rhs.array())>;
                constexpr bool rhs_is_tot = is_tot_v<rhs_t>;

                if constexpr(!r_is_tot) { // Result is not a ToT
                    if constexpr(!l_is_tot && !rhs_is_tot){ // No ToTs
                        result = lhs * rhs;
                    } else if constexpr(l_is_tot && rhs_is_tot){ // ToT with ToT
                        TA::expressions::einsum(result, lhs, rhs);
                    } else { // ToT with tensor or tensor with ToT
                      throw std::runtime_error("Mixed ToT and tensor contractions must result in a ToT");
                    }
                } else { // Result is a ToT
                    // einsum requires the ToT to be the right input
                    if constexpr (!rhs_is_tot && l_is_tot) {
                        TA::expressions::einsum(result, rhs, lhs);
                    } else if constexpr(!rhs_is_tot && !l_is_tot){
                        throw std::runtime_error("Tensor times tensor can't result in a ToT");
                    } else { // ToT * ToT and tensor * ToT
                        TA::expressions::einsum(result, lhs, rhs);
                    }
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
