#include "libchemist/operators/hamiltonian/core_hamiltonian.hpp"
#include "../test_operator.hpp"

using namespace libchemist;
using namespace libchemist::operators;

TEST_CASE("Core Hamiltonian") {
    CoreHamiltonian defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T = NElectronKinetic{nelec};
    auto V = NElectronNuclearAttraction{nelec, nuclei};
    auto G = NElectronRepulsion{nelec};
    auto t = ElectronKinetic{};
    auto v = ElectronNuclearAttraction{Electron{}, nuclei};

    ElectronicHamiltonian h_e;
    h_e.add_terms(T, V, G);

    CoreHamiltonian corr, from_elec_ham{h_e};
    corr.add_terms(t, v);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted == ElectronicHamiltonian{});
        }
        SECTION("From Hamiltonian") {
            REQUIRE(from_elec_ham.has_term<ElectronKinetic>());
            REQUIRE(from_elec_ham.has_term<ElectronNuclearAttraction>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronKinetic>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronNuclearAttraction>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronRepulsion>());
            REQUIRE(from_elec_ham == corr);
        }
        SECTION("Copy") {
            CoreHamiltonian other(from_elec_ham);
            REQUIRE(other == corr);
        }
        SECTION("Move") {
            CoreHamiltonian other(std::move(from_elec_ham));
            REQUIRE(other == corr);
        }
        SECTION("Copy assignment") {
            CoreHamiltonian copy;
            auto pcopy = &(copy = from_elec_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == corr);
        }
        SECTION("Move assignment") {
            CoreHamiltonian copy;
            auto pcopy = &(copy = std::move(from_elec_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == corr);
        }
    }

    SECTION("Hash") {
        SECTION("LHS == default") {
            auto lhs = pluginplay::hash_objects(defaulted);

            SECTION("LHS == RHS") {
                CoreHamiltonian rhs;
                REQUIRE(lhs == pluginplay::hash_objects(rhs));
            }

            SECTION("LHS != RHS") {
                REQUIRE(lhs != pluginplay::hash_objects(from_elec_ham));
            }
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

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Ô"); }
}
