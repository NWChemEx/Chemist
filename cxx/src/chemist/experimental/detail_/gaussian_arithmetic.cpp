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

#include <algorithm>
#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <chemist/types/floating_point.hpp>
#include <cmath>
#include <span>
#include <stdexcept>

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

/// Computes @f$n!@f$ for a small, non-negative @p n
double factorial_(long n) {
    double result = 1.0;
    for(long k = 2; k <= n; ++k) result *= static_cast<double>(k);
    return result;
}

/// Computes the binomial coefficient @f$\binom{n}{k}@f$ for small @p n, @p k
double binomial_(long n, long k) {
    if(k < 0 || k > n) return 0.0;
    return factorial_(n) / (factorial_(k) * factorial_(n - k));
}

/// Returns @f$(-1)^{n}@f$, with C++'s truncating @p n % 2 for negative @p n
double parity_(long n) { return n % 2 ? -1.0 : 1.0; }

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

float_type contracted_gaussian_normalization(
  wtf::buffer::BufferView<const wtf::fp::Float> coefficients,
  wtf::buffer::BufferView<const wtf::fp::Float> exponents, std::size_t l) {
    if(coefficients.size() != exponents.size())
        throw std::runtime_error(
          "chemist::experimental: contracted_gaussian_normalization requires "
          "the coefficient and exponent buffers to be the same length.");

    // wtf::buffer::visit_contiguous_buffer_view visits the cross product of
    // concrete types the two buffers could each be holding, so this visitor
    // has to be instantiable for every mixed pair even though only the
    // matched pairs are reachable T1/T2 themselves may already come
    // back const-qualified, and requiring an explicit `const` in the
    // parameter would fail to deduce against a non-const alternative.
    auto visitor = [l]<typename T1, typename T2>(
                     std::span<T1> d, std::span<T2> zeta) -> float_type {
        using d_type = std::remove_const_t<T1>;
        using z_type = std::remove_const_t<T2>;
        if constexpr(!std::is_same_v<d_type, z_type>) {
            throw std::runtime_error(
              "chemist::experimental: contracted_gaussian_normalization "
              "requires the coefficient and exponent buffers to hold the "
              "same concrete floating-point type.");
        } else {
            using value_type = d_type;

            const auto n = d.size();
            // The (l + 3/2) exponent is a plain scalar power, not a
            // value_type-typed operand, exactly like the 0.75/0.5 exponents
            // in primitive_normalization above.
            const double l_exponent = static_cast<double>(l) + 1.5;

            auto sum = value_type(0.0);
            for(std::size_t p = 0; p < n; ++p) {
                for(std::size_t q = 0; q < n; ++q) {
                    auto two_sqrt_zp_zq =
                      value_type(2.0) * tensorwrapper::types::pow(
                                          value_type(zeta[p] * zeta[q]), 0.5);
                    auto ratio = two_sqrt_zp_zq / (zeta[p] + zeta[q]);
                    auto s_pq =
                      tensorwrapper::types::pow(value_type(ratio), l_exponent);
                    sum = sum + value_type(d[p] * d[q] * s_pq);
                }
            }

            auto n_g = tensorwrapper::types::pow(value_type(sum), -0.5);
            return float_type(value_type(n_g));
        }
    };
    return wtf::buffer::visit_contiguous_buffer_view<fp_types>(
      visitor, coefficients, exponents);
}

double cartesian_ao_normalization(std::size_t i, std::size_t j,
                                  std::size_t k) noexcept {
    const auto numerator   = double_factorial_2l_minus_1_(i + j + k);
    const auto denominator = double_factorial_2l_minus_1_(i) *
                             double_factorial_2l_minus_1_(j) *
                             double_factorial_2l_minus_1_(k);
    return std::sqrt(numerator / denominator);
}

double spherical_transform_coefficient(std::size_t l_in, long m,
                                       std::size_t i_in, std::size_t j_in,
                                       std::size_t k_in) noexcept {
    // This implementation was written by Claude and appears to be correct
    // based on numerical tests, but the equations have not been human-verified.

    // Signed copies, since the formula takes differences which can go
    // negative. The integer divisions below deliberately truncate toward zero,
    // exactly as in the reference implementation; the signs of the sine-like
    // components depend on it.
    const auto l  = static_cast<long>(l_in);
    const auto lx = static_cast<long>(i_in);
    const auto ly = static_cast<long>(j_in);
    const auto lz = static_cast<long>(k_in);

    const long abs_m = m < 0 ? -m : m;
    if(lx + ly + lz != l || abs_m > l) return 0.0;
    if((lx + ly - abs_m) % 2) return 0.0;

    const long j = (lx + ly - abs_m) / 2;
    if(j < 0) return 0.0;

    // Whether x^lx contributes to the cosine-like (m >= 0) or the sine-like
    // (m < 0) component.
    const double comp = (m >= 0) ? 1.0 : -1.0;
    const long i      = abs_m - lx;
    if(comp != parity_(i < 0 ? -i : i)) return 0.0;

    double pfac = std::sqrt((factorial_(2 * lx) * factorial_(2 * ly) *
                             factorial_(2 * lz) / factorial_(2 * l)) *
                            (factorial_(l - abs_m) / factorial_(l)) /
                            factorial_(l + abs_m) /
                            (factorial_(lx) * factorial_(ly) * factorial_(lz)));
    pfac /= static_cast<double>(1L << l);
    pfac *= (m < 0) ? parity_((i - 1) / 2) : parity_(i / 2);

    const long i_max = (l - abs_m) / 2;
    double sum       = 0.0;
    for(long ii = j; ii <= i_max; ++ii) {
        const double pfac1 = binomial_(l, ii) * binomial_(ii, j) * parity_(ii) *
                             factorial_(2 * (l - ii)) /
                             factorial_(l - abs_m - 2 * ii);
        double sum1      = 0.0;
        const long k_min = std::max((lx - abs_m) / 2, 0L);
        const long k_max = std::min(j, lx / 2);
        for(long kk = k_min; kk <= k_max; ++kk) {
            if(lx - 2 * kk <= abs_m)
                sum1 += binomial_(j, kk) * binomial_(abs_m, lx - 2 * kk) *
                        parity_(kk);
        }
        sum += pfac1 * sum1;
    }

    // This is N^AO_ijk; it is what makes the coefficients assume Cartesian
    // AOs normalized only up to N^G.
    sum *= cartesian_ao_normalization(i_in, j_in, k_in);

    constexpr double sqrt2 = 1.41421356237309504880;
    return (m == 0) ? pfac * sum : sqrt2 * pfac * sum;
}

} // namespace chemist::experimental::detail_
