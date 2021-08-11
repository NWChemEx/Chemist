#include "libchemist/operators/hamiltonian/electronic_hamiltonian.hpp"
#include "../test_operator.hpp"

using namespace libchemist;
using namespace libchemist::operators;

TEST_CASE("Electronic Hamiltonian") {
    ElectronicHamiltonian defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T   = NElectronKinetic{nelec};
    auto V   = NElectronNuclearAttraction{nelec, nuclei};
    auto G   = NElectronRepulsion{nelec};
    auto V_n = NuclearRepulsion{nuclei};

    ElectronicHamiltonian corr;
    corr.add_terms(T, V, G);

    Hamiltonian ham_input{T, V, G, V_n};
    ElectronicHamiltonian from_ham{ham_input};

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted == ElectronicHamiltonian{});
        }
        SECTION("From Hamiltonian") {
            REQUIRE(from_ham.has_term<NElectronKinetic>());
            REQUIRE(from_ham.has_term<NElectronNuclearAttraction>());
            REQUIRE(from_ham.has_term<NElectronRepulsion>());
            REQUIRE_FALSE(from_ham.has_term<NuclearRepulsion>());
            REQUIRE(from_ham == corr);
        }
        SECTION("Copy") {
            ElectronicHamiltonian other(from_ham);
            REQUIRE(other == corr);
        }
        SECTION("Move") {
            ElectronicHamiltonian other(std::move(from_ham));
            REQUIRE(other == corr);
        }
        SECTION("Copy assignment") {
            ElectronicHamiltonian copy;
            auto pcopy = &(copy = from_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == corr);
        }
        SECTION("Move assignment") {
            ElectronicHamiltonian copy;
            auto pcopy = &(copy = std::move(from_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == corr);
        }
    }

    SECTION("Hash") {
        SECTION("LHS == default") {
            auto lhs = pluginplay::hash_objects(defaulted);

            SECTION("LHS == RHS") {
                ElectronicHamiltonian rhs;
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

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Ô"); }
}
