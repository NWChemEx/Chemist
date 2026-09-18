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

// PrimitiveCommon implements the API shared by Primitive and PrimitiveView.
// It has no state of its own to construct, so these tests drive it through
// Primitive --- the simplest concrete derived class --- and pull in
// PrimitiveView only where a property genuinely spans two different derived
// types (interoperability, and the const-qualification which gates the
// setters). Primitive's own ctors, assignment, swap, and load; and
// PrimitiveView's own ctors, assignment, aliasing, and conversions, are each
// tested in their own namesake file.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/primitive_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <cmath>
#include <sstream>
#include <utility>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Avoids relying on M_PI, which is not standard C++ and is not always
/// defined under strict conformance flags.
constexpr double pi_ = 3.14159265358979323846;

/** @brief True when T::set_coefficient(0.0) and T::set_l(0) compile.
 *
 *  test_chemist::has_setters_v probes set_x, which is Point's setter, not
 *  Primitive's, so it cannot be reused here.
 */
///@{
template<typename T, typename = void>
struct has_coefficient_setter : std::false_type {};

template<typename T>
struct has_coefficient_setter<
  T, std::void_t<decltype(std::declval<T&>().set_coefficient(0.0))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_coefficient_setter_v =
  has_coefficient_setter<T>::value;

template<typename T, typename = void>
struct has_l_setter : std::false_type {};

template<typename T>
struct has_l_setter<T, std::void_t<decltype(std::declval<T&>().set_l(0))>>
  : std::true_type {};

template<typename T>
inline constexpr bool has_l_setter_v = has_l_setter<T>::value;
///@}

} // namespace

TEST_CASE("experimental::PrimitiveCommon") {
    Point origin(0.0, 0.0, 0.0);
    Point r0(1.0, 0.0, 0.0);

    // c = 2.0, zeta = 1.0, l = 0, centered at the origin
    Primitive s(2.0, 1.0, std::size_t(0), 0.0, 0.0, 0.0);

    SECTION("get_coefficient/get_exponent/get_l") {
        REQUIRE(as_double(s.get_coefficient()) == 2.0);
        REQUIRE(as_double(s.get_exponent()) == 1.0);
        REQUIRE(s.get_l() == 0);

        // The same values are visible through a read-only reference
        REQUIRE(as_double(std::as_const(s).get_coefficient()) == 2.0);
        REQUIRE(as_double(std::as_const(s).get_exponent()) == 1.0);
        REQUIRE(std::as_const(s).get_l() == 0);
    }

    SECTION("get_center") {
        REQUIRE(s.get_center() == origin);
        REQUIRE(std::as_const(s).get_center() == origin);
    }

    SECTION("set_coefficient/set_exponent/set_l") {
        s.set_coefficient(4.0);
        s.set_exponent(5.0);
        s.set_l(2ul);
        REQUIRE(as_double(s.get_coefficient()) == 4.0);
        REQUIRE(as_double(s.get_exponent()) == 5.0);
        REQUIRE(s.get_l() == 2);

        // Setting to a different concrete type is an error, not a silent
        // conversion.
        REQUIRE_THROWS_AS(s.set_coefficient(1.0f), std::runtime_error);
    }

    SECTION("set_center") {
        s.set_center(Point(9.0, 0.0, 0.0));
        REQUIRE(s.get_center() == Point(9.0, 0.0, 0.0));

        // set_center writes the new coordinates into the center *this
        // already owns/aliases; it does not rebind *this to r0.
        Point r0(1.0, 1.0, 1.0);
        s.set_center(r0);
        r0.set_x(42.0);
        REQUIRE(s.get_center() == Point(1.0, 1.0, 1.0));
    }

    SECTION("evaluate(point)") {
        // At the center the displacement is zero, so evaluate is just c
        REQUIRE(as_double(s.evaluate(origin)) == 2.0);

        // c * exp(-zeta * |r - r0|^2), with |origin - r0|^2 == 1
        auto corr = 2.0 * std::exp(-1.0);
        REQUIRE(as_double(s.evaluate(r0)) ==
                Catch::Approx(corr).epsilon(1e-12));
    }

    SECTION("evaluate(point) interoperates across derived types") {
        primitive_view sv(s);
        REQUIRE(as_double(s.evaluate(origin)) ==
                as_double(sv.evaluate(origin)));
    }

    SECTION("evaluate(point set)") {
        PointSet pts{origin, r0};
        auto rv = s.evaluate(pts);
        REQUIRE(rv.size() == 2);
        REQUIRE(as_double(rv[0]) == 2.0);
        auto corr = 2.0 * std::exp(-1.0);
        REQUIRE(as_double(rv[1]) == Catch::Approx(corr).epsilon(1e-12));
    }

    SECTION("normalization_constant") {
        // N^chi(zeta=1; l=0) = (2*1/pi)^{3/4}
        auto corr = std::pow(2.0 / pi_, 0.75);
        REQUIRE(as_double(s.normalization_constant()) ==
                Catch::Approx(corr).epsilon(1e-12));
    }

    SECTION("normalization_constant depends on l") {
        Primitive p_only(2.0, 1.0, std::size_t(0), 0.0, 0.0, 0.0);
        Primitive d_only(2.0, 1.0, std::size_t(1), 0.0, 0.0, 0.0);
        REQUIRE(as_double(p_only.normalization_constant()) !=
                as_double(d_only.normalization_constant()));
    }

    SECTION("normalized_evaluate(point)") {
        auto n_chi = as_double(s.normalization_constant());
        auto value = as_double(s.evaluate(r0));
        REQUIRE(as_double(s.normalized_evaluate(r0)) ==
                Catch::Approx(n_chi * value).epsilon(1e-12));
    }

    SECTION("normalized_evaluate(point set)") {
        PointSet pts{origin, r0};
        auto n_chi = as_double(s.normalization_constant());
        auto rv    = s.normalized_evaluate(pts);
        REQUIRE(rv.size() == 2);
        REQUIRE(
          as_double(rv[0]) ==
          Catch::Approx(n_chi * as_double(s.evaluate(origin))).epsilon(1e-12));
        REQUIRE(
          as_double(rv[1]) ==
          Catch::Approx(n_chi * as_double(s.evaluate(r0))).epsilon(1e-12));
    }

    SECTION("operator==/operator!=") {
        Primitive defaulted;
        REQUIRE(defaulted != s);
        REQUIRE_FALSE(defaulted == s);

        Primitive same(2.0, 1.0, std::size_t(0), 0.0, 0.0, 0.0);
        REQUIRE(s == same);

        Primitive diff_l(2.0, 1.0, std::size_t(1), 0.0, 0.0, 0.0);
        REQUIRE(s != diff_l);
    }

    SECTION("operator==/operator!= interoperate across derived types") {
        primitive_view sv(s);
        REQUIRE(s == sv);
        REQUIRE(sv == s);
    }

    SECTION("save") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(s);
        }
        Primitive deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == s);
    }

    SECTION("save works the same way through a view") {
        primitive_view sv(s);
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(sv);
        }
        Primitive deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == s);
    }

    SECTION("Setters are gated by constness, not by which derived class") {
        STATIC_REQUIRE(has_coefficient_setter_v<Primitive>);
        STATIC_REQUIRE(has_coefficient_setter_v<primitive_view>);
        STATIC_REQUIRE_FALSE(has_coefficient_setter_v<const_primitive_view>);

        STATIC_REQUIRE(has_l_setter_v<Primitive>);
        STATIC_REQUIRE(has_l_setter_v<primitive_view>);
        STATIC_REQUIRE_FALSE(has_l_setter_v<const_primitive_view>);
    }
}
