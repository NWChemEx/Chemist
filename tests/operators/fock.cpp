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
