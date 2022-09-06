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

#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/fock.hpp"
#include "chemist/operators/kinetic.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace chemist::operators;

TEST_CASE("Fock Class") {
    SECTION("Default CTor") {
        Fock ham;
        CHECK(ham.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("One Operator Construction") {
        Fock ham(ElectronKinetic{});
        CHECK(ham.has_term<ElectronKinetic>());
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Two Operator Construction") {
        Fock ham(ElectronKinetic{}, ElectronNuclearAttraction{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearAttraction>().size() == 1);
    }

    SECTION("Single Term Add") {
        Fock ham;
        ham.add_term(ElectronKinetic{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Term Add Chain") {
        Fock ham;
        ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearAttraction{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearAttraction>().size() == 1);
    }

    SECTION("Copy CTor") {
        Fock ham(ElectronKinetic{}, ElectronNuclearAttraction{});

        Fock copy(ham);

        CHECK(copy == ham);
    }

    SECTION("Move CTor") {
        Fock ref(ElectronKinetic{}, ElectronNuclearAttraction{});
        Fock ham_1(ref);

        Fock ham_2(std::move(ham_1));

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("Copy Assignment") {
        Fock ref(ElectronKinetic{}, ElectronNuclearAttraction{});
        Fock ham(ElectronNuclearAttraction{});

        ham = ref;

        CHECK(ham == ref);
    }

    SECTION("Move Assignment") {
        Fock ref(ElectronKinetic{}, ElectronNuclearAttraction{});
        Fock ham_1(ref);
        Fock ham_2(ElectronNuclearAttraction{});

        ham_2 = std::move(ham_1);

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
    }
}
