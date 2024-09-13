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
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>

TEST_CASE("AOs") {
    using chemist::wavefunction::AOs;
    using ao_basis_type = AOs::ao_basis_type;

    auto h2_basis = test_chemist::h2_basis();

    AOs defaulted;
    AOs value(h2_basis);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.ao_basis_set() == ao_basis_type{});
        }

        SECTION("Value") {
            REQUIRE(value.size() == 2);
            REQUIRE(value.ao_basis_set() == h2_basis);
        }

        SECTION("Copy/Move") {
            test_chemist::test_copy_and_move(defaulted, value);
        }
    }

    SECTION("ao_basis_set()") {
        REQUIRE(defaulted.ao_basis_set() == ao_basis_type{});
        REQUIRE(value.ao_basis_set() == h2_basis);
    }

    SECTION("ao_basis_set() const") {
        REQUIRE(std::as_const(defaulted).ao_basis_set() == ao_basis_type{});
        REQUIRE(std::as_const(value).ao_basis_set() == h2_basis);
    }

    SECTION("clone") {
        REQUIRE(defaulted.clone()->are_equal(defaulted));
        REQUIRE(value.clone()->are_equal(value));
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(value.size() == 2);
    }

    SECTION("are_equal") {
        // Relies on operator== so just spot check
        REQUIRE(defaulted.are_equal(AOs{}));
        REQUIRE_FALSE(defaulted.are_equal(value));
    }

    SECTION("swap") {
        AOs defaulted_copy(defaulted);
        AOs value_copy(value);

        value.swap(defaulted);

        REQUIRE(defaulted_copy == value);
        REQUIRE(value_copy == defaulted);
    }

    SECTION("operator==") {
        // Default == default
        REQUIRE(defaulted == AOs{});

        // Default != value
        REQUIRE_FALSE(defaulted == value);

        // value == value (with same value)
        REQUIRE(value == AOs{h2_basis});

        ao_basis_type h_basis;
        h_basis.add_center(h2_basis[0]);

        // value != value (with different value)
        REQUIRE_FALSE(value == AOs{h_basis});
    }

    SECTION("operator!=") {
        // Just negates operator==, so spot check
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != AOs{});
    }
}
