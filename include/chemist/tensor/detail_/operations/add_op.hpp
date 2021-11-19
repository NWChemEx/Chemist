#pragma once
#include "chemist/tensor/detail_/op_layer.hpp"
#include "chemist/tensor/type_traits/type_traits.hpp"

namespace chemist::tensor::detail_ {

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
    /** @brief Creates a new AddOp by wraping the incoming expressions
     *
     *  @tparam[in] lhs The expression on the left of the addition operator.
     *  @tparam[in] rhs The expression on the right of the addition operator.
     */
    AddOp(LHSType lhs, RHSType rhs) :
      m_lhs_(std::move(lhs)), m_rhs_(std::move(rhs)) {}

    /** @brief Evaluates the wrapped expression.
     *
     *  @tparam ResultType The type of the expression this expression is being
     *                     evaluated into. Assumed to be an instantiation of
     *                     LabeledTensorWrapper.
     *  @param[in] r The tensor we are evaluating the expression into.
     *
     *  @return The result of adding @p lhs to @p rhs.
     */
    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    /// The expression appearing on the left of the addition sign
    LHSType m_lhs_;
    /// The expression appearing on the right of the addition sign
    RHSType m_rhs_;
};

/** @brief Syntactic sugar for creating an AddOp instance.
 *
 *  @relates AddOp
 *
 *  @tparam LHSType the expression type on the left of the addition operator.
 *  @tparam RHSType the expression type on the right of the addition operator.
 *  @tparam <anonymous> Used to disable this overload if LHSType is not part of
 *                      the expresssion layer.
 *  @tparam <anonymous> Used to disable this overload if RHSType is not part of
 *                      the expression layer.
 *  @param[in] lhs The expression on the left of the addition sign.
 *  @param[in] rhs The expression on the right of the addition sign.
 */
template<typename LHSType, typename RHSType,
         typename = enable_if_expression_t<std::decay_t<LHSType>>,
         typename = enable_if_expression_t<std::decay_t<RHSType>>>
auto operator+(LHSType&& lhs, RHSType&& rhs) {
    using clean_lhs_t = std::decay_t<LHSType>;
    using clean_rhs_t = std::decay_t<RHSType>;
    using return_t    = AddOp<clean_lhs_t, clean_rhs_t>;
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

} // namespace chemist::tensor::detail_
