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

#include "../catch.hpp"
#include "../test_helpers.hpp"
#include <chemist/electron/electron_class.hpp>

TEST_CASE("Electron") {
    using chemist::Electron;

    Electron e;

    SECTION("copy/move ctor/assignment") {
        test_chemist::test_copy_and_move(e);
    }

    SECTION("operator==") { REQUIRE(e == Electron{}); }
    SECTION("operator!=") { REQUIRE_FALSE(e != Electron{}); }
}