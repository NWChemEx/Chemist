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
#include <chemist/density/decomposable_density.hpp>

/* Testing Strategy.
 *
 * The DecomposableDensity class extends Density by adding a left_factor
 * method and overriding the implementation of basis_set.
 */

using namespace chemist;

// Tuple containing the known densities
using density_types = std::tuple<Decomposable1EDensity>;

TEMPLATE_LIST_TEST_CASE("DecomposableDensity", "", density_types) {
    using density_type = TestType;
    using value_type   = typename density_type::value_type;
    using aos_type     = typename density_type::basis_type;
    using transformed_basis_type =
      typename density_type::transformed_basis_type;

    value_type a_tensor{{1.0, 0.0}, {0.0, 1.0}};
    value_type b_tensor{{2.0, 0.0}, {0.0, 2.0}};
    aos_type aos(test_chemist::h2_basis());
    transformed_basis_type mos(aos, b_tensor);

    density_type defaulted;
    density_type has_value(a_tensor, mos);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.value() == value_type{});
            REQUIRE(defaulted.basis_set() == aos_type{});
            REQUIRE(defaulted.left_factor() == value_type{});
        }

        SECTION("Space ctor") {
            REQUIRE(has_value.value() == a_tensor);
            REQUIRE(has_value.basis_set() == aos);
            REQUIRE(has_value.left_factor() == b_tensor);
        }

        SECTION("Tensor ctor") {
            density_type other(a_tensor, aos, b_tensor);
            REQUIRE(other == has_value);
        }

        test_chemist::test_copy_and_move(defaulted, has_value);
    }

    SECTION("basis_set") {
        REQUIRE(defaulted.basis_set() == aos_type{});
        REQUIRE(has_value.basis_set() == aos);
    }

    SECTION("left_factor") {
        REQUIRE(defaulted.left_factor() == value_type{});
        REQUIRE(has_value.left_factor() == b_tensor);
    }

    SECTION("Comparison") {
        SECTION("Both default") {
            density_type rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        SECTION("Both have same value") {
            density_type rhs(a_tensor, mos);
            REQUIRE(has_value == rhs);
            REQUIRE_FALSE(has_value != rhs);
        }

        SECTION("Different tensors") {
            density_type rhs(value_type{}, mos);
            REQUIRE(rhs != has_value);
            REQUIRE_FALSE(rhs == has_value);
        }

        SECTION("Different AOs") {
            density_type rhs(a_tensor, transformed_basis_type{});
            REQUIRE(rhs != has_value);
            REQUIRE_FALSE(rhs == has_value);
        }

        SECTION("With base_type") {
            Density<Electron> base_with_value(a_tensor, aos);
            REQUIRE(has_value == base_with_value);
            REQUIRE_FALSE(has_value != base_with_value);
        }
    }
}
