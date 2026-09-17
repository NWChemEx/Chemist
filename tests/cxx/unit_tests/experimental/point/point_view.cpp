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

// Only what PointView itself implements is tested here: its ctors
// (including the implicit conversions), assignment (which, unlike a normal
// copy, writes through the alias), swap, as_point, and the aliasing behavior
// of its own coord_. Everything PointView inherits from PointCommon ---
// get_/set_ accessors, inner_product, magnitude, the comparison operators,
// save, and the SFINAE which removes the setters on a read-only view --- is
// tested once, generically, in point_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/point/point_view.hpp>
#include <utility>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/** @brief Takes a read-only view by value.
 *
 *  Used to prove that the Point -> PointView and mutable -> read-only
 *  conversions are implicit; if either were explicit, calling this with a
 *  Point or a mutable view would not compile.
 */
double x_of(const_point_view p) { return as_double(p.get_x()); }

} // namespace

TEST_CASE("experimental::PointView") {
    Point p(1.0, 2.0, 3.0);
    Point p345(3.0, 4.0, 0.0);

    point_view pv(p);
    const_point_view cpv(p345);

    SECTION("Ctors and assignment") {
        SECTION("From a Point") {
            // *this aliases p's state through its own coord_, rather than
            // copying it; get_x/y/z are PointCommon's, but whether they see
            // p's state at all is entirely down to PointView's coord_.
            REQUIRE(pv.get_x() == 1.0);
            REQUIRE(pv.get_y() == 2.0);
            REQUIRE(pv.get_z() == 3.0);

            p.set_x(42.0);
            REQUIRE(pv.get_x() == 42.0);
        }

        SECTION("From a Point, implicitly") {
            // Would not compile if the ctor were explicit
            REQUIRE(x_of(p) == 1.0);
        }

        SECTION("From three coordinate views") {
            using coord_type = Point::coord_type;
            coord_type x(1.0), y(2.0), z(3.0);
            point_view from_coords(x.as_view(), y.as_view(), z.as_view());
            REQUIRE(from_coords == p);

            // The view aliases the loose coordinates, it did not copy them
            from_coords.set_y(9.0);
            REQUIRE(as_double(y) == 9.0);
        }

        SECTION("Mutable to read-only, implicitly") {
            // Would not compile if the conversion were explicit
            REQUIRE(x_of(pv) == 1.0);

            const_point_view as_const(pv);
            REQUIRE(as_const == pv);
        }

        SECTION("Copy ctor is shallow") {
            point_view copy(pv);
            REQUIRE(copy == pv);

            // Writing through the copy writes into the same Point
            copy.set_z(7.0);
            REQUIRE(p.get_z() == 7.0);
        }

        SECTION("Move ctor") {
            point_view moved(std::move(pv));
            REQUIRE(moved == Point(1.0, 2.0, 3.0));
            moved.set_z(7.0);
            REQUIRE(p.get_z() == 7.0);
        }

        SECTION("Copy assignment writes through") {
            // Unlike the copy ctor, assignment does NOT rebind *this; it
            // overwrites whatever *this already aliases. This is PointView's
            // own operator=, not PointCommon's.
            Point other(9.0, 9.0, 9.0);
            point_view other_view(other);

            pv = other_view;

            // p, not pv's binding, is what changed
            REQUIRE(p == other);
            // ... and other was left alone
            REQUIRE(other == Point(9.0, 9.0, 9.0));
        }

        SECTION("Assignment from a Point writes through") {
            pv = p345;
            REQUIRE(p == p345);
        }

        SECTION("Assignment from a read-only view writes through") {
            pv = cpv;
            REQUIRE(p == p345);
        }
    }

    SECTION("swap rebinds") {
        Point other(9.0, 9.0, 9.0);
        point_view other_view(other);

        pv.swap(other_view);

        // The views traded places; neither Point changed
        REQUIRE(pv == other);
        REQUIRE(other_view == Point(1.0, 2.0, 3.0));
        REQUIRE(p == Point(1.0, 2.0, 3.0));
        REQUIRE(other == Point(9.0, 9.0, 9.0));
    }

    SECTION("as_point is a deep copy") {
        auto copy = pv.as_point();
        REQUIRE(copy == p);

        copy.set_x(42.0);
        REQUIRE(p.get_x() == 1.0);
    }

    SECTION("Writing through the view mutates the aliased Point") {
        // set_x/y/z are PointCommon's; what is under test here is that
        // PointView's own coord_ hands back a reference into p rather than
        // into some internal copy.
        pv.set_x(4.0);
        pv.set_y(5.0);
        pv.set_z(6.0);
        REQUIRE(p == Point(4.0, 5.0, 6.0));
    }

    SECTION("operator-") {
        // operator- is defined generically over PointCommon, so this is
        // confirming PointView can be used as either operand, not re-testing
        // the subtraction logic itself (see point_class.cpp for that).
        auto diff = cpv - pv;
        STATIC_REQUIRE(std::is_same_v<decltype(diff), Point>);
        REQUIRE(diff == Point(2.0, 2.0, -3.0));

        REQUIRE((cpv - p) == Point(2.0, 2.0, -3.0));
    }
}
