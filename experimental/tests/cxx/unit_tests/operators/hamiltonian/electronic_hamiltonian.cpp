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
#include "chemist/operators/hamiltonian/electronic_hamiltonian.hpp"

using namespace chemist;
using namespace chemist::operators;

TEST_CASE("Electronic Hamiltonian") {
    ElectronicHamiltonian defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T   = NElectronKinetic{nelec};
    auto V   = NElectronNuclearAttraction{nelec, nuclei};
    auto G   = NElectronRepulsion{nelec};
    auto V_n = NuclearRepulsion{nuclei};

    ElectronicHamiltonian non_default{T, V, G};

    Hamiltonian ham_input{T, V, G, V_n};
    ElectronicHamiltonian from_ham{ham_input};

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted == ElectronicHamiltonian{}); }
        SECTION("With Values") {
            REQUIRE(non_default == ElectronicHamiltonian{T, V, G});
        }
        SECTION("From Hamiltonian") {
            REQUIRE(from_ham.has_term<NElectronKinetic>());
            REQUIRE(from_ham.has_term<NElectronNuclearAttraction>());
            REQUIRE(from_ham.has_term<NElectronRepulsion>());
            REQUIRE_FALSE(from_ham.has_term<NuclearRepulsion>());
            REQUIRE(from_ham == non_default);
        }
        SECTION("Copy") {
            ElectronicHamiltonian other(from_ham);
            REQUIRE(other == non_default);
        }
        SECTION("Move") {
            ElectronicHamiltonian other(std::move(from_ham));
            REQUIRE(other == non_default);
        }
        SECTION("Copy assignment") {
            ElectronicHamiltonian copy;
            auto pcopy = &(copy = from_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
        SECTION("Move assignment") {
            ElectronicHamiltonian copy;
            auto pcopy = &(copy = std::move(from_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                ElectronicHamiltonian rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != from_ham);
                REQUIRE_FALSE(defaulted == from_ham);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "O"); }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default.size() == 3);
    }
}
