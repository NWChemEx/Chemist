#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"

namespace libchemist::tensor::detail_ {

template<typename LHSType, typename RHSType>
class SubtOp : public OpLayer<SubtOp<LHSType, RHSType>> {
public:
    SubtOp(LHSType& lhs, RHSType& rhs) : m_lhs_(lhs), m_rhs_(rhs) {}

    template<typename ResultType>
    auto evaluate(ResultType&& r);

private:
    LHSType& m_lhs_;
    RHSType& m_rhs_;
};

template<typename LHSType, typename RHSType>
auto operator-(OpLayer<LHSType>& lhs, OpLayer<RHSType>& rhs) {
    return SubtOp<LHSType, RHSType>(lhs.downcast(), rhs.downcast());
}

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto SubtOp<LHSType, RHSType>::evaluate(ResultType&& r) {
    auto lhs_variant     = m_lhs_.evaluate(r);
    auto rhs_variant     = m_rhs_.evaluate(r);
    using lhs_variant_t  = std::decay_t<decltype(lhs_variant)>;
    using rhs_variant_t  = std::decay_t<decltype(rhs_variant)>;
    using result_variant = subt_variant_t<lhs_variant_t, rhs_variant_t>;
    auto l               = [rhs_variant{std::move(rhs_variant)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { return result_variant{lhs - rhs}; };
        return std::visit(m, rhs_variant);
    };
    return std::visit(l, lhs_variant);
}

} // namespace libchemist::tensor::detail_
