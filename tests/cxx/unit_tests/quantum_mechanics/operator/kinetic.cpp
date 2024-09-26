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
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

using types2test = std::tuple<Electron, ManyElectrons, Nucleus>;

TEMPLATE_LIST_TEST_CASE("Kinetic", "", types2test) {
    types2test defaulted_values;
    types2test values{Electron{}, ManyElectrons{3}, Nucleus{"H", 1ul, 1.0079}};

    auto corr_defaulted = std::get<TestType>(defaulted_values);
    auto corr_value     = std::get<TestType>(values);

    using kinetic_type = Kinetic<TestType>;

    kinetic_type defaulted;
    kinetic_type value{corr_value};

    SECTION("Ctors and assignment") {
        SECTION("Defaulted") {
            REQUIRE(defaulted.particle() == corr_defaulted);
        }

        SECTION("Value") { REQUIRE(value.particle() == corr_value); }

        test_chemist::test_copy_and_move(defaulted, value);
    }

    SECTION("particle()") {
        REQUIRE(defaulted.particle() == corr_defaulted);
        REQUIRE(value.particle() == corr_value);
    }

    SECTION("particle() const") {
        REQUIRE(std::as_const(defaulted).particle() == corr_defaulted);
        REQUIRE(std::as_const(value).particle() == corr_value);
    }
}