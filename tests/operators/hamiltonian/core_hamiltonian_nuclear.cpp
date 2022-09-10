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
#include "chemist/operators/hamiltonian/core_hamiltonian_nuclear.hpp"
#include <pluginplay/pluginplay.hpp>

using namespace chemist;
using namespace chemist::operators;

TEST_CASE("Core Hamiltonian Deriv") {
    CoreHamiltonian_Nuclear defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T  = NElectronKinetic_Nuclear{nelec, nuclei};
    auto Vv = NElectronNuclearAttraction{nelec, nuclei};
    auto V  = NElectronNuclearAttraction_Nuclear{Vv, nuclei};
    auto Gg = NElectronRepulsion{nelec};
    auto G  = NElectronRepulsion_Nuclear{Gg, nuclei};
    auto t  = ElectronKinetic_Nuclear{};
    auto v  = ElectronNuclearAttraction_Nuclear{};

    ElectronicHamiltonian_Nuclear h_e{T, V, G};

    CoreHamiltonian_Nuclear non_default{t, v}, from_elec_ham{h_e};

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted == ElectronicHamiltonian_Nuclear{});
        }
        SECTION("With Values") {
            REQUIRE(non_default == CoreHamiltonian_Nuclear{t, v});
        }
        SECTION("From ElectronicHamiltonian") {
            REQUIRE(from_elec_ham.has_term<ElectronKinetic_Nuclear>());
            REQUIRE(
              from_elec_ham.has_term<ElectronNuclearAttraction_Nuclear>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronKinetic_Nuclear>());
            REQUIRE_FALSE(
              from_elec_ham.has_term<NElectronNuclearAttraction_Nuclear>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronRepulsion_Nuclear>());
            REQUIRE(from_elec_ham == non_default);
        }
        SECTION("Copy") {
            CoreHamiltonian_Nuclear other(from_elec_ham);
            REQUIRE(other == non_default);
        }
        SECTION("Move") {
            CoreHamiltonian_Nuclear other(std::move(from_elec_ham));
            REQUIRE(other == non_default);
        }
        SECTION("Copy assignment") {
            CoreHamiltonian_Nuclear copy;
            auto pcopy = &(copy = from_elec_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
        SECTION("Move assignment") {
            CoreHamiltonian_Nuclear copy;
            auto pcopy = &(copy = std::move(from_elec_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
    }

    SECTION("Hash") {
        SECTION("LHS == default") {
            auto lhs = pluginplay::hash_objects(defaulted);

            SECTION("LHS == RHS") {
                CoreHamiltonian_Nuclear rhs;
                REQUIRE(lhs == pluginplay::hash_objects(rhs));
            }

            SECTION("LHS != RHS") {
                REQUIRE(lhs != pluginplay::hash_objects(from_elec_ham));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                CoreHamiltonian_Nuclear rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != from_elec_ham);
                REQUIRE_FALSE(defaulted == from_elec_ham);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Ô"); }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default.size() == 2);
    }
}
