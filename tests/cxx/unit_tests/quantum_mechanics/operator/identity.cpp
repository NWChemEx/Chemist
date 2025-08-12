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

#include "../../test_helpers.hpp"
#include <chemist/quantum_mechanics/operator/identity.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

TEST_CASE("Identity") {
    Identity defaulted;

    SECTION("Ctors and assignment") {
        REQUIRE(defaulted == Identity{});
        test_chemist::test_copy_and_move(defaulted);
    }
}
