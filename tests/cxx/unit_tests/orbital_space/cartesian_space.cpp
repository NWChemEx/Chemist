/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/orbital_space/cartesian_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::orbital_space;
using namespace Catch::Matchers;

TEST_CASE("CartesianSpace") {
    SECTION("CTors") {
        SECTION("Axes not set") {
            auto s = CartesianSpace(3);
            REQUIRE(s.size() == 3);
        }
    }

    SECTION("Accessors") {
        SECTION("Axis Types") {
            std::vector<int> val{2, 0, 1};
            auto s = CartesianSpace(3, val);
            REQUIRE(s.EnumToStr(s.axis_arr[0]) == "z");
            REQUIRE(s.EnumToStr(s.axis_arr[1]) == "x");
            REQUIRE(s.EnumToStr(s.axis_arr[2]) == "y");
        }
    }

    SECTION("Comparison") {
        auto s2_1 = CartesianSpace(2);
        auto s2_2 = CartesianSpace(2);
        auto s3   = CartesianSpace(3);
        REQUIRE(s2_1 == s2_2);
        REQUIRE_FALSE(s2_1 == s3);
    }
}