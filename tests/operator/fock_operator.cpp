#include "libchemist/operator/coulomb.hpp"
#include "libchemist/operator/fock_operator.hpp"
#include "libchemist/operator/kinetic.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;

TEST_CASE("FockOperator Class") {
    SECTION("Default CTor") {
        FockOperator ham;
        CHECK(ham.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("One Operator Construction") {
        FockOperator ham(ElectronKinetic{});
        CHECK(ham.has_term<ElectronKinetic>());
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Two Operator Construction") {
        FockOperator ham(ElectronKinetic{}, ElectronNuclearCoulomb{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearCoulomb>().size() == 1);
    }

    SECTION("Single Term Add") {
        FockOperator ham;
        ham.add_term(ElectronKinetic{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
    }

    SECTION("Term Add Chain") {
        FockOperator ham;
        ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearCoulomb{});
        CHECK(ham.get_terms<ElectronKinetic>().size() == 1);
        CHECK(ham.get_terms<ElectronNuclearCoulomb>().size() == 1);
    }

    SECTION("Copy CTor") {
        FockOperator ham(ElectronKinetic{}, ElectronNuclearCoulomb{});

        FockOperator copy(ham);

        CHECK(copy == ham);
    }

    SECTION("Move CTor") {
        FockOperator ref(ElectronKinetic{}, ElectronNuclearCoulomb{});
        FockOperator ham_1(ref);

        FockOperator ham_2(std::move(ham_1));

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
    }

    SECTION("Copy Assignment") {
        FockOperator ref(ElectronKinetic{}, ElectronNuclearCoulomb{});
        FockOperator ham(ElectronNuclearCoulomb{});

        ham = ref;

        CHECK(ham == ref);
    }

    SECTION("Move Assignment") {
        FockOperator ref(ElectronKinetic{}, ElectronNuclearCoulomb{});
        FockOperator ham_1(ref);
        FockOperator ham_2(ElectronNuclearCoulomb{});

        ham_2 = std::move(ham_1);

        CHECK(ham_2 == ref);
        CHECK(ham_1.get_terms<ElectronKinetic>().size() == 0);
    }
}
