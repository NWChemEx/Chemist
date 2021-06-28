#pragma once
#include "libchemist/tensor/detail_/op_layer.hpp"
#include "libchemist/tensor/detail_/type_traits.hpp"

namespace libchemist::tensor::detail_ {

/** @brief Implements operator+ for the operation layer.
 *
 *  This class is charged with determining what:
 *  ```
 *  <expr> + <expr>
 *  ```
 *  works out to. More specifically given two pieces of an expression this class
 *  is charged with adding the variants contained in those pieces.
 *
 *  @tparam LHSType The type of the object on the left side of operator+
 *  @tparam RHSType The type of the object on the right side of operator+
 */
template<typename LHSType, typename RHSType>
class AddOp : public OpLayer<AddOp<LHSType, RHSType>> {
public:
    AddOp(LHSType lhs, RHSType rhs) :
        m_lhs_(std::move(lhs)), m_rhs_(std::move(rhs)) {}

    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    LHSType m_lhs_;
    RHSType m_rhs_;
};

template<typename LHSType, typename RHSType,
         typename=enable_if_expression_t<std::decay_t<LHSType>>,
         typename=enable_if_expression_t<std::decay_t<RHSType>>>
auto operator+(LHSType&& lhs, RHSType&& rhs) {
    using clean_lhs_t = std::decay_t<LHSType>;
    using clean_rhs_t = std::decay_t<RHSType>;
    using return_t = AddOp<clean_lhs_t, clean_rhs_t>;
    return return_t(std::forward<LHSType>(lhs), std::forward<RHSType>(rhs));
}

// ----------------------- Implementations -------------------------------------

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto AddOp<LHSType, RHSType>::variant(ResultType&& r) {
    auto lhs_variant     = m_lhs_.variant(r);
    auto rhs_variant     = m_rhs_.variant(r);
    using lhs_variant_t  = std::decay_t<decltype(lhs_variant)>;
    using rhs_variant_t  = std::decay_t<decltype(rhs_variant)>;
    using result_variant = add_variant_t<lhs_variant_t, rhs_variant_t>;
    auto l               = [rhs_variant{std::move(rhs_variant)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { return result_variant{lhs + rhs}; };
        return std::visit(m, rhs_variant);
    };
    return std::visit(l, lhs_variant);
}

} // namespace libchemist::tensor::detail_
