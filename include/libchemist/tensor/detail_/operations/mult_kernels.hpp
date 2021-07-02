#pragma once
#include "libchemist/tensor/type_traits/labeled_variant.hpp"
#include "libchemist/tensor/type_traits/variant_type.hpp"
#include "libchemist/tensor/types.hpp"
#include <TiledArray/expressions/contraction_helpers.h>
#include <type_traits>
#include <variant>

namespace libchemist::tensor::detail_ {

/** @brief  Dispatches to the correct multiplication function based on the
 *          types of the tensors in the multiplication.
 *
 *  This is the primary template for the MultKernel; it is not defined. This
 *  template is chosen when no specialization exists for the requested
 *  operation, resulting in a compiler error.
 *
 *  @tparam ResultType The type of the resulting tensor wrapper. Assumed to be a
 *                     non-qualified TensorWrapper type.
 *  @tparam LHSType The type of the tensor wrapper on the left of the
 *                  multiplication sign. Assumed to be a non-qualified
 *                  TensorWrapper type.
 *  @tparam RHSType The type of the tensor wrapper on the right side of the
 *                  multiplication sign. Assumed to be a non-qualified
 *                  TensorWrapper type.
 */
template<typename ResultType, typename LHSType, typename RHSType>
struct MultKernel;

/** @brief Specializes MultKernel to the product of two non-hierarchical tensors
 *
 *  This specialization of MultKernel is selected when the user is multiplying
 *  two normal together (i.e., non-ToT), resulting in a third normal tensor. It
 *  simply dispatches to TA's operator*.
 */
template<>
struct MultKernel<type::SparseTensorWrapper, type::SparseTensorWrapper,
                  type::SparseTensorWrapper> {
    /// Type of the resulting tensor wrapper
    using result_type = type::SparseTensorWrapper;

    /// Type of a variant containing labeled results
    using return_type = labeled_variant_t<variant_type_t<result_type>>;

    /** @brief Performs the multiplication of two normal tensors which results
     *         in a third normal tensor.
     *
     *  @tparam ResultType The type of the tensor to use for the result.
     *                     @p ResultType is assumed to be a LabeledTensorWrapper
     *                     which annotates a read/write TensorWrapper.
     *  @tparam LHSType The type of the tensor appearing on the left of the
     *                  multiplication operator. @p LHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *  @tparam RHSType The type of the tensor appearing on the right of the
     *                  multiplication operator. @p RHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *
     *  @param[in] result The annotated tensor wrapper describing the resulting
     *                    product.
     *  @param[in] lhs The annotated tensor wrapper appearing on the left of the
     *                 multiplication operator.
     *  @param[in] rhs The annotated tensor wrapper appearing on the right of
     *                 the multiplication operator.
     *
     *  @return A LabeledTensorWrapper containing the result of the
     *          multiplication.
     */
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l                 = [&](auto&& result) {
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
};

// N.B. tensor = tensor * ToT and tensor = ToT * tensor are not possible

/** @brief Specializes MultKernel to the product of two hierarchical tensors
 *
 *  This specialization of MultKernel is selected when the user is multiplying
 *  two ToTs together, resulting in a normal tensor. It dispatches to TA's
 *  einsum.
 */
template<>
struct MultKernel<type::SparseTensorWrapper, type::ToTWrapper,
                  type::ToTWrapper> {
    /// Type of the wrapped tensor resulting from the product
    using result_type = type::SparseTensorWrapper;

    /// Type of the variant containing the labeled result
    using return_type = labeled_variant_t<variant_type_t<result_type>>;

    /** @brief Performs the multiplication of two ToTs which results in a
     *         normal tensor.
     *
     *  @tparam ResultType The type of the tensor to use for the result.
     *                     @p ResultType is assumed to be a LabeledTensorWrapper
     *                     which annotates a read/write TensorWrapper.
     *  @tparam LHSType The type of the tensor appearing on the left of the
     *                  multiplication operator. @p LHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *  @tparam RHSType The type of the tensor appearing on the right of the
     *                  multiplication operator. @p RHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *
     *  @param[in] result The annotated tensor wrapper describing the resulting
     *                    product.
     *  @param[in] lhs The annotated tensor wrapper appearing on the left of the
     *                 multiplication operator.
     *  @param[in] rhs The annotated tensor wrapper appearing on the right of
     *                 the multiplication operator.
     *
     *  @return A LabeledTensorWrapper containing the result of the
     *          multiplication.
     */
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l                 = [&](auto&& result) {
            auto m = [&](auto&& lhs) {
                auto n = [&](auto&& rhs) {
                    TA::expressions::einsum(result, lhs, rhs);
                    return result_variant_t(result);
                };
                return std::visit(n, rhs_variant);
            };
            return std::visit(m, lhs_variant);
        };
        return std::visit(l, result_variant);
    }
};

/** @brief Specializes MultKernel to the product of a non-hierarchical tensor
 *         and a ToT
 *
 *  This specialization of MultKernel is selected when the user is multiplying
 *  a normal tensor (i.e., non-ToT) with a ToT, resulting in a ToT. It
 *  dispatches to TA's einsum.
 */
template<>
struct MultKernel<type::ToTWrapper, type::SparseTensorWrapper,
                  type::ToTWrapper> {
    /// Type of the tensor wrapper associated with the result
    using result_type = type::ToTWrapper;

    /// Type of the labeled variant containing the result
    using return_type = labeled_variant_t<variant_type_t<result_type>>;

    /** @brief Performs the multiplication of a normal tensor with a ToT
     *         resulting in a ToT.
     *
     *  @tparam ResultType The type of the tensor to use for the result.
     *                     @p ResultType is assumed to be a LabeledTensorWrapper
     *                     which annotates a read/write TensorWrapper.
     *  @tparam LHSType The type of the tensor appearing on the left of the
     *                  multiplication operator. @p LHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *  @tparam RHSType The type of the tensor appearing on the right of the
     *                  multiplication operator. @p RHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *
     *  @param[in] result The annotated tensor wrapper describing the resulting
     *                    product.
     *  @param[in] lhs The annotated tensor wrapper appearing on the left of the
     *                 multiplication operator.
     *  @param[in] rhs The annotated tensor wrapper appearing on the right of
     *                 the multiplication operator.
     *
     *  @return A LabeledTensorWrapper containing the result of the
     *          multiplication.
     */
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l                 = [&](auto&& result) {
            auto m = [&](auto&& lhs) {
                auto n = [&](auto&& rhs) {
                    TA::expressions::einsum(result, lhs, rhs);
                    return result_variant_t(result);
                };
                return std::visit(n, rhs_variant);
            };
            return std::visit(m, lhs_variant);
        };
        return std::visit(l, result_variant);
    }
};

/** @brief Specializes MultKernel to the product of a ToT and anon-hierarchical
 *         tensor.
 *
 *  This specialization of MultKernel is selected when the user is multiplying
 *  a ToT by a normal tensor (i.e., non-ToT), resulting in a ToT. It dispatches
 *  to TA's einsum.
 */
template<>
struct MultKernel<type::ToTWrapper, type::ToTWrapper,
                  type::SparseTensorWrapper> {
    /// Type of the tensor wrapper associated with the result
    using result_type = type::ToTWrapper;

    /// Type of the variant holding the labeled result
    using return_type = labeled_variant_t<variant_type_t<result_type>>;

    /** @brief Performs the multiplication of a ToT with a normal tensor
     *         resulting in a ToT.
     *
     *  @tparam ResultType The type of the tensor to use for the result.
     *                     @p ResultType is assumed to be a LabeledTensorWrapper
     *                     which annotates a read/write TensorWrapper.
     *  @tparam LHSType The type of the tensor appearing on the left of the
     *                  multiplication operator. @p LHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *  @tparam RHSType The type of the tensor appearing on the right of the
     *                  multiplication operator. @p RHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *
     *  @param[in] result The annotated tensor wrapper describing the resulting
     *                    product.
     *  @param[in] lhs The annotated tensor wrapper appearing on the left of the
     *                 multiplication operator.
     *  @param[in] rhs The annotated tensor wrapper appearing on the right of
     *                 the multiplication operator.
     *
     *  @return A LabeledTensorWrapper containing the result of the
     *          multiplication.
     */
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l                 = [&](auto&& result) {
            auto m = [&](auto&& lhs) {
                auto n = [&](auto&& rhs) {
                    // Einsum requires ToT in right-most position
                    TA::expressions::einsum(result, rhs, lhs);
                    return result_variant_t(result);
                };
                return std::visit(n, rhs_variant);
            };
            return std::visit(m, lhs_variant);
        };
        return std::visit(l, result_variant);
    }
};

/** @brief Specializes MultKernel to the product of two ToTs
 *
 *  This specialization of MultKernel is selected when the user is multiplying
 *  two ToTs together, resulting in a third ToT. It dispatches to TA's einsum.
 */
template<>
struct MultKernel<type::ToTWrapper, type::ToTWrapper, type::ToTWrapper> {
    /// Type of the tensor wrapper associated with the result
    using result_type = type::ToTWrapper;

    /// Type of the variant holding the annotated result
    using return_type = labeled_variant_t<variant_type_t<result_type>>;

    /** @brief Performs the multiplication of two ToTs which results in a third
     *         ToT.
     *
     *  @tparam ResultType The type of the tensor to use for the result.
     *                     @p ResultType is assumed to be a LabeledTensorWrapper
     *                     which annotates a read/write TensorWrapper.
     *  @tparam LHSType The type of the tensor appearing on the left of the
     *                  multiplication operator. @p LHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *  @tparam RHSType The type of the tensor appearing on the right of the
     *                  multiplication operator. @p RHSType is assumed to be a
     *                  possibly cv-qualified LabeledTensorWrapper instance
     *                  which annotates a possibly cv-qualified TensorWrapper.
     *
     *  @param[in] result The annotated tensor wrapper describing the resulting
     *                    product.
     *  @param[in] lhs The annotated tensor wrapper appearing on the left of the
     *                 multiplication operator.
     *  @param[in] rhs The annotated tensor wrapper appearing on the right of
     *                 the multiplication operator.
     *
     *  @return A LabeledTensorWrapper containing the result of the
     *          multiplication.
     */
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l                 = [&](auto&& result) {
            auto m = [&](auto&& lhs) {
                auto n = [&](auto&& rhs) {
                    TA::expressions::einsum(result, lhs, rhs);
                    return result_variant_t(result);
                };
                return std::visit(n, rhs_variant);
            };
            return std::visit(m, lhs_variant);
        };
        return std::visit(l, result_variant);
    }
};

} // namespace libchemist::tensor::detail_
