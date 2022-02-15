#include "../test_operator.hpp"
#include "libchemist/operators/hamiltonian/electronic_hamiltonian_nuclear.hpp"

using namespace libchemist;
using namespace libchemist::operators;

TEST_CASE("Electronic Hamiltonian Nuclear") {
    ElectronicHamiltonian_Nuclear defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T   = NElectronKinetic_Nuclear{nelec};
    auto V   = NElectronNuclearAttraction_Nuclear{nelec, nuclei};
    auto G   = NElectronRepulsion_Nuclear{nelec};
    auto V_n = NuclearRepulsion_Nuclear{nuclei};

    ElectronicHamiltonian_Nuclear non_default{T, V, G};

    Hamiltonian_Nuclear ham_input{T, V, G, V_n};
    ElectronicHamiltonian_Nuclear from_ham{ham_input};

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted == ElectronicHamiltonian_Nuclear{});
        }
        SECTION("With Values") {
            REQUIRE(non_default == ElectronicHamiltonian_Nuclear{T, V, G});
        }
        SECTION("From Hamiltonian") {
            REQUIRE(from_ham.has_term<NElectronKinetic_Nuclear>());
            REQUIRE(from_ham.has_term<NElectronNuclearAttraction_Nuclear>());
            REQUIRE(from_ham.has_term<NElectronRepulsion_Nuclear>());
            REQUIRE_FALSE(from_ham.has_term<NuclearRepulsion_Nuclear>());
            REQUIRE(from_ham == non_default);
        }
        SECTION("Copy") {
            ElectronicHamiltonian_Nuclear other(from_ham);
            REQUIRE(other == non_default);
        }
        SECTION("Move") {
            ElectronicHamiltonian_Nuclear other(std::move(from_ham));
            REQUIRE(other == non_default);
        }
        SECTION("Copy assignment") {
            ElectronicHamiltonian_Nuclear copy;
            auto pcopy = &(copy = from_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
        SECTION("Move assignment") {
            ElectronicHamiltonian_Nuclear copy;
            auto pcopy = &(copy = std::move(from_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
    }

    SECTION("Hash") {
        SECTION("LHS == default") {
            auto lhs = pluginplay::hash_objects(defaulted);

            SECTION("LHS == RHS") {
                ElectronicHamiltonian_Nuclear rhs;
                REQUIRE(lhs == pluginplay::hash_objects(rhs));
            }

            SECTION("LHS != RHS") {
                REQUIRE(lhs != pluginplay::hash_objects(from_ham));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                ElectronicHamiltonian_Nuclear rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != from_ham);
                REQUIRE_FALSE(defaulted == from_ham);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Ô"); }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default.size() == 3);
    }
}
