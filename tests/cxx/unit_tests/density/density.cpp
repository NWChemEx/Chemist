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

#include "../quantum_mechanics/test_qm.hpp"
#include "../test_helpers.hpp"
#include <chemist/density/density.hpp>

// Tuple containing the known densities
using density_types = std::tuple<chemist::Density<chemist::Electron>>;

TEMPLATE_LIST_TEST_CASE("Density", "", density_types) {
    using density_type = TestType;
    using value_type   = typename density_type::value_type;
    using basis_type   = typename density_type::basis_type;
    basis_type orbs(test_chemist::h2_basis());

    density_type defaulted;
    value_type rho{{1.0, 2.0}, {2.0, 3.0}}; // Not realistic; fine for unit test
    density_type has_value(rho, orbs);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.value() == value_type{});
            REQUIRE(defaulted.basis_set() == basis_type{});
        }

        SECTION("Value") {
            REQUIRE(has_value.value() == rho);
            REQUIRE(has_value.basis_set() == orbs);
        }

        test_chemist::test_copy_and_move(defaulted, has_value);
    }

    SECTION("value()") {
        REQUIRE(defaulted.value() == value_type{});
        REQUIRE(has_value.value() == rho);
    }

    SECTION("value() const") {
        REQUIRE(std::as_const(defaulted).value() == value_type{});
        REQUIRE(std::as_const(has_value).value() == rho);
    }

    SECTION("basis_set") {
        REQUIRE(defaulted.basis_set() == basis_type{});
        REQUIRE(has_value.basis_set() == orbs);
    }

    SECTION("basis_set() const") {
        REQUIRE(std::as_const(defaulted).basis_set() == basis_type{});
        REQUIRE(std::as_const(has_value).basis_set() == orbs);
    }

    SECTION("Comparison") {
        SECTION("Both default") {
            density_type rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        SECTION("Both have same value") {
            density_type rhs(rho, orbs);
            REQUIRE(has_value == rhs);
            REQUIRE_FALSE(has_value != rhs);
        }

        SECTION("Different tensors") {
            density_type rhs(value_type{}, orbs);
            REQUIRE(rhs != has_value);
            REQUIRE_FALSE(rhs == has_value);
        }

        SECTION("Different basis") {
            density_type rhs(rho, basis_type{});
            REQUIRE(rhs != has_value);
            REQUIRE_FALSE(rhs == has_value);
        }
    }
}
