#include "libchemist/operator/coulomb.hpp"
#include "libchemist/operator/hamiltonian.hpp"
#include "libchemist/operator/kinetic.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;

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
        Hamiltonian ham(ElectronKinetic{}, ElectronNuclearCoulomb{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearCoulomb>().size() == 1);
    }

    SECTION("Single Term Add") {
        Hamiltonian ham;
        ham.add_term(ElectronKinetic{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Term Add Chain") {
        Hamiltonian ham;
        ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearCoulomb{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearCoulomb>().size() == 1);
    }

    SECTION("Copy CTor") {
        Hamiltonian ham(ElectronKinetic{}, ElectronNuclearCoulomb{});

        Hamiltonian copy(ham);

        CHECK(copy == ham);
    }

    SECTION("Move CTor") {
        Hamiltonian ref(ElectronKinetic{}, ElectronNuclearCoulomb{});
        Hamiltonian ham_1(ref);

        Hamiltonian ham_2(std::move(ham_1));

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("Copy Assignment") {
        Hamiltonian ref(ElectronKinetic{}, ElectronNuclearCoulomb{});
        Hamiltonian ham(ElectronElectronCoulomb{});

        ham = ref;

        CHECK(ham == ref);
        CHECK(ham.get_terms<ElectronElectronCoulomb>().size() == 0);
    }

    SECTION("Move Assignment") {
        Hamiltonian ref(ElectronKinetic{}, ElectronNuclearCoulomb{});
        Hamiltonian ham_1(ref);
        Hamiltonian ham_2(ElectronElectronCoulomb{});

        ham_2 = std::move(ham_1);

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
        CHECK(ham_2.get_terms<ElectronElectronCoulomb>().size() == 0);
    }
}
