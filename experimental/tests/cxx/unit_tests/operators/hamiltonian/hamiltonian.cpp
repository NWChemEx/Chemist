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
#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/hamiltonian/hamiltonian.hpp"
#include "chemist/operators/kinetic.hpp"

using namespace chemist;
using namespace chemist::operators;

TEST_CASE("Hamiltonian Class") {
    SECTION("Default CTor") {
        Hamiltonian ham;
        CHECK(ham.get_terms<ElectronKinetic>().size() == 0);
        CHECK(ham.size() == 0);
        CHECK(ham.n_electrons() == 0);
    }

    SECTION("One Operator Construction") {
        Hamiltonian ham(NElectronKinetic{ManyElectrons{2}});
        CHECK(ham.has_term<NElectronKinetic>());
        CHECK(ham.get_terms<NElectronKinetic>().size() == 1);
        CHECK(ham.size() == 1);
        CHECK(ham.n_electrons() == 2);
    }

    SECTION("Two Operator Construction") {
        Hamiltonian ham(ElectronKinetic{}, ElectronNuclearAttraction{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearAttraction>().size() == 1);
        CHECK(ham.size() == 2);
    }

    SECTION("Single Term Add") {
        Hamiltonian ham;
        ham.add_term(ElectronKinetic{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.size() == 1);
    }

    SECTION("Term Add Chain") {
        Hamiltonian ham;
        ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearAttraction{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearAttraction>().size() == 1);
        CHECK(ham.size() == 2);
    }

    SECTION("Copy CTor") {
        Hamiltonian ham(ElectronKinetic{}, ElectronNuclearAttraction{});

        Hamiltonian copy(ham);

        CHECK(copy == ham);
    }

    SECTION("Move CTor") {
        Hamiltonian ref(ElectronKinetic{}, ElectronNuclearAttraction{});
        Hamiltonian ham_1(ref);

        Hamiltonian ham_2(std::move(ham_1));

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("Copy Assignment") {
        Hamiltonian ref(ElectronKinetic{}, ElectronNuclearAttraction{});
        Hamiltonian ham(ElectronRepulsion{});

        ham = ref;

        CHECK(ham == ref);
        CHECK(ham.get_terms<ElectronRepulsion>().size() == 0);
    }

    SECTION("Move Assignment") {
        Hamiltonian ref(ElectronKinetic{}, ElectronNuclearAttraction{});
        Hamiltonian ham_1(ref);
        Hamiltonian ham_2(ElectronRepulsion{});

        ham_2 = std::move(ham_1);

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
        CHECK(ham_2.get_terms<ElectronRepulsion>().size() == 0);
    }
}
