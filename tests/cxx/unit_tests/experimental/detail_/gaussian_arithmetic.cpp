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

// Only spherical_transform_coefficient is tested here. The other functions in
// gaussian_arithmetic.hpp predate this file and are tested through the
// classes which use them (primitive_common.cpp, contracted_gaussian_common.cpp,
// and cartesian_ao_libint_normalization.cpp).
//
// The values below can all be derived by hand from the real solid harmonics,
// written in terms of Cartesian AOs normalized only up to N^G. That is, the
// coefficient of (i,j,k) is the solid harmonic's coefficient of x^i y^j z^k
// times N^AO_ijk, which is 1 when all of l sits on one axis. chemist's values
// were additionally checked against libint2's for every coefficient with
// l <= 5 when the function was written; spherical_ao_libint_normalization.cpp
// is the permanent check against libint2.

#include "../../test_helpers.hpp"
#include <chemist/experimental/detail_/gaussian_arithmetic.hpp>
#include <cmath>

using namespace chemist::experimental::detail_;

TEST_CASE("experimental::detail_::spherical_transform_coefficient") {
    const double sqrt3 = std::sqrt(3.0);

    SECTION("s") {
        REQUIRE(spherical_transform_coefficient(0, 0, 0, 0, 0) ==
                Catch::Approx(1.0));
    }

    SECTION("p: m = -1, 0, +1 are y, z, x") {
        REQUIRE(spherical_transform_coefficient(1, -1, 0, 1, 0) ==
                Catch::Approx(1.0));
        REQUIRE(spherical_transform_coefficient(1, 0, 0, 0, 1) ==
                Catch::Approx(1.0));
        REQUIRE(spherical_transform_coefficient(1, 1, 1, 0, 0) ==
                Catch::Approx(1.0));

        // ... and nothing else contributes
        REQUIRE(spherical_transform_coefficient(1, -1, 1, 0, 0) == 0.0);
        REQUIRE(spherical_transform_coefficient(1, -1, 0, 0, 1) == 0.0);
        REQUIRE(spherical_transform_coefficient(1, 0, 1, 0, 0) == 0.0);
        REQUIRE(spherical_transform_coefficient(1, 1, 0, 1, 0) == 0.0);
    }

    SECTION("d, m = 0: zz - (xx + yy)/2") {
        REQUIRE(spherical_transform_coefficient(2, 0, 0, 0, 2) ==
                Catch::Approx(1.0));
        REQUIRE(spherical_transform_coefficient(2, 0, 2, 0, 0) ==
                Catch::Approx(-0.5));
        REQUIRE(spherical_transform_coefficient(2, 0, 0, 2, 0) ==
                Catch::Approx(-0.5));
        REQUIRE(spherical_transform_coefficient(2, 0, 1, 1, 0) == 0.0);
        REQUIRE(spherical_transform_coefficient(2, 0, 1, 0, 1) == 0.0);
        REQUIRE(spherical_transform_coefficient(2, 0, 0, 1, 1) == 0.0);
    }

    SECTION("d, m = -2: xy, carrying N^AO_110 = sqrt(3)") {
        REQUIRE(spherical_transform_coefficient(2, -2, 1, 1, 0) ==
                Catch::Approx(sqrt3));
        REQUIRE(spherical_transform_coefficient(2, -2, 2, 0, 0) == 0.0);
    }

    SECTION("d, m = -1 and +1: yz and xz") {
        REQUIRE(spherical_transform_coefficient(2, -1, 0, 1, 1) ==
                Catch::Approx(sqrt3));
        REQUIRE(spherical_transform_coefficient(2, 1, 1, 0, 1) ==
                Catch::Approx(sqrt3));
    }

    SECTION("d, m = +2: sqrt(3)/2 (xx - yy)") {
        REQUIRE(spherical_transform_coefficient(2, 2, 2, 0, 0) ==
                Catch::Approx(sqrt3 / 2.0));
        REQUIRE(spherical_transform_coefficient(2, 2, 0, 2, 0) ==
                Catch::Approx(-sqrt3 / 2.0));
        REQUIRE(spherical_transform_coefficient(2, 2, 0, 0, 2) == 0.0);
    }

    SECTION("f, m = 0: the z^3 coefficient is 1") {
        REQUIRE(spherical_transform_coefficient(3, 0, 0, 0, 3) ==
                Catch::Approx(1.0));
    }

    SECTION("Returns 0 when (i,j,k) is not in the shell") {
        REQUIRE(spherical_transform_coefficient(2, 0, 0, 0, 1) == 0.0);
        REQUIRE(spherical_transform_coefficient(1, 0, 0, 0, 2) == 0.0);
    }

    SECTION("Returns 0 when |m| > l") {
        REQUIRE(spherical_transform_coefficient(1, 2, 1, 0, 0) == 0.0);
        REQUIRE(spherical_transform_coefficient(1, -2, 0, 1, 0) == 0.0);
    }
}
