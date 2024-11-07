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
#include <chemist/quantum_mechanics/wavefunction/determinant.hpp>
#include <chemist/quantum_mechanics/wavefunction/mos.hpp>

TEST_CASE("Determinant<MOs>") {
    using orbitals_type    = chemist::wavefunction::MOs;
    using determinant_type = chemist::wavefunction::Determinant<orbitals_type>;
    using index_set_type   = typename determinant_type::orbital_index_set_type;

    orbitals_type defaulted_mos;
    auto mos = test_chemist::h2_mos();

    index_set_type i;
    index_set_type i0{0};

    determinant_type defaulted;
    determinant_type value(i0, mos);

    SECTION("Ctors and Assignment") {
        SECTION("Defaulted") {
            REQUIRE(defaulted.orbital_indices() == i);
            REQUIRE(defaulted.orbitals() == defaulted_mos);
        }

        SECTION("Value") {
            REQUIRE(value.orbital_indices() == i0);
            REQUIRE(value.orbitals() == mos);
        }

        test_chemist::test_copy_and_move(defaulted, value);
    }

    SECTION("orbital_indices()") {
        REQUIRE(defaulted.orbital_indices() == i);
        REQUIRE(value.orbital_indices() == i0);
    }

    SECTION("orbital_indices() const") {
        REQUIRE(std::as_const(defaulted).orbital_indices() == i);
        REQUIRE(std::as_const(value).orbital_indices() == i0);
    }

    SECTION("orbitals()") {
        REQUIRE(defaulted.orbitals() == defaulted_mos);
        REQUIRE(value.orbitals() == mos);
    }

    SECTION("orbitals() const") {
        REQUIRE(std::as_const(defaulted).orbitals() == defaulted_mos);
        REQUIRE(std::as_const(value).orbitals() == mos);
    }

    SECTION("operator==") {
        SECTION("Default vs. default") {
            REQUIRE(defaulted == determinant_type{});
        }

        SECTION("Default vs. non-default") {
            REQUIRE_FALSE(defaulted == value);
        }

        SECTION("Value vs. same value") {
            REQUIRE(value == determinant_type(i0, mos));
        }

        SECTION("Different occupieds") {
            REQUIRE_FALSE(value == determinant_type(i, mos));
        }

        SECTION("Different mos") {
            tensorwrapper::Tensor c2({{1.0, 0.0}, {0.0, 1.0}});
            decltype(mos) mos2(test_chemist::h2_aos(), c2);
            REQUIRE_FALSE(value == determinant_type(i0, mos2));
        }
    }

    SECTION("operator!=") {
        // N.b. just negates operator== so spot check
        REQUIRE_FALSE(defaulted != determinant_type{});
        REQUIRE(defaulted != value);
    }
}