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
#include <chemist/quantum_mechanics/quantum_mechanics.hpp>

using namespace chemist;

TEST_CASE("Density<MOs, Electron>") {
    using orbital_type  = wavefunction::MOs;
    using particle_type = Electron;
    using density_type  = qm_operator::Density<orbital_type, particle_type>;
    using weight_type   = typename density_type::weight_type;

    orbital_type defaulted_mos;
    auto h2_mos = test_chemist::h2_mos();

    std::vector<weight_type> defaulted_weights;
    std::vector<weight_type> h2_weights{2.0, 0.0};

    density_type defaulted;
    density_type defaulted_value(defaulted_mos, defaulted_weights);
    density_type value(h2_mos, h2_weights);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.orbitals() == defaulted_mos);
            REQUIRE(defaulted.weights() == defaulted_weights);
        }

        SECTION("Value") {
            REQUIRE(defaulted_value.size() == 0);
            REQUIRE(defaulted_value.orbitals() == defaulted_mos);
            REQUIRE(defaulted_value.weights() == defaulted_weights);

            REQUIRE(value.size() == 2);
            REQUIRE(value.orbitals() == h2_mos);
            REQUIRE(value.weights() == h2_weights);

            using error_t = std::runtime_error;
            REQUIRE_THROWS_AS(density_type(h2_mos, defaulted_weights), error_t);
        }

        SECTION("Value (wrong weight type)") {
            std::vector<int> occupations{2, 0};
            density_type value2(h2_mos, occupations);
            REQUIRE(value2.size() == 2);
            REQUIRE(value2.orbitals() == h2_mos);
            REQUIRE(value2.weights() == h2_weights);
        }

        SECTION("Determinant") {
            using determinant_type = wavefunction::Determinant<orbital_type>;
            using orbital_index_set_type =
              typename determinant_type::orbital_index_set_type;
            orbital_index_set_type occs{0};
            determinant_type Psi(occs, h2_mos);
            density_type value2(Psi);
            REQUIRE(value2.size() == 2);
            REQUIRE(value2.orbitals() == h2_mos);
            REQUIRE(value2.weights() == h2_weights);
        }

        test_chemist::test_copy_and_move(defaulted, defaulted_value, value);
    }

    SECTION("orbitals()") {
        REQUIRE(defaulted.orbitals() == defaulted_mos);
        REQUIRE(value.orbitals() == h2_mos);
    }

    SECTION("orbitals() const") {
        REQUIRE(std::as_const(defaulted).orbitals() == defaulted_mos);
        REQUIRE(std::as_const(value).orbitals() == h2_mos);
    }

    SECTION("weights()") {
        REQUIRE(defaulted.weights() == defaulted_weights);
        REQUIRE(value.weights() == h2_weights);
    }

    SECTION("weights() const") {
        REQUIRE(std::as_const(defaulted).weights() == defaulted_weights);
        REQUIRE(std::as_const(value).weights() == h2_weights);
    }

    SECTION("operator==") {
        REQUIRE(defaulted == density_type{});
        REQUIRE(defaulted == defaulted_value);
        REQUIRE_FALSE(defaulted == value);

        REQUIRE(value == density_type(h2_mos, h2_weights));

        typename wavefunction::MOs::transform_type c{{1.0, 0.0}, {0.0, 1.0}};
        wavefunction::MOs mos2(h2_mos.from_space(), c);
        REQUIRE_FALSE(value == density_type(mos2, h2_weights));

        std::vector<weight_type> weights2{0.0, 2.0};
        REQUIRE_FALSE(value == density_type(h2_mos, weights2));
    }

    SECTION("operator!=") {
        REQUIRE(value != defaulted);
        REQUIRE_FALSE(defaulted != density_type{});
    }
}
