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

// CartesianAOCommon implements the API shared by CartesianAO and
// CartesianAOView. It has no state of its own to construct, so these tests
// drive it through CartesianAO --- the simplest concrete derived class --- and
// pull in CartesianAOView only where a property genuinely spans two different
// derived types. CartesianAO's own ctors, assignment, swap, and serialization
// are tested in cartesian_ao_class.cpp; CartesianAOView's own ctors,
// assignment, aliasing, and conversions are tested in cartesian_ao_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cartesian_ao_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <cmath>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/** @brief True when T::set_i(0) compiles.
 *
 *  The experimental setters are removed by SFINAE when the object models
 *  read-only state, rather than being present and throwing. This trait is what
 *  lets a test assert that, since "does not compile" can not be a runtime
 *  assertion. Defined locally rather than added to the shared
 *  experimental_test_helpers.hpp, following contracted_gaussian_common.cpp's
 *  precedent.
 */
///@{
template<typename T, typename = void>
struct has_i_setter : std::false_type {};

template<typename T>
struct has_i_setter<T, std::void_t<decltype(std::declval<T&>().set_i(0))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_i_setter_v = has_i_setter<T>::value;
///@}

} // namespace

TEST_CASE("experimental::CartesianAOCommon") {
    Point origin(0.0, 0.0, 0.0);
    Point r(0.3, -0.4, 0.5);

    // One-primitive d_xy function: i = 1, j = 1, k = 0, so l = 2.
    std::vector<double> cs{1.0};
    std::vector<double> es{1.2};
    CartesianAO dxy(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(1),
                    std::size_t(1), std::size_t(0), 0.0, 0.0, 0.0);

    // Two-primitive d_zz function, for the cases which need a real contraction.
    std::vector<double> cs2{1.0, 1.0};
    std::vector<double> es2{1.0, 2.0};
    CartesianAO dzz(cs2.begin(), cs2.end(), es2.begin(), es2.end(),
                    std::size_t(0), std::size_t(0), std::size_t(2), 0.0, 0.0,
                    0.0);

    SECTION("get_i/get_j/get_k") {
        REQUIRE(dxy.get_i() == 1);
        REQUIRE(dxy.get_j() == 1);
        REQUIRE(dxy.get_k() == 0);

        REQUIRE(dzz.get_i() == 0);
        REQUIRE(dzz.get_j() == 0);
        REQUIRE(dzz.get_k() == 2);
    }

    SECTION("get_l is the sum of the powers") {
        REQUIRE(dxy.get_l() == 2);
        REQUIRE(dzz.get_l() == 2);

        // It is not stored separately, so it can not disagree with the
        // contraction it was built with.
        REQUIRE(dxy.get_l() == dxy.get_contracted_gaussian().get_l());
        REQUIRE(dzz.get_l() == dzz.get_contracted_gaussian().get_l());
    }

    SECTION("get_contracted_gaussian aliases, rather than copies") {
        // Writing through the returned view is visible through *this.
        dxy.get_contracted_gaussian().set_center(Point(1.0, 2.0, 3.0));
        REQUIRE(dxy.get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("get_center is reached through the contracted Gaussian") {
        REQUIRE(dxy.get_center() == origin);
        REQUIRE(dxy.get_center() == dxy.get_contracted_gaussian().get_center());

        CartesianAO moved(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(1), std::size_t(1), std::size_t(0), 1.0,
                          2.0, 3.0);
        REQUIRE(moved.get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("set_i/set_j/set_k") {
        dxy.set_i(2ul);
        REQUIRE(dxy.get_i() == 2);

        dxy.set_j(0ul);
        REQUIRE(dxy.get_j() == 0);

        dxy.set_k(1ul);
        REQUIRE(dxy.get_k() == 1);
    }

    SECTION("set_i/set_j/set_k keep the contraction's l in sync") {
        // l is i+j+k, and the contraction needs l to normalize itself, so
        // changing a power has to change the contraction's l too.
        dxy.set_k(2ul);
        REQUIRE(dxy.get_l() == 4);
        REQUIRE(dxy.get_contracted_gaussian().get_l() == 4);

        dxy.set_i(0ul);
        dxy.set_j(0ul);
        REQUIRE(dxy.get_l() == 2);
        REQUIRE(dxy.get_contracted_gaussian().get_l() == 2);
    }

    SECTION("Setters are gated by constness, not by which derived class") {
        STATIC_REQUIRE(has_i_setter_v<CartesianAO>);
        STATIC_REQUIRE(has_i_setter_v<cartesian_ao_view>);
        STATIC_REQUIRE_FALSE(has_i_setter_v<const_cartesian_ao_view>);
    }

    SECTION("normalization_constant is N^AO_ijk times the contraction's") {
        // N^AO_xy = sqrt((2*2-1)!! / ((2*1-1)!!(2*1-1)!!(-1)!!)) = sqrt(3)
        auto n_g =
          as_double(dxy.get_contracted_gaussian().normalization_constant());
        auto corr = std::sqrt(3.0) * n_g;
        REQUIRE(as_double(dxy.normalization_constant()) ==
                Catch::Approx(corr).epsilon(1e-15));

        // N^AO_zz = sqrt(3!!/5!!... ) reduces to 1: all of l is on one axis.
        auto n_g_zz =
          as_double(dzz.get_contracted_gaussian().normalization_constant());
        REQUIRE(as_double(dzz.normalization_constant()) ==
                Catch::Approx(n_g_zz).epsilon(1e-15));
    }

    SECTION("normalization_constant is 1 for every s function") {
        CartesianAO s(cs.begin(), cs.end(), es.begin(), es.end(),
                      std::size_t(0), std::size_t(0), std::size_t(0), 0.0, 0.0,
                      0.0);
        // A one-primitive contraction with a unit coefficient has N^G = 1, and
        // N^AO_000 is 1, so nothing is left.
        REQUIRE(as_double(s.normalization_constant()) ==
                Catch::Approx(1.0).epsilon(1e-15));
    }

    SECTION("evaluate(point) is the raw monomial times the raw contraction") {
        // At the center the monomial vanishes for any l > 0.
        REQUIRE(as_double(dxy.evaluate(origin)) == 0.0);

        auto x = 0.3, y = -0.4, z = 0.5;
        auto r2   = x * x + y * y + z * z;
        auto corr = x * y * (1.0 * std::exp(-1.2 * r2));
        REQUIRE(as_double(dxy.evaluate(r)) ==
                Catch::Approx(corr).epsilon(1e-12));

        auto corr_zz = z * z * (std::exp(-1.0 * r2) + std::exp(-2.0 * r2));
        REQUIRE(as_double(dzz.evaluate(r)) ==
                Catch::Approx(corr_zz).epsilon(1e-12));
    }

    SECTION("evaluate(point) measures the monomial from the center") {
        CartesianAO shifted(cs.begin(), cs.end(), es.begin(), es.end(),
                            std::size_t(1), std::size_t(0), std::size_t(0), 1.0,
                            0.0, 0.0);
        // The x-power is (r_x - center_x), so it vanishes at x = 1, not x = 0.
        REQUIRE(as_double(shifted.evaluate(Point(1.0, 5.0, 5.0))) == 0.0);
        REQUIRE(as_double(shifted.evaluate(origin)) != 0.0);
    }

    SECTION("evaluate(point) interoperates across derived types") {
        cartesian_ao_view v(dxy);
        REQUIRE(as_double(dxy.evaluate(r)) == as_double(v.evaluate(r)));
    }

    SECTION("evaluate(point set)") {
        PointSet pts{origin, r};
        auto rv = dxy.evaluate(pts);
        REQUIRE(rv.size() == 2);
        REQUIRE(as_double(rv[0]) == as_double(dxy.evaluate(origin)));
        REQUIRE(as_double(rv[1]) == as_double(dxy.evaluate(r)));
    }

    SECTION("normalized_evaluate(point)") {
        // N^AO_ijk * monomial * (the contraction's own normalized value)
        auto corr =
          std::sqrt(3.0) * 0.3 * -0.4 *
          as_double(dxy.get_contracted_gaussian().normalized_evaluate(r));
        REQUIRE(as_double(dxy.normalized_evaluate(r)) ==
                Catch::Approx(corr).epsilon(1e-12));
    }

    SECTION("normalized_evaluate is not normalization_constant * evaluate") {
        // Each primitive's N^chi sits inside the contraction sum, so the naive
        // composition is wrong for a contraction of more than one primitive.
        auto naive =
          as_double(dzz.normalization_constant()) * as_double(dzz.evaluate(r));
        REQUIRE(as_double(dzz.normalized_evaluate(r)) !=
                Catch::Approx(naive).epsilon(1e-12));
    }

    SECTION("normalized_evaluate(point set)") {
        PointSet pts{origin, r};
        auto rv = dxy.normalized_evaluate(pts);
        REQUIRE(rv.size() == 2);
        REQUIRE(as_double(rv[0]) == as_double(dxy.normalized_evaluate(origin)));
        REQUIRE(as_double(rv[1]) == as_double(dxy.normalized_evaluate(r)));
    }

    SECTION("operator==/operator!=") {
        CartesianAO same(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(1), std::size_t(1), std::size_t(0), 0.0,
                         0.0, 0.0);
        REQUIRE(dxy == same);
        REQUIRE_FALSE(dxy != same);

        // Same contraction, different monomial.
        CartesianAO dxz(cs.begin(), cs.end(), es.begin(), es.end(),
                        std::size_t(1), std::size_t(0), std::size_t(1), 0.0,
                        0.0, 0.0);
        REQUIRE(dxy != dxz);

        // Same monomial, different contraction.
        std::vector<double> other_es{9.9};
        CartesianAO diff_exp(cs.begin(), cs.end(), other_es.begin(),
                             other_es.end(), std::size_t(1), std::size_t(1),
                             std::size_t(0), 0.0, 0.0, 0.0);
        REQUIRE(dxy != diff_exp);

        // Same everything but the center.
        CartesianAO moved(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(1), std::size_t(1), std::size_t(0), 1.0,
                          0.0, 0.0);
        REQUIRE(dxy != moved);

        CartesianAO defaulted;
        REQUIRE(defaulted != dxy);
    }

    SECTION("operator==/operator!= interoperate across derived types") {
        cartesian_ao_view v(dxy);
        const_cartesian_ao_view cv(dxy);
        REQUIRE(dxy == v);
        REQUIRE(v == dxy);
        REQUIRE(dxy == cv);
        REQUIRE(cv == v);
    }
}
