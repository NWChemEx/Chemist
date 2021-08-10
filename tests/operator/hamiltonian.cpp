#include "libchemist/operator/coulomb.hpp"
#include "libchemist/operator/hamiltonian.hpp"
#include "libchemist/operator/kinetic.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist::operators;

TEST_CASE("Hamiltonian Class") {
    SECTION("Default CTor") {
        Hamiltonian ham;
        CHECK(ham.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("One Operator Construction") {
        Hamiltonian ham(ElectronKinetic{});
        CHECK(ham.has_term<ElectronKinetic>());
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Two Operator Construction") {
        Hamiltonian ham(ElectronKinetic{}, ElectronNuclearAttraction{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearAttraction>().size() == 1);
    }

    SECTION("Single Term Add") {
        Hamiltonian ham;
        ham.add_term(ElectronKinetic{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Term Add Chain") {
        Hamiltonian ham;
        ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearAttraction{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearAttraction>().size() == 1);
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
