/*
 * Copyright 2023 NWChemEx-Project
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

#include <catch2/catch.hpp>
#include <chemist/basis_set2/detail_/compute_n_aos.hpp>

using chemist::ShellType;
using namespace chemist::basis_set::detail_;

TEST_CASE("compute_n_aos") {
    ShellType purity;
    std::size_t l0 = 0, l1 = 1, l2 = 2, l3 = 3;
    SECTION("Spherical") {
        purity = ShellType::pure;
        SECTION("L = 0") { REQUIRE(compute_n_aos(l0, purity) == 1); }
        SECTION("L = 1") { REQUIRE(compute_n_aos(l1, purity) == 3); }
        SECTION("L = 2") { REQUIRE(compute_n_aos(l2, purity) == 5); }
        SECTION("L = 3") { REQUIRE(compute_n_aos(l3, purity) == 7); }
    }
    SECTION("Cartesian") {
        purity = ShellType::cartesian;
        SECTION("L = 0") { REQUIRE(compute_n_aos(l0, purity) == 1); }
        SECTION("L = 1") { REQUIRE(compute_n_aos(l1, purity) == 3); }
        SECTION("L = 2") { REQUIRE(compute_n_aos(l2, purity) == 6); }
        SECTION("L = 3") { REQUIRE(compute_n_aos(l3, purity) == 10); }
    }
}