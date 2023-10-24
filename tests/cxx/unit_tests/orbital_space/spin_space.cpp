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

#include "chemist/orbital_space/spin_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::orbital_space;
using namespace Catch::Matchers;

TEST_CASE("SpinSpace") {
    SECTION("CTors") {
        SpinSpace s;
        REQUIRE(s.size() == 2);
    }

    SECTION("Accessors") {
        SECTION("Total Spin") {
            SpinSpace s;
            REQUIRE_THAT(s.TSpin(), WithinAbs(0.5, 0.00001));   
        }

        SECTION("Spin Types") {
            SpinSpace s;
            REQUIRE(s.SpinType(0) == "alpha");
            REQUIRE(s.SpinType(1) == "beta");
        }
    }

    SECTION("Comparison") {
        SpinSpace s1;
        SpinSpace s2;
        REQUIRE(s1 == s2);
    }
}