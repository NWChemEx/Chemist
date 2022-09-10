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

#include "chemist/basis_set/shell_view.hpp"
#include <catch2/catch.hpp>

using namespace chemist;

TEST_CASE("ShellView : default ctor") {
    ShellView<double> s; // Basically just make sure it compiles.
}

TEST_CASE("ShellView<double> : pure()") {
    ShellView<double> s;
    SECTION("Value") { REQUIRE(s.pure() == ShellType::cartesian); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<ShellType&, decltype(s.pure())>);
    }
}

TEST_CASE("ShellView<const double> : pure()") {
    ShellView<const double> s;
    SECTION("Value") { REQUIRE(s.pure() == ShellType::cartesian); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const ShellType&, decltype(s.pure())>);
    }
}

TEST_CASE("ShellView : pure() const") {
    const ShellView<double> s;
    SECTION("Value") { REQUIRE(s.pure() == ShellType::cartesian); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const ShellType&, decltype(s.pure())>);
    }
}

TEST_CASE("ShellView<double> : l()") {
    ShellView<double> s;
    SECTION("Value") { REQUIRE(s.l() == 0); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<std::size_t&, decltype(s.l())>);
    }
}

TEST_CASE("ShellView<const double> : l()") {
    ShellView<const double> s;
    SECTION("Value") { REQUIRE(s.l() == 0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(s.l())>);
    }
}

TEST_CASE("ShellView : l() const") {
    const ShellView<double> s;
    SECTION("Value") { REQUIRE(s.l() == 0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(s.l())>);
    }
}
