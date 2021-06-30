#pragma once
#include "libchemist/tensor/detail_/type_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include <TiledArray/expressions/contraction_helpers.h>
#include <type_traits>
#include <variant>

namespace libchemist::tensor::detail_ {

/** @brief  Dispatches to the correct multiplication function based on the
 *          types of the tensors in the multiplication.
 *
 *
 */
template<typename ResultType, typename LHSType, typename RHSType>
struct MultKernel;

/// Kernel for tensor = tensor * tensor
template<>
struct MultKernel<SparseTensorWrapper, SparseTensorWrapper, SparseTensorWrapper> {
    using result_type = SparseTensorWrapper;
    using return_type    = labeled_variant_t<variant_type_t<result_type>>;
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
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
};

/// tensor = tensor * ToT and tensor = ToT * tensor are not possible

/// Kernel for tensor = ToT * ToT
template<>
struct MultKernel<SparseTensorWrapper, ToTWrapper, ToTWrapper> {
    using result_type = SparseTensorWrapper;
    using return_type    = labeled_variant_t<variant_type_t<result_type>>;
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l = [&](auto&& result) {
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

/// Kernel for ToT = tensor * ToT
template<>
struct MultKernel<ToTWrapper, SparseTensorWrapper, ToTWrapper> {
    using result_type = ToTWrapper;
    using return_type = labeled_variant_t<variant_type_t<result_type>>;
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l = [&](auto&& result) {
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


/// Kernel for ToT = ToT * tensor
template<>
struct MultKernel<ToTWrapper, ToTWrapper, SparseTensorWrapper> {
    using result_type = ToTWrapper;
    using return_type    = labeled_variant_t<variant_type_t<result_type>>;
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l = [&](auto&& result) {
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

/// Kernel for ToT = ToT * ToT
template<>
struct MultKernel<ToTWrapper, ToTWrapper, ToTWrapper> {
    using result_type = ToTWrapper;
    using return_type = labeled_variant_t<variant_type_t<result_type>>;
    template<typename ResultType, typename LHSType, typename RHSType>
    static return_type eval(ResultType&& result, LHSType&& lhs, RHSType&& rhs) {
        auto lhs_variant       = lhs.variant(result);
        auto rhs_variant       = rhs.variant(result);
        auto result_variant    = result.variant(result);
        using result_variant_t = std::decay_t<decltype(result_variant)>;
        auto l = [&](auto&& result) {
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

}
