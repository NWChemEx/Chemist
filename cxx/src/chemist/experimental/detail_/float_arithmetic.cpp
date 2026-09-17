/*
 * Copyright 2026 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/types/floating_point.hpp>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace chemist::experimental::detail_ {
namespace {

/** @brief The list of concrete types the visitors below are instantiated for.
 *
 *  This must be chemist's full list and not wtf::default_fp_types; the latter
 *  is only float/double/long double and would silently fail to restore the
 *  concrete type of any Sigma uncertainty-quantification value.
 */
using fp_types = chemist::types::floating_point_types;

/** @brief Applies @p op to two type-restored values.
 *
 *  wtf's visitation machinery visits the cross product of the types the two
 *  operands could be holding, so the callable it is given has to be
 *  instantiable for every mixed pair even though only the matched pairs are
 *  reachable. The `if constexpr` below is what makes that true: mixed pairs
 *  compile down to a throw rather than to arithmetic which, for the UQ types,
 *  would not compile at all.
 */
template<typename OpType>
float_type binary_op_(const_float_reference lhs, const_float_reference rhs,
                      OpType&& op) {
    auto visitor = [&op](const auto& a, const auto& b) -> float_type {
        using lhs_type = std::decay_t<decltype(a)>;
        using rhs_type = std::decay_t<decltype(b)>;
        if constexpr(std::is_same_v<lhs_type, rhs_type>) {
            return float_type(lhs_type(op(a, b)));
        } else {
            throw std::runtime_error(
              "chemist::experimental: arithmetic between floating-point "
              "values of different concrete types is not supported.");
        }
    };
    return wtf::fp::visit_float_view<fp_types>(visitor, lhs, rhs);
}

} // namespace

float_type add(const_float_reference lhs, const_float_reference rhs) {
    return binary_op_(lhs, rhs,
                      [](const auto& a, const auto& b) { return a + b; });
}

float_type subtract(const_float_reference lhs, const_float_reference rhs) {
    return binary_op_(lhs, rhs,
                      [](const auto& a, const auto& b) { return a - b; });
}

float_type multiply(const_float_reference lhs, const_float_reference rhs) {
    return binary_op_(lhs, rhs,
                      [](const auto& a, const auto& b) { return a * b; });
}

float_type sqrt(const_float_reference value) {
    auto visitor = [](const auto& v) -> float_type {
        using value_type = std::decay_t<decltype(v)>;
        // pow knows how to propagate uncertainty for the UQ types and falls
        // back to std::pow for the built-ins.
        return float_type(
          value_type(tensorwrapper::types::pow(value_type(v), 0.5)));
    };
    return wtf::fp::visit_float_view<fp_types>(visitor, value);
}

void assign(float_reference lhs, const_float_reference rhs) {
    auto visitor = [](auto& a, const auto& b) {
        using lhs_type = std::decay_t<decltype(a)>;
        using rhs_type = std::decay_t<decltype(b)>;
        if constexpr(std::is_same_v<lhs_type, rhs_type> &&
                     !std::is_const_v<std::remove_reference_t<decltype(a)>>) {
            a = b;
        } else {
            throw std::runtime_error(
              "chemist::experimental: can not assign a floating-point value "
              "to one of a different concrete type.");
        }
    };
    wtf::fp::visit_float_view<fp_types>(visitor, lhs, rhs);
}

float_type copy(const_float_reference value) {
    auto visitor = [](const auto& v) -> float_type {
        using value_type = std::decay_t<decltype(v)>;
        return float_type(value_type(v));
    };
    return wtf::fp::visit_float_view<fp_types>(visitor, value);
}

std::string to_string(const_float_reference value) {
    auto visitor = [](const auto& v) -> std::string {
        using value_type = std::decay_t<decltype(v)>;
        std::stringstream ss;
        constexpr auto digits = wtf::type_traits::precision_v<value_type>;
        if constexpr(digits > 0) ss << std::setprecision(digits);
        ss << v;
        return ss.str();
    };
    return wtf::fp::visit_float_view<fp_types>(visitor, value);
}

} // namespace chemist::experimental::detail_
