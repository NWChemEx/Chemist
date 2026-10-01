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

// Checks chemist's spherical-AO normalization against libint2's, which is an
// implementation chemist shares no code with. chemist does not depend on
// libint2, so the reference values below were produced once by a throwaway
// libint2 program and hard-coded here; see the comment on the values.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/spherical_ao_class.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// One spherical AO to check, and its value according to libint2.
struct reference_value {
    const char* name;
    std::size_t l;
    long m;
    double value;
};

/** @brief The exponent shared by every AO checked below.
 *
 *  Arbitrary, but it must match what the libint2 program was run with.
 */
constexpr double zeta = 1.2;

/** @brief Normalized values of single-primitive spherical AOs at
 *         r = (0.3, -0.4, 0.5), from libint2 2.9.0.
 *
 *  Each AO is one primitive with a unit coefficient and exponent zeta,
 *  centered at the origin, and each value is
 *
 *  @f[
 *    \phi_{\ell m}(\vec{r}) = C \sum_{ijk} c^{(ijk)}_{\ell m}\,
 *                             x^i y^j z^k e^{-\zeta r^2}
 *  @f]
 *
 *  with @f$C@f$ the contraction coefficient libint2::Shell's ctor leaves
 *  behind (@f$N^{\chi} N^{G}@f$) and @f$c^{(ijk)}_{\ell m}@f$ libint2's
 *  solid-harmonic coefficients. libint2 does not evaluate functions in space,
 *  so the value was assembled from those two pieces, and the program then
 *  used libint2's *overlap engine* to confirm the function assembled is unit
 *  normalized:
 *
 *  1. the self-overlap of component m of a pure shell is 1, and
 *  2. @f$c^T S c = 1@f$ with @f$S@f$ the engine's Cartesian self-overlap,
 *     i.e. the coefficients used are the ones giving that normalized function.
 *
 *  So a match below means chemist's SphericalAO is the same normalized
 *  function libint2 integrates over, including the sign convention of each
 *  component. The components were chosen to cover every l up to f, both
 *  signs of m, and components with and without N^AO_ijk != 1 contributions.
 */
constexpr reference_value libint_values[]{
  {"s", 0, 0, 4.48455482355613722e-01},
  {"p(m=-1)", 1, -1, -3.93006693956847564e-01},
  {"p(m=0)", 1, 0, 4.91258367446059274e-01},
  {"p(m=+1)", 1, 1, 2.94755020467635631e-01},
  {"d(m=-2)", 2, -2, -2.58310357836833537e-01},
  {"d(m=0)", 2, 0, 1.55349536074546213e-01},
  {"d(m=+1)", 2, 1, 3.22887947296041922e-01},
  {"f(m=-2)", 3, -2, -2.82964819648930166e-01},
  {"f(m=+1)", 3, 1, 1.67777498960509919e-01},
  {"f(m=+3)", 3, 3, -1.12631906286013841e-01}};

} // namespace

TEST_CASE("experimental::SphericalAO normalization matches libint2") {
    std::vector<double> cs{1.0};
    std::vector<double> es{zeta};
    Point r(0.3, -0.4, 0.5);

    for(const auto& ref : libint_values) {
        SECTION(ref.name) {
            SphericalAO ao(cs.begin(), cs.end(), es.begin(), es.end(), ref.l,
                           ref.m, 0.0, 0.0, 0.0);

            REQUIRE(as_double(ao.normalized_evaluate(r)) ==
                    Catch::Approx(ref.value).epsilon(1e-10));

            // N^G is 1 for a one-primitive, unit-coefficient contraction; the
            // rest of the normalization is in the primitive and the transform.
            REQUIRE(as_double(ao.normalization_constant()) ==
                    Catch::Approx(1.0).epsilon(1e-15));
        }
    }
}
