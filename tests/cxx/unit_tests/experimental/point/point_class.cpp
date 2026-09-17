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

// Only what Point itself implements is tested here: its ctors, assignment,
// swap, and load, plus the free functions operator- and operator<< (which
// live alongside it in point_class.hpp). Everything Point inherits from
// PointCommon --- get_/set_ accessors, inner_product, magnitude, the
// comparison operators, and save --- is tested once in point_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <sstream>
#include <utility>

using namespace chemist::experimental;

TEST_CASE("experimental::Point") {
    Point defaulted;
    Point p(1.0, 2.0, 3.0);

    // 3-4-5 in the xy-plane, so the magnitude is exactly representable
    Point p345(3.0, 4.0, 0.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.get_x() == 0.0);
            REQUIRE(defaulted.get_y() == 0.0);
            REQUIRE(defaulted.get_z() == 0.0);
        }

        SECTION("x, y, z") {
            REQUIRE(p.get_x() == 1.0);
            REQUIRE(p.get_y() == 2.0);
            REQUIRE(p.get_z() == 3.0);

            // The concrete type is deduced from the arguments, not fixed
            Point as_floats(1.0f, 2.0f, 3.0f);
            REQUIRE(as_floats.get_x() == 1.0f);

            // ... and a float-valued point is NOT a double-valued one
            REQUIRE(as_floats != p);
        }

        SECTION("Type-erased x, y, z") {
            using coord_type = Point::coord_type;
            Point erased(coord_type(1.0), coord_type(2.0), coord_type(3.0));
            REQUIRE(erased == p);
        }

        test_chemist::test_copy_and_move(defaulted, p, p345);
    }

    SECTION("swap") {
        Point lhs(p);
        Point rhs(p345);
        lhs.swap(rhs);
        REQUIRE(lhs == p345);
        REQUIRE(rhs == p);
    }

    SECTION("operator-") {
        auto diff = p - defaulted;
        REQUIRE(diff == p);

        auto zero = p - p;
        REQUIRE(zero == defaulted);

        // (3,4,0) - (1,2,3) == (2,2,-3)
        REQUIRE((p345 - p) == Point(2.0, 2.0, -3.0));
    }

    SECTION("operator<<") {
        std::stringstream ss;
        ss << p;
        auto corr = "x : 1,\ny : 2,\nz : 3";
        REQUIRE(ss.str() == corr);

        // The old chemist::Point printed all significant figures; so does
        // *this, even though wtf's own to_string would truncate to six.
        std::stringstream ss2;
        ss2 << Point(1.0 / 3.0, 0.0, 0.0);
        REQUIRE(ss2.str().find("0.333333333333333") == 4);
    }

    SECTION("load") {
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

    SECTION("load restores the concrete type") {
        Point as_floats(1.0f, 2.0f, 3.0f);
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(as_floats);
        }
        // *this held doubles before the load and floats after it
        Point deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == as_floats);
        REQUIRE(deserialized != Point(1.0, 2.0, 3.0));
    }
}
