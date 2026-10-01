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

// SphericalAOCommon implements the API shared by SphericalAO and
// SphericalAOView. These tests drive it through SphericalAO, and check the
// transform against Cartesian AOs built by hand. The absolute normalization is
// checked against libint2 in spherical_ao_libint_normalization.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/spherical_ao_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <cmath>
#include <type_traits>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// True when T::set_m(0) compiles; see cartesian_ao_common.cpp.
///@{
template<typename T, typename = void>
struct has_m_setter : std::false_type {};

template<typename T>
struct has_m_setter<T, std::void_t<decltype(std::declval<T&>().set_m(0))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_m_setter_v = has_m_setter<T>::value;
///@}

} // namespace

TEST_CASE("experimental::SphericalAOCommon") {
    Point r(0.3, -0.4, 0.5);

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    auto spherical = [&](std::size_t l, long m) {
        return SphericalAO(cs.begin(), cs.end(), es.begin(), es.end(), l, m,
                           1.0, 2.0, 3.0);
    };
    auto cartesian = [&](std::size_t i, std::size_t j, std::size_t k) {
        return CartesianAO(cs.begin(), cs.end(), es.begin(), es.end(), i, j, k,
                           1.0, 2.0, 3.0);
    };

    SphericalAO d0 = spherical(2, 0);

    SECTION("Accessors") {
        REQUIRE(d0.get_m() == 0);
        REQUIRE(d0.get_l() == 2);
        REQUIRE(d0.get_center() == Point(1.0, 2.0, 3.0));
        REQUIRE(d0.get_contracted_gaussian() ==
                d0.get_cartesian_shell().get_contracted_gaussian());
    }

    SECTION("set_m") {
        d0.set_m(-2);
        REQUIRE(d0.get_m() == -2);
        REQUIRE_THROWS_AS(d0.set_m(3), std::invalid_argument);
        REQUIRE_THROWS_AS(d0.set_m(-3), std::invalid_argument);
        REQUIRE(d0.get_m() == -2); // Strong guarantee
    }

    SECTION("set_m exists only when the AO is mutable") {
        STATIC_REQUIRE(has_m_setter_v<SphericalAO>);
        STATIC_REQUIRE_FALSE(has_m_setter_v<const_spherical_ao_view>);
    }

    SECTION("normalization_constant is N^G") {
        REQUIRE(
          as_double(d0.normalization_constant()) ==
          as_double(d0.get_contracted_gaussian().normalization_constant()));
    }

    SECTION("p functions are the Cartesian p functions") {
        // m = -1, 0, +1 are y, z, x with unit coefficients and N^AO = 1.
        auto py = cartesian(0, 1, 0);
        auto pz = cartesian(0, 0, 1);
        auto px = cartesian(1, 0, 0);
        REQUIRE(as_double(spherical(1, -1).normalized_evaluate(r)) ==
                Catch::Approx(as_double(py.normalized_evaluate(r))));
        REQUIRE(as_double(spherical(1, 0).normalized_evaluate(r)) ==
                Catch::Approx(as_double(pz.normalized_evaluate(r))));
        REQUIRE(as_double(spherical(1, 1).normalized_evaluate(r)) ==
                Catch::Approx(as_double(px.normalized_evaluate(r))));
    }

    SECTION("d(m=-2) is the normalized Cartesian d_xy") {
        // The coefficient is sqrt(3) = N^AO_110, so once N^AO is divided back
        // out of it, it multiplies the fully normalized d_xy by exactly 1.
        auto dxy = cartesian(1, 1, 0);
        REQUIRE(as_double(spherical(2, -2).normalized_evaluate(r)) ==
                Catch::Approx(as_double(dxy.normalized_evaluate(r))));

        // evaluate applies the coefficient as-is to the unnormalized d_xy.
        REQUIRE(as_double(spherical(2, -2).evaluate(r)) ==
                Catch::Approx(std::sqrt(3.0) * as_double(dxy.evaluate(r))));
    }

    SECTION("d(m=0) is zz - (xx + yy)/2") {
        auto xx = as_double(cartesian(2, 0, 0).normalized_evaluate(r));
        auto yy = as_double(cartesian(0, 2, 0).normalized_evaluate(r));
        auto zz = as_double(cartesian(0, 0, 2).normalized_evaluate(r));
        REQUIRE(as_double(d0.normalized_evaluate(r)) ==
                Catch::Approx(zz - 0.5 * (xx + yy)));
    }

    SECTION("evaluate over a set of points") {
        PointSet pts{Point(0.0, 0.0, 0.0), r};
        auto raw  = d0.evaluate(pts);
        auto norm = d0.normalized_evaluate(pts);
        REQUIRE(raw.size() == 2);
        REQUIRE(norm.size() == 2);
        REQUIRE(as_double(raw[1]) == as_double(d0.evaluate(r)));
        REQUIRE(as_double(norm[1]) == as_double(d0.normalized_evaluate(r)));
    }

    SECTION("Comparisons") {
        REQUIRE(d0 == spherical(2, 0));
        REQUIRE_FALSE(d0 != spherical(2, 0));
        REQUIRE(d0 != spherical(2, 1));
        REQUIRE(d0 != spherical(3, 0));

        // Owning versus aliasing is not considered
        SphericalAO other = spherical(2, 0);
        const_spherical_ao_view v(other);
        REQUIRE(d0 == v);
        REQUIRE(v == d0);
    }
}
