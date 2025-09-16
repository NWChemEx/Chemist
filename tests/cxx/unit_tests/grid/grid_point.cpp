/*
 * Copyright 2025 NWChemEx-Project
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

#include "../test_helpers.hpp"
#include <chemist/grid/grid_point.hpp>
#include <utility>

using namespace chemist;
using point_type = GridPoint::point_type;

TEST_CASE("GridPoint") {
    GridPoint defaulted;
    GridPoint origin(1.2, 0.0, 0.0, 0.0);
    GridPoint non_origin(2.3, point_type(3.4, 4.5, 5.6));

    SECTION("Ctors and assignment") {
        SECTION("Default Ctor") {
            REQUIRE(defaulted.weight() == 0.0);
            REQUIRE(defaulted.point() == point_type());
        }

        SECTION("weight, x, y, and z") {
            REQUIRE(origin.weight() == 1.2);
            REQUIRE(origin.point() == point_type(0.0, 0.0, 0.0));
        }

        SECTION("weight, point") {
            REQUIRE(non_origin.weight() == 2.3);
            REQUIRE(non_origin.point() == point_type(3.4, 4.5, 5.6));
        }

        test_chemist::test_copy_and_move(defaulted, origin, non_origin);
    }

    SECTION("weight()") {
        REQUIRE(defaulted.weight() == 0.0);
        REQUIRE(origin.weight() == 1.2);
        REQUIRE(non_origin.weight() == 2.3);
    }

    SECTION("weight() const") {
        REQUIRE(std::as_const(defaulted).weight() == 0.0);
        REQUIRE(std::as_const(origin).weight() == 1.2);
        REQUIRE(std::as_const(non_origin).weight() == 2.3);
    }

    SECTION("point()") {
        REQUIRE(defaulted.point() == point_type());
        REQUIRE(origin.point() == point_type(0.0, 0.0, 0.0));
        REQUIRE(non_origin.point() == point_type(3.4, 4.5, 5.6));
    }

    SECTION("point() const") {
        REQUIRE(std::as_const(defaulted).point() == point_type());
        REQUIRE(std::as_const(origin).point() == point_type(0.0, 0.0, 0.0));
        REQUIRE(std::as_const(non_origin).point() == point_type(3.4, 4.5, 5.6));
    }

    SECTION("operator==") {
        // Default == Default
        REQUIRE(defaulted == GridPoint());

        // Default == zero-initialized
        REQUIRE(defaulted == GridPoint(0.0, 0.0, 0.0, 0.0));

        // Different weights
        REQUIRE_FALSE(origin == GridPoint(2.3, point_type()));

        // Different points
        REQUIRE_FALSE(non_origin == GridPoint(2.3, point_type(0.0, 0.0, 0.0)));
    }

    SECTION("operator!=") {
        // N.b., just negates operator== so spot checking is fine

        // Check on objects with equal state
        REQUIRE_FALSE(defaulted != GridPoint());

        // Check on objects with different state
        REQUIRE(origin != non_origin);
    }
}
