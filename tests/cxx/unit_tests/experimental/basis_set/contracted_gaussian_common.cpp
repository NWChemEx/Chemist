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

// ContractedGaussianCommon implements the API shared by ContractedGaussian
// and ContractedGaussianView. It has no state of its own to construct, so
// these tests drive it through ContractedGaussian --- the simplest concrete
// derived class --- and pull in ContractedGaussianView only where a property
// genuinely spans two different derived types. ContractedGaussian's own
// ctors, assignment, swap, and load are tested in contracted_gaussian_class.
// cpp; ContractedGaussianView's own ctors, assignment, aliasing, and
// conversions are tested in contracted_gaussian_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <cmath>
#include <sstream>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/** @brief True when T::set_l(0)/T::set_center(Point{}) compile.
 *
 *  The experimental setters are removed by SFINAE when the object aliases
 *  read-only state, rather than being present and throwing. This trait is
 *  what lets a test assert that, since "does not compile" can not be a
 *  runtime assertion. Defined locally rather than added to the shared
 *  experimental_test_helpers.hpp, following primitive_common.cpp's
 *  precedent.
 */
///@{
template<typename T, typename = void>
struct has_l_setter : std::false_type {};

template<typename T>
struct has_l_setter<T, std::void_t<decltype(std::declval<T&>().set_l(0))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_l_setter_v = has_l_setter<T>::value;

template<typename T, typename = void>
struct has_center_setter : std::false_type {};

template<typename T>
struct has_center_setter<
  T, std::void_t<decltype(std::declval<T&>().set_center(Point{}))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_center_setter_v = has_center_setter<T>::value;
///@}

} // namespace

TEST_CASE("experimental::ContractedGaussianCommon") {
    Point origin(0.0, 0.0, 0.0);
    Point r0(1.0, 0.0, 0.0);

    // Two-primitive s-shell (l = 0), d1 = d2 = 1.0, zeta1 = 1.0, zeta2 = 2.0,
    // centered at the origin. See the worked N^G example below.
    std::vector<double> cs{1.0, 1.0};
    std::vector<double> es{1.0, 2.0};
    ContractedGaussian s(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(0), 0.0, 0.0, 0.0);

    SECTION("get_l/get_center") {
        REQUIRE(s.get_l() == 0);
        REQUIRE(s.get_center() == origin);
        REQUIRE(std::as_const(s).get_l() == 0);
        REQUIRE(std::as_const(s).get_center() == origin);
    }

    SECTION("set_l/set_center") {
        s.set_l(2ul);
        REQUIRE(s.get_l() == 2);

        s.set_center(r0);
        REQUIRE(s.get_center() == r0);

        // set_center writes the new coordinates into the center *this
        // already owns/aliases; it does not rebind *this to r0.
        Point moved(9.0, 9.0, 9.0);
        s.set_center(moved);
        moved.set_x(42.0);
        REQUIRE(s.get_center() == Point(9.0, 9.0, 9.0));
    }

    SECTION("get_coefficient_buffer/get_exponent_buffer") {
        REQUIRE(s.get_coefficient_buffer().size() == 2);
        REQUIRE(s.get_exponent_buffer().size() == 2);
        REQUIRE(as_double(std::as_const(s).get_coefficient_buffer().at(0)) ==
                1.0);
        REQUIRE(as_double(std::as_const(s).get_exponent_buffer().at(1)) == 2.0);
    }

    SECTION("Indexing yields Primitives sharing l and center") {
        REQUIRE(s[0].get_center() == s.get_center());
        REQUIRE(s[1].get_center() == s.get_center());
        REQUIRE(s[0].get_l() == s.get_l());
        REQUIRE(s[1].get_l() == s.get_l());

        // Every index aliases the SAME shared l and center: mutating one
        // primitive's l is visible through every other index too.
        s[0].set_l(3ul);
        REQUIRE(s[1].get_l() == 3);
        REQUIRE(s.get_l() == 3);

        s[0].set_center(Point(5.0, 5.0, 5.0));
        REQUIRE(s[1].get_center() == Point(5.0, 5.0, 5.0));
        REQUIRE(s.get_center() == Point(5.0, 5.0, 5.0));
    }

    SECTION("evaluate(point)") {
        // Raw, unnormalized sum: c1*exp(-zeta1*r^2) + c2*exp(-zeta2*r^2)
        REQUIRE(as_double(s.evaluate(origin)) == 2.0);

        auto corr = 1.0 * std::exp(-1.0) + 1.0 * std::exp(-2.0);
        REQUIRE(as_double(s.evaluate(r0)) ==
                Catch::Approx(corr).epsilon(1e-12));
    }

    SECTION("evaluate(point) on an empty contraction returns 0.0") {
        ContractedGaussian empty;
        REQUIRE(as_double(empty.evaluate(origin)) == 0.0);
    }

    SECTION("evaluate(point) interoperates across derived types") {
        contracted_gaussian_view sv(s);
        REQUIRE(as_double(s.evaluate(origin)) ==
                as_double(sv.evaluate(origin)));
    }

    SECTION("evaluate(point set)") {
        PointSet pts{origin, r0};
        auto rv = s.evaluate(pts);
        REQUIRE(rv.size() == 2);
        REQUIRE(as_double(rv[0]) == as_double(s.evaluate(origin)));
        REQUIRE(as_double(rv[1]) == as_double(s.evaluate(r0)));
    }

    SECTION("normalization_constant (worked N^G example)") {
        // S_pp = 1 for both primitives (identity). Cross term:
        //   2*sqrt(zeta1*zeta2) = 2*sqrt(2)
        //   zeta1 + zeta2       = 3
        //   S_12 = S_21 = (2*sqrt(2)/3)^1.5
        // sum = d1^2*S_11 + d2^2*S_22 + 2*d1*d2*S_12
        // N^G = sum^-0.5
        auto s_12 = std::pow(2.0 * std::sqrt(2.0) / 3.0, 1.5);
        auto sum  = 1.0 + 1.0 + 2.0 * s_12;
        auto n_g  = std::pow(sum, -0.5);
        REQUIRE(as_double(s.normalization_constant()) ==
                Catch::Approx(n_g).epsilon(1e-9));
        REQUIRE(as_double(s.normalization_constant()) ==
                Catch::Approx(0.5109158).epsilon(1e-6));
    }

    SECTION("normalization_constant depends on l") {
        ContractedGaussian p_shell(cs.begin(), cs.end(), es.begin(), es.end(),
                                   std::size_t(0), 0.0, 0.0, 0.0);
        ContractedGaussian d_shell(cs.begin(), cs.end(), es.begin(), es.end(),
                                   std::size_t(1), 0.0, 0.0, 0.0);
        REQUIRE(as_double(p_shell.normalization_constant()) !=
                as_double(d_shell.normalization_constant()));
    }

    SECTION("normalized_evaluate(point)") {
        // NOT normalization_constant() * evaluate(r): each primitive's own
        // N^chi must be applied before summing, and N^G applied once after.
        auto n_g  = as_double(s.normalization_constant());
        auto corr = n_g * (as_double(s[0].normalized_evaluate(r0)) +
                           as_double(s[1].normalized_evaluate(r0)));
        REQUIRE(as_double(s.normalized_evaluate(r0)) ==
                Catch::Approx(corr).epsilon(1e-12));

        // Confirm it genuinely differs from the naive composition.
        auto naive = n_g * as_double(s.evaluate(r0));
        REQUIRE(as_double(s.normalized_evaluate(r0)) !=
                Catch::Approx(naive).epsilon(1e-12));
    }

    SECTION("normalized_evaluate(point) on an empty contraction returns 0.0") {
        ContractedGaussian empty;
        REQUIRE(as_double(empty.normalized_evaluate(origin)) == 0.0);
    }

    SECTION("normalized_evaluate(point set)") {
        PointSet pts{origin, r0};
        auto rv = s.normalized_evaluate(pts);
        REQUIRE(rv.size() == 2);
        REQUIRE(as_double(rv[0]) == as_double(s.normalized_evaluate(origin)));
        REQUIRE(as_double(rv[1]) == as_double(s.normalized_evaluate(r0)));
    }

    SECTION("operator==/operator!=") {
        ContractedGaussian defaulted;
        REQUIRE(defaulted != s);
        REQUIRE_FALSE(defaulted == s);

        ContractedGaussian same(cs.begin(), cs.end(), es.begin(), es.end(),
                                std::size_t(0), 0.0, 0.0, 0.0);
        REQUIRE(s == same);

        ContractedGaussian diff_l(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(1), 0.0, 0.0, 0.0);
        REQUIRE(s != diff_l);

        std::vector<double> shorter_cs{1.0};
        std::vector<double> shorter_es{1.0};
        ContractedGaussian diff_size(shorter_cs.begin(), shorter_cs.end(),
                                     shorter_es.begin(), shorter_es.end(),
                                     std::size_t(0), 0.0, 0.0, 0.0);
        REQUIRE(s != diff_size);
    }

    SECTION("operator==/operator!= interoperate across derived types") {
        contracted_gaussian_view sv(s);
        REQUIRE(s == sv);
        REQUIRE(sv == s);
    }

    SECTION("save") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(s);
        }
        ContractedGaussian deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == s);
    }

    SECTION("save works the same way through a view") {
        contracted_gaussian_view sv(s);
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(sv);
        }
        ContractedGaussian deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == s);
    }

    SECTION("Setters are gated by constness, not by which derived class") {
        STATIC_REQUIRE(has_l_setter_v<ContractedGaussian>);
        STATIC_REQUIRE(has_l_setter_v<contracted_gaussian_view>);
        STATIC_REQUIRE_FALSE(has_l_setter_v<const_contracted_gaussian_view>);

        STATIC_REQUIRE(has_center_setter_v<ContractedGaussian>);
        STATIC_REQUIRE(has_center_setter_v<contracted_gaussian_view>);
        STATIC_REQUIRE_FALSE(
          has_center_setter_v<const_contracted_gaussian_view>);
    }
}
