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
#include "chemist/operators/hamiltonian/core_hamiltonian.hpp"

using namespace chemist;
using namespace chemist::operators;

TEST_CASE("Core Hamiltonian") {
    CoreHamiltonian defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T = NElectronKinetic{nelec};
    auto V = NElectronNuclearAttraction{nelec, nuclei};
    auto G = NElectronRepulsion{nelec};
    auto t = ElectronKinetic{};
    auto v = ElectronNuclearAttraction{Electron{}, nuclei};

    ElectronicHamiltonian h_e{T, V, G};

    CoreHamiltonian non_default{t, v}, from_elec_ham{h_e};

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted == ElectronicHamiltonian{}); }
        SECTION("With Values") {
            REQUIRE(non_default == CoreHamiltonian{t, v});
        }
        SECTION("From ElectronicHamiltonian") {
            REQUIRE(from_elec_ham.has_term<ElectronKinetic>());
            REQUIRE(from_elec_ham.has_term<ElectronNuclearAttraction>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronKinetic>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronNuclearAttraction>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronRepulsion>());
            REQUIRE(from_elec_ham == non_default);
        }
        SECTION("Copy") {
            CoreHamiltonian other(from_elec_ham);
            REQUIRE(other == non_default);
        }
        SECTION("Move") {
            CoreHamiltonian other(std::move(from_elec_ham));
            REQUIRE(other == non_default);
        }
        SECTION("Copy assignment") {
            CoreHamiltonian copy;
            auto pcopy = &(copy = from_elec_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
        SECTION("Move assignment") {
            CoreHamiltonian copy;
            auto pcopy = &(copy = std::move(from_elec_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                CoreHamiltonian rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != from_elec_ham);
                REQUIRE_FALSE(defaulted == from_elec_ham);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "O"); }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default.size() == 2);
    }
}
