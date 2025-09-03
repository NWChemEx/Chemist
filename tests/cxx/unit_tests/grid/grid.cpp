/*
 * Copyright 2024 NWChemEx-Project
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

#include "../catch.hpp"
#include <chemist/grid/grid.hpp>

TEST_CASE("Grid") {
    chemist::Grid grid1;

    SECTION("vector_t") {
        STATIC_REQUIRE(
          std::is_same_v<chemist::Grid::vector_t, std::vector<double>>);
    }
    SECTION("xs") { REQUIRE(grid1.m_xs.size() == 0); }
    SECTION("ys") { REQUIRE(grid1.m_ys.size() == 0); }
    SECTION("zs") { REQUIRE(grid1.m_zs.size() == 0); }
    SECTION("ws") { REQUIRE(grid1.m_ws.size() == 0); }
}
