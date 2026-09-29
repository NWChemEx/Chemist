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

// Checks chemist's Cartesian-AO normalization against libint2's, which is an
// implementation chemist shares no code with. chemist does not depend on
// libint2, so the reference values below were produced once by a throwaway
// libint2 program and hard-coded here; see the comment on the values.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cartesian_ao_class.hpp>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// One Cartesian AO to check, and the constant libint2 gives for it.
struct reference_value {
    const char* name;
    std::size_t i, j, k;
    double normalization_constant;
};

/** @brief The exponent shared by every AO checked below.
 *
 *  Arbitrary, but it must match what the libint2 program was run with.
 */
constexpr double zeta = 1.2;

/** @brief Normalization constants for single-primitive Cartesian AOs, from
 *         libint2 2.11.0.
 *
 *  Each is the full constant which multiplies @f$x^i y^j z^k
 *  e^{-\zeta r^2}@f$ for a one-primitive contraction with a unit coefficient
 *  centered at the origin and zeta = 1.2, i.e.
 *  @f$N^{\chi} N^{G} N^{AO}_{ijk}@f$ with @f$N^{G} = 1@f$.
 *
 *  Obtaining these from libint2 took two steps, because libint2 follows the
 *  prevailing convention of dropping @f$N^{AO}_{ijk}@f$ (see
 *  docs/source/developer/design/basis_set/normalization.rst):
 *
 *  1. libint2::Shell's ctor renormalizes the contraction coefficients, giving
 *     @f$N^{\chi} N^{G}@f$ --- the part of the constant libint2 does apply.
 *  2. The remaining factor was read back out of libint2's *overlap engine*
 *     rather than from a formula: for a shell carrying libint2's
 *     normalization, the self-overlap of component @f$(i,j,k)@f$ is
 *     @f$1/(N^{AO}_{ijk})^2@f$, so @f$N^{AO}_{ijk}@f$ is the inverse square
 *     root of that diagonal element.
 *
 *  Going through the integral engine, rather than re-deriving
 *  @f$N^{AO}_{ijk}@f$ from double factorials, is what makes this an
 *  independent check instead of chemist's own formula written twice.
 */
constexpr reference_value libint_values[]{
  {"s", 0, 0, 0, 8.17139165538358192e-01},
  {"px", 1, 0, 0, 1.79026221434522692e+00},
  {"py", 0, 1, 0, 1.79026221434522692e+00},
  {"pz", 0, 0, 1, 1.79026221434522692e+00},
  {"dxy", 1, 1, 0, 3.92226799458411834e+00},
  {"dzz", 0, 0, 2, 2.26452248250699473e+00},
  {"fxyz", 1, 1, 1, 8.59325862885708958e+00},
  {"fxxz", 2, 0, 1, 4.96132018258671526e+00},
  {"fxxx", 3, 0, 0, 2.21876983728111288e+00}};

} // namespace

TEST_CASE("experimental::CartesianAO normalization matches libint2") {
    std::vector<double> cs{1.0};
    std::vector<double> es{zeta};

    for(const auto& ref : libint_values) {
        SECTION(ref.name) {
            CartesianAO ao(cs.begin(), cs.end(), es.begin(), es.end(), ref.i,
                           ref.j, ref.k, 0.0, 0.0, 0.0);

            // CartesianAO::normalization_constant is N^AO_ijk * N^G; the
            // remaining N^chi belongs to the primitive, because it differs from
            // primitive to primitive and so sits inside the contraction sum.
            // With one primitive there is only one of them to multiply in.
            const auto n_ao_n_g = as_double(ao.normalization_constant());
            const auto n_chi    = as_double(
              ao.get_contracted_gaussian()[0].normalization_constant());

            REQUIRE(n_ao_n_g * n_chi ==
                    Catch::Approx(ref.normalization_constant).epsilon(1e-10));
        }
    }
}

TEST_CASE("experimental::CartesianAO N^AO_ijk is 1 on a single axis") {
    // A consequence of the formula worth stating separately: libint2's values
    // above agree with chemist's *unnormalized-by-N^AO* convention exactly for
    // these components, and differ for the others. That is the whole reason
    // N^AO_ijk needs a home in chemist.
    std::vector<double> cs{1.0};
    std::vector<double> es{zeta};

    auto constant_of = [&](std::size_t i, std::size_t j, std::size_t k) {
        CartesianAO ao(cs.begin(), cs.end(), es.begin(), es.end(), i, j, k, 0.0,
                       0.0, 0.0);
        return as_double(ao.normalization_constant());
    };

    // N^G is 1 for a one-primitive unit-coefficient contraction, so these are
    // N^AO_ijk alone.
    REQUIRE(constant_of(0, 0, 0) == Catch::Approx(1.0).epsilon(1e-15));
    REQUIRE(constant_of(1, 0, 0) == Catch::Approx(1.0).epsilon(1e-15));
    REQUIRE(constant_of(0, 0, 2) == Catch::Approx(1.0).epsilon(1e-15));
    REQUIRE(constant_of(3, 0, 0) == Catch::Approx(1.0).epsilon(1e-15));

    // ... and these are not.
    REQUIRE(constant_of(1, 1, 0) ==
            Catch::Approx(1.7320508075688772).epsilon(1e-15));
    REQUIRE(constant_of(1, 1, 1) ==
            Catch::Approx(3.8729833462074170).epsilon(1e-15));
    REQUIRE(constant_of(2, 0, 1) ==
            Catch::Approx(2.2360679774997896).epsilon(1e-15));
}
