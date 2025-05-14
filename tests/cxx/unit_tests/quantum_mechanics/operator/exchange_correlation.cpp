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
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/operator/exchange_correlation.hpp>

using namespace chemist;
using namespace chemist::qm_operator;

using types2test =
  std::tuple<std::pair<Electron, chemist::Density<Electron>>,
             std::pair<ManyElectrons, chemist::Density<Electron>>,
             std::pair<Electron, DecomposableDensity<Electron>>,
             std::pair<ManyElectrons, DecomposableDensity<Electron>>>;

TEMPLATE_LIST_TEST_CASE("ExchangeCorrelation", "", types2test) {
    auto defaulted_values = test_chemist::defaulted_particles();
    auto values           = test_chemist::non_defaulted_particles();

    using lhs_type = std::tuple_element_t<0, TestType>;
    using rhs_type = std::tuple_element_t<1, TestType>;

    auto corr_lhs_defaulted = std::get<lhs_type>(defaulted_values);
    auto corr_lhs_value     = std::get<lhs_type>(values);
    auto corr_rhs_defaulted = std::get<rhs_type>(defaulted_values);
    auto corr_rhs_value     = std::get<rhs_type>(values);

    using xc_type = ExchangeCorrelation<lhs_type, rhs_type>;

    xc_type defaulted;
    xc_type value(xc_functional::PBE, corr_lhs_value, corr_rhs_value);

    SECTION("Ctors and assignment") {
        SECTION("Defaulted") {
            REQUIRE(defaulted.lhs_particle() == corr_lhs_defaulted);
            REQUIRE(defaulted.rhs_particle() == corr_rhs_defaulted);
            REQUIRE(defaulted.functional_name() == xc_functional::NONE);
        }

        SECTION("Value") {
            REQUIRE(value.lhs_particle() == corr_lhs_value);
            REQUIRE(value.rhs_particle() == corr_rhs_value);
            REQUIRE(value.functional_name() == xc_functional::PBE);
        }

        test_chemist::test_copy_and_move(defaulted, value);
    }

    SECTION("lhs_particle()") {
        REQUIRE(defaulted.lhs_particle() == corr_lhs_defaulted);
        REQUIRE(value.lhs_particle() == corr_lhs_value);
    }

    SECTION("lhs_particle() const") {
        REQUIRE(std::as_const(defaulted).lhs_particle() == corr_lhs_defaulted);
        REQUIRE(std::as_const(value).lhs_particle() == corr_lhs_value);
    }

    SECTION("rhs_particle()") {
        REQUIRE(defaulted.rhs_particle() == corr_rhs_defaulted);
        REQUIRE(value.rhs_particle() == corr_rhs_value);
    }

    SECTION("rhs_particle() const") {
        REQUIRE(std::as_const(defaulted).rhs_particle() == corr_rhs_defaulted);
        REQUIRE(std::as_const(value).rhs_particle() == corr_rhs_value);
    }

    SECTION("functional_name()") {
        REQUIRE(defaulted.functional_name() == xc_functional::NONE);
        REQUIRE(value.functional_name() == xc_functional::PBE);
    }

    SECTION("functional_name() const") {
        REQUIRE(std::as_const(defaulted).functional_name() ==
                xc_functional::NONE);
        REQUIRE(std::as_const(value).functional_name() == xc_functional::PBE);
    }
}