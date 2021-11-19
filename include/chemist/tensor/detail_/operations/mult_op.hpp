#pragma once
#include "chemist/tensor/detail_/op_layer.hpp"
#include "chemist/tensor/detail_/operations/mult_kernels.hpp"
#include "chemist/tensor/type_traits/type_traits.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace chemist::tensor::detail_ {
template<typename T>
class LabeledTensorWrapper;

/** @brief Implements the operator* of the expression layer.
 *
 *  @tparam LHSType The type on the left side of the multiplication operator.
 *                  Assumed to be a class derived from OpLayer.
 *  @tparam RHSType The type on the right side of the multiplication operator.
 *                  Assumed to be a class derived from OpLayer.
 */
template<typename LHSType, typename RHSType>
class MultOp : public OpLayer<MultOp<LHSType, RHSType>> {
public:
    /** @brief Creates a new MultOp instance
     *
     *  @param[in] lhs The instance on the left of the multiplication operator.
     *  @param[in] rhs The instance on the right of the multiplication operator.
     */
    MultOp(LHSType lhs, RHSType rhs) :
      m_lhs_(std::move(lhs)), m_rhs_(std::move(rhs)) {}

    /** @brief Evaluates the wrapped multiplication.
     *
     *  @param[in] r The LabeledTensorWrapper the multiplication is going to be
     *               assigned to.
     *
     *  @return The variant which results from subtracting @p rhs from @p lhs.
     */
    template<typename ResultType>
    auto variant(ResultType&& r);

private:
    /// The expression which was on the left
    LHSType m_lhs_;
    /// The expression which was on the right
    RHSType m_rhs_;
};

/** @brief Syntactic sugar for generating a MultOp instance.
 *
 *  @relates MultOp
 *
 *  @tparam LHSType The type of the expression on the left of the multiplication
 *                  sign. Assumed to be a class derived from OpLayer.
 *  @tparam RHSType The type of the expression to the right of the
 *                  multiplication sign.Assumed to be a class derived from
 *                  OpLayer.
 *
 *  @param[in] lhs The object on the left of the times sign.
 *  @param[in] rhs The object on the right of the times sign.
 *
 *  @return A SubtOp object describing the multiplication to perform.
 */
template<typename LHSType, typename RHSType,
         typename = enable_if_expression_t<std::decay_t<LHSType>>,
         typename = enable_if_expression_t<std::decay_t<RHSType>>>
auto operator*(LHSType&& lhs, RHSType&& rhs) {
    using clean_lhs_t = std::decay_t<LHSType>;
    using clean_rhs_t = std::decay_t<RHSType>;
    using return_t    = MultOp<clean_lhs_t, clean_rhs_t>;
    return return_t(std::forward<LHSType>(lhs), std::forward<RHSType>(rhs));
}

// ---------------------------- Implementations -------------------------------

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto MultOp<LHSType, RHSType>::variant(ResultType&& r) {
    using lhs_type    = typename std::decay_t<LHSType>::tensor_wrapper_type;
    using rhs_type    = typename std::decay_t<RHSType>::tensor_wrapper_type;
    using result_type = typename std::decay_t<ResultType>::tensor_wrapper_type;

    return MultKernel<result_type, lhs_type, rhs_type>::eval(r, m_lhs_, m_rhs_);
}

} // namespace chemist::tensor::detail_
