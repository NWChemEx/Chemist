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
#include <chemist/grid/grid_class.hpp>
#include <utility>

using namespace chemist;

TEST_CASE("Grid") {
    Grid defaulted;

    std::vector<GridPoint> points;
    points.push_back(GridPoint());
    points.push_back(GridPoint(0.1, 1.2, 2.3, 3.4));
    Grid range(points.begin(), points.end());

    SECTION("Ctors and assignment") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Range") {
            REQUIRE(range.size() == 2);
            REQUIRE(range.at(0) == points.at(0));
            REQUIRE(range.at(1) == points.at(1));
        }
        test_chemist::test_copy_and_move(defaulted, range);
    }

    SECTION("at_()") {
        REQUIRE(range.at(0) == points.at(0));
        REQUIRE(range.at(1) == points.at(1));
    }

    SECTION("ato_() const") {
        REQUIRE(std::as_const(range).at(0) == points.at(0));
        REQUIRE(std::as_const(range).at(1) == points.at(1));
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(range.size() == 2);
    }
}
