#pragma once
#include "chemist/tensor/detail_/op_layer.hpp"
#include "chemist/tensor/type_traits/type_traits.hpp"

namespace chemist::tensor::detail_ {

/** @brief Implements the operator- of the expression layer.
 *
 *  @tparam LHSType The type on the left side of the subtraction operator.
 *                  Assumed to be a class derived from OpLayer.
 *  @tparam RHSType The type on the right side of the subtraction operator.
 *                  Assumed to be a class derived from OpLayer.
 */
template<typename LHSType, typename RHSType>
class SubtOp : public OpLayer<SubtOp<LHSType, RHSType>> {
public:
    /** @brief Creates a new SubtOp instance
     *
     *  @param[in] lhs The instance on the left of the subtraction operator.
     *  @param[in] rhs The instance on the right of the subtraction operator.
     */
    SubtOp(LHSType lhs, RHSType rhs) :
      m_lhs_(std::move(lhs)), m_rhs_(std::move(rhs)) {}

    /** @brief Evaluates the wrapped subtraction.
     *
     *  @param[in] r The LabeledTensorWrapper the subtraction is going to be
     *               assigned to.
     *
     *  @return The variant which results from subtracting @p rhs from @p lhs.
     */
    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    /// The expression on the left of the minus sign
    LHSType m_lhs_;
    /// The expression on the right of the minus sign
    RHSType m_rhs_;
};

/** @brief Syntactic sugar for generating a SubtOp instance.
 *
 *  These four overloads of operator- allow you to subtract tensor wrappers of
 *  different const-ness via the expression layer.
 *
 *  @relates SubtOp
 *
 *  @tparam LHSType The type of the expression on the left of the minus sign.
 *                  Assumed to be a class derived from OpLayer.
 *  @tparam RHSType The type of the expression to the right of the minus sign.
 *                  Assumed to be a class derived from OpLayer.
 *
 *  @param[in] lhs The object on the left of the minus sign.
 *  @param[in] rhs The object on the right of the minus sign.
 *
 *  @return A SubtOp object describing the subtraction to perform.
 */
///@{
template<typename LHSType, typename RHSType>
auto operator-(OpLayer<LHSType>& lhs, OpLayer<RHSType>& rhs) {
    return SubtOp<LHSType, RHSType>(lhs.downcast(), rhs.downcast());
}

template<typename LHSType, typename RHSType>
auto operator-(const OpLayer<LHSType>& lhs, OpLayer<RHSType>& rhs) {
    return SubtOp<LHSType, RHSType>(lhs.downcast(), rhs.downcast());
}

template<typename LHSType, typename RHSType>
auto operator-(OpLayer<LHSType>& lhs, const OpLayer<RHSType>& rhs) {
    return SubtOp<LHSType, RHSType>(lhs.downcast(), rhs.downcast());
}

template<typename LHSType, typename RHSType>
auto operator-(const OpLayer<LHSType>& lhs, const OpLayer<RHSType>& rhs) {
    return SubtOp<LHSType, RHSType>(lhs.downcast(), rhs.downcast());
}
///@}

// ------------------------ Implementations ------------------------------------

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto SubtOp<LHSType, RHSType>::variant(ResultType&& r) {
    auto lhs_variant     = m_lhs_.variant(r);
    auto rhs_variant     = m_rhs_.variant(r);
    using lhs_variant_t  = std::decay_t<decltype(lhs_variant)>;
    using rhs_variant_t  = std::decay_t<decltype(rhs_variant)>;
    using result_variant = subt_variant_t<lhs_variant_t, rhs_variant_t>;
    auto l               = [rhs_variant{std::move(rhs_variant)}](auto&& lhs) {
        auto m = [&](auto&& rhs) { return result_variant{lhs - rhs}; };
        return std::visit(m, rhs_variant);
    };
    return std::visit(l, lhs_variant);
}

} // namespace chemist::tensor::detail_
