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

#include "../test_operator.hpp"
#include <iostream>

using namespace chemist::operators;

/* Testing strategy:
 *
 * OperatorBase is an abstract class, so we can't make instances of it. Instead
 * we loop over instances of the derived classes, which get used in practice.
 * For each of these derived classes we cast them down to OperatorBase and test
 * the non-virtual members (virtual members are tested in the derived classes).
 */

TEMPLATE_LIST_TEST_CASE("OperatorBase", "", testing::all_operators) {
    using derived_type = TestType;

    derived_type defaulted;

    SECTION("Non-polymorphic comparison") {
        REQUIRE(derived_type{} == defaulted);
        REQUIRE_FALSE(derived_type{} != defaulted);
    }

    SECTION("coefficient") { REQUIRE(defaulted.coefficient() == 1.0); }
}
