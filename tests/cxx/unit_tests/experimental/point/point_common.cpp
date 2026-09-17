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

// PointCommon implements the API shared by Point and PointView. It has no
// state of its own to construct, so these tests drive it through Point ---
// the simplest concrete derived class --- and pull in PointView only where a
// property genuinely spans two different derived types (interoperability,
// and the const-qualification which gates the setters). Point's own ctors,
// assignment, swap, and load; and PointView's own ctors, assignment,
// aliasing, and conversions, are each tested in their own namesake file.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/point/point_view.hpp>
#include <sstream>
#include <utility>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::PointCommon") {
    Point origin(0.0, 0.0, 0.0);
    Point p(1.0, 2.0, 3.0);

    // 3-4-5 in the xy-plane, so the magnitude is exactly representable
    Point p345(3.0, 4.0, 0.0);

    SECTION("get_coord") {
        REQUIRE(p.get_coord(0) == 1.0);
        REQUIRE(p.get_coord(1) == 2.0);
        REQUIRE(p.get_coord(2) == 3.0);

        // This out-of-range check exercises assert_coord_in_range_, which is
        // shared by every derived class; Point is enough to cover it, so it
        // is not repeated in point_class.cpp or point_view.cpp.
        REQUIRE_THROWS_AS(p.get_coord(3), std::out_of_range);
    }

    SECTION("get_x/get_y/get_z") {
        REQUIRE(p.get_x() == 1.0);
        REQUIRE(p.get_y() == 2.0);
        REQUIRE(p.get_z() == 3.0);

        // The same values are visible through a read-only reference
        REQUIRE(std::as_const(p).get_x() == 1.0);
        REQUIRE(std::as_const(p).get_y() == 2.0);
        REQUIRE(std::as_const(p).get_z() == 3.0);
    }

    SECTION("set_coord") {
        p.set_coord(0, 4.0);
        p.set_coord(1, 5.0);
        p.set_coord(2, 6.0);
        REQUIRE(p == Point(4.0, 5.0, 6.0));

        REQUIRE_THROWS_AS(p.set_coord(3, 0.0), std::out_of_range);

        // Setting a coordinate to a different concrete type is an error, not
        // a silent conversion.
        REQUIRE_THROWS_AS(p.set_coord(0, 1.0f), std::runtime_error);
    }

    SECTION("set_x/set_y/set_z") {
        Point defaulted;
        defaulted.set_x(1.0);
        defaulted.set_y(2.0);
        defaulted.set_z(3.0);
        REQUIRE(defaulted == p);
    }

    SECTION("inner_product") {
        REQUIRE(as_double(origin.inner_product(p)) == 0.0);
        // 1*1 + 2*2 + 3*3
        REQUIRE(as_double(p.inner_product(p)) == 14.0);
        // 1*3 + 2*4 + 3*0
        REQUIRE(as_double(p.inner_product(p345)) == 11.0);

        // Mixing concrete floating-point types is an error
        Point as_floats(1.0f, 2.0f, 3.0f);
        REQUIRE_THROWS_AS(p.inner_product(as_floats), std::runtime_error);
    }

    SECTION("inner_product interoperates across derived types") {
        // inner_product is templated on the OtherDerived/OtherPoint of its
        // argument, so a Point and a PointView must be usable together; this
        // is the property that makes that template parameter meaningful.
        point_view pv(p);
        REQUIRE(as_double(p.inner_product(pv)) == 14.0);
        REQUIRE(as_double(pv.inner_product(p)) == 14.0);
    }

    SECTION("magnitude") {
        REQUIRE(as_double(origin.magnitude()) == 0.0);
        REQUIRE(as_double(p345.magnitude()) == 5.0);
    }

    SECTION("operator==/operator!=") {
        Point defaulted;
        REQUIRE(defaulted == origin);
        REQUIRE_FALSE(defaulted != origin);

        REQUIRE(p != origin);
        REQUIRE_FALSE(p == origin);

        // Value equality is per-coordinate; a single difference is enough
        REQUIRE(p != Point(1.0, 2.0, 4.0));
    }

    SECTION("operator==/operator!= interoperate across derived types") {
        point_view pv(p);
        REQUIRE(p == pv);
        REQUIRE(pv == p);
        REQUIRE(pv != p345);
        REQUIRE_FALSE(pv == p345);
    }

    SECTION("save") {
        // save() has no counterpart to read its own output back, so the
        // round trip below uses Point::load purely as an instrument; load's
        // own correctness is verified separately in point_class.cpp.
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(p);
        }
        Point deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == p);
    }

    SECTION("save works the same way through a view") {
        // save() is inherited verbatim; the only thing worth confirming here
        // is that it is actually reachable through a PointView, not that its
        // logic changes (it does not).
        point_view pv(p);
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(pv);
        }
        Point deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == p);
    }

    SECTION("Setters are gated by constness, not by which derived class") {
        // enable_if_mutable_t lives in PointCommon and keys off of the
        // PointType template parameter, so this is a property of the base,
        // not of Point or PointView individually. A concrete const-qualified
        // instantiation only exists through PointView, since Point is always
        // mutable.
        STATIC_REQUIRE(test_chemist::has_setters_v<Point>);
        STATIC_REQUIRE(test_chemist::has_setters_v<point_view>);
        STATIC_REQUIRE_FALSE(test_chemist::has_setters_v<const_point_view>);
    }
}
