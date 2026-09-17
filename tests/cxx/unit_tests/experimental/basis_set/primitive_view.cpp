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

// Only what PrimitiveView itself implements is tested here: its ctors
// (including the implicit conversions), assignment (which, unlike a normal
// copy, writes through the alias), swap, as_primitive, and the aliasing
// behavior of its own coefficient_/exponent_/l_/center_. Everything
// PrimitiveView inherits from PrimitiveCommon --- get_/set_ accessors,
// evaluate, normalized_evaluate, the comparison operators, save, and the
// SFINAE which removes the setters on a read-only view --- is tested once,
// generically, in primitive_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/primitive_view.hpp>
#include <utility>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/** @brief Takes a read-only view by value.
 *
 *  Used to prove that the Primitive -> PrimitiveView and mutable ->
 *  read-only conversions are implicit; if either were explicit, calling this
 *  with a Primitive or a mutable view would not compile.
 */
double coefficient_of(const_primitive_view p) {
    return as_double(p.get_coefficient());
}

} // namespace

TEST_CASE("experimental::PrimitiveView") {
    Primitive s(2.0, 1.0, std::size_t(1), 1.0, 2.0, 3.0);
    Primitive other(9.0, 9.0, std::size_t(2), 9.0, 9.0, 9.0);

    primitive_view sv(s);
    const_primitive_view csv(other);

    SECTION("Ctors and assignment") {
        SECTION("From a Primitive") {
            // *this aliases s's state through its own coefficient_/exponent_/
            // l_/center_, rather than copying it.
            REQUIRE(as_double(sv.get_coefficient()) == 2.0);
            REQUIRE(as_double(sv.get_exponent()) == 1.0);
            REQUIRE(sv.get_l() == 1);
            REQUIRE(sv.get_center() == s.get_center());

            s.set_coefficient(42.0);
            REQUIRE(as_double(sv.get_coefficient()) == 42.0);
        }

        SECTION("From a Primitive, implicitly") {
            // Would not compile if the ctor were explicit
            REQUIRE(coefficient_of(s) == 2.0);
        }

        SECTION("From already-aliased state") {
            using coefficient_type = Primitive::coefficient_type;
            coefficient_type c(2.0), e(1.0);
            Primitive::angular_momentum_type l = 1;
            Point center(1.0, 2.0, 3.0);
            primitive_view from_parts(c.as_view(), e.as_view(), l, center);
            REQUIRE(from_parts == s);

            // The view aliases the loose state, it did not copy it
            from_parts.set_l(7ul);
            REQUIRE(l == 7);
        }

        SECTION("Mutable to read-only, implicitly") {
            // Would not compile if the conversion were explicit
            REQUIRE(coefficient_of(sv) == 2.0);

            const_primitive_view as_const(sv);
            REQUIRE(as_const == sv);
        }

        SECTION("Copy ctor is shallow") {
            primitive_view copy(sv);
            REQUIRE(copy == sv);

            // Writing through the copy writes into the same Primitive
            copy.set_l(4ul);
            REQUIRE(s.get_l() == 4);
        }

        SECTION("Move ctor") {
            primitive_view moved(std::move(sv));
            REQUIRE(moved == s);
            moved.set_l(4ul);
            REQUIRE(s.get_l() == 4);
        }

        SECTION("Copy assignment writes through") {
            // Unlike the copy ctor, assignment does NOT rebind *this; it
            // overwrites whatever *this already aliases.
            primitive_view other_view(other);

            sv = other_view;

            // s, not sv's binding, is what changed
            REQUIRE(s == other);
        }

        SECTION("Assignment from a Primitive writes through") {
            sv = other;
            REQUIRE(s == other);
        }

        SECTION("Assignment from a read-only view writes through") {
            sv = csv;
            REQUIRE(s == other);
        }
    }

    SECTION("swap rebinds") {
        primitive_view other_view(other);

        sv.swap(other_view);

        // The views traded places; neither Primitive changed
        REQUIRE(sv == other);
        REQUIRE(other_view ==
                Primitive(2.0, 1.0, std::size_t(1), 1.0, 2.0, 3.0));
        REQUIRE(s == Primitive(2.0, 1.0, std::size_t(1), 1.0, 2.0, 3.0));
        REQUIRE(other == Primitive(9.0, 9.0, std::size_t(2), 9.0, 9.0, 9.0));
    }

    SECTION("as_primitive is a deep copy") {
        auto copy = sv.as_primitive();
        REQUIRE(copy == s);

        copy.set_l(4ul);
        REQUIRE(s.get_l() == 1);
    }

    SECTION("Writing through the view mutates the aliased Primitive") {
        sv.set_coefficient(4.0);
        sv.set_exponent(5.0);
        sv.set_l(6ul);
        REQUIRE(as_double(s.get_coefficient()) == 4.0);
        REQUIRE(as_double(s.get_exponent()) == 5.0);
        REQUIRE(s.get_l() == 6);
    }
}
