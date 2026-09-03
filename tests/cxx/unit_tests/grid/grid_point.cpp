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

TEST_CASE("GridPoint") {
    GridPoint defaulted;
    GridPoint origin(1.2, 0.0, 0.0, 0.0);
    GridPoint non_origin(2.3, 3.4, 4.5, 5.6);

    SECTION("Ctors and assignment") {
        SECTION("Default Ctor") {
            REQUIRE(defaulted.get_weight() == 0.0);
            REQUIRE(defaulted.get_x() == 0.0);
            REQUIRE(defaulted.get_y() == 0.0);
            REQUIRE(defaulted.get_z() == 0.0);
        }

        SECTION("weight, x, y, and z") {
            REQUIRE(origin.get_weight() == 1.2);
            REQUIRE(origin.get_x() == 0.0);
            REQUIRE(origin.get_y() == 0.0);
            REQUIRE(origin.get_z() == 0.0);

            REQUIRE(non_origin.get_weight() == 2.3);
            REQUIRE(non_origin.get_x() == 3.4);
            REQUIRE(non_origin.get_y() == 4.5);
            REQUIRE(non_origin.get_z() == 5.6);
        }

        test_chemist::test_copy_and_move(defaulted, origin, non_origin);
    }

    SECTION("weight()") {
        REQUIRE(defaulted.get_weight() == 0.0);
        REQUIRE(origin.get_weight() == 1.2);
        REQUIRE(non_origin.get_weight() == 2.3);

        defaulted.set_weight(42.0);
        REQUIRE(defaulted.get_weight() == 42.0);
    }

    SECTION("weight() const") {
        REQUIRE(std::as_const(defaulted).get_weight() == 0.0);
        REQUIRE(std::as_const(origin).get_weight() == 1.2);
        REQUIRE(std::as_const(non_origin).get_weight() == 2.3);
    }

    SECTION("x()/y()/z()") {
        REQUIRE(defaulted.get_x() == 0.0);
        REQUIRE(defaulted.get_y() == 0.0);
        REQUIRE(defaulted.get_z() == 0.0);

        REQUIRE(non_origin.get_x() == 3.4);
        REQUIRE(non_origin.get_y() == 4.5);
        REQUIRE(non_origin.get_z() == 5.6);

        defaulted.set_x(1.0);
        defaulted.set_y(2.0);
        defaulted.set_z(3.0);
        REQUIRE(defaulted.get_x() == 1.0);
        REQUIRE(defaulted.get_y() == 2.0);
        REQUIRE(defaulted.get_z() == 3.0);
    }

    SECTION("x()/y()/z() const") {
        REQUIRE(std::as_const(defaulted).get_x() == 0.0);
        REQUIRE(std::as_const(defaulted).get_y() == 0.0);
        REQUIRE(std::as_const(defaulted).get_z() == 0.0);

        REQUIRE(std::as_const(non_origin).get_x() == 3.4);
        REQUIRE(std::as_const(non_origin).get_y() == 4.5);
        REQUIRE(std::as_const(non_origin).get_z() == 5.6);
    }

    SECTION("operator==") {
        // Default == Default
        REQUIRE(defaulted == GridPoint());

        // Default == zero-initialized
        REQUIRE(defaulted == GridPoint(0.0, 0.0, 0.0, 0.0));

        // Different weights
        REQUIRE_FALSE(origin == GridPoint(2.3, 0.0, 0.0, 0.0));

        // Different points
        REQUIRE_FALSE(non_origin == GridPoint(2.3, 0.0, 0.0, 0.0));
    }

    SECTION("operator!=") {
        // N.b., just negates operator== so spot checking is fine

        // Check on objects with equal state
        REQUIRE_FALSE(defaulted != GridPoint());

        // Check on objects with different state
        REQUIRE(origin != non_origin);
    }
}
