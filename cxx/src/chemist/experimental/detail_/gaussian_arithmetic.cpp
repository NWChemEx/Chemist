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

#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <chemist/types/floating_point.hpp>

namespace chemist::experimental::detail_ {
namespace {

/// The list of concrete types the visitor below is instantiated for.
using fp_types = chemist::types::floating_point_types;

/// The ratio of a circle's circumference to its diameter, to double precision
constexpr double pi_ = 3.14159265358979323846;

/** @brief Computes @f$(2\ell-1)!!@f$, treating @f$(-1)!! = 1@f$.
 *
 *  @p l is always small (it is a Cartesian angular momentum), so a loop is
 *  simpler, and no less exact, than a closed-form expression.
 */
double double_factorial_2l_minus_1_(std::size_t l) {
    double result = 1.0;
    for(std::size_t k = 1; k <= l; ++k)
        result *= static_cast<double>(2 * k - 1);
    return result;
}

} // namespace

float_type primitive_normalization(const_float_reference exponent,
                                   std::size_t l) {
    auto visitor = [l](const auto& zeta) -> float_type {
        using value_type = std::decay_t<decltype(zeta)>;

        // Every scalar constant below is first embedded as a value_type,
        // rather than mixed in as a bare double, so that only same-type
        // arithmetic (already required to work by binary_op_ above) is ever
        // used. The UQ types' operators require an exact value_t match
        // (float or double, whichever the concrete type holds), which a
        // literal double is not guaranteed to be; each type's own
        // single-value ctor is what does that narrowing correctly.
        auto two_zeta    = value_type(2.0) * zeta;
        auto two_zeta_pi = two_zeta / value_type(pi_);
        // (2 zeta / pi)^{3/4}
        auto lead = tensorwrapper::types::pow(value_type(two_zeta_pi), 0.75);

        // (4 zeta)^l, computed by repeated multiplication since l is a small
        // non-negative integer.
        auto four_zeta   = value_type(4.0) * zeta;
        auto four_zeta_l = value_type(1.0);
        for(std::size_t k = 0; k < l; ++k)
            four_zeta_l = four_zeta_l * four_zeta;

        auto ratio = four_zeta_l / value_type(double_factorial_2l_minus_1_(l));
        auto tail  = tensorwrapper::types::pow(value_type(ratio), 0.5);

        return float_type(value_type(lead * tail));
    };
    return wtf::fp::visit_float_view<fp_types>(visitor, exponent);
}

} // namespace chemist::experimental::detail_
