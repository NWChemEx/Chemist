#include "../test_operator.hpp"
#include "chemist/operators/hamiltonian/core_hamiltonian.hpp"

using namespace chemist;
using namespace chemist::operators;

TEST_CASE("Core Hamiltonian") {
    CoreHamiltonian defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T = NElectronKinetic{nelec};
    auto V = NElectronNuclearAttraction{nelec, nuclei};
    auto G = NElectronRepulsion{nelec};
    auto t = ElectronKinetic{};
    auto v = ElectronNuclearAttraction{Electron{}, nuclei};

    ElectronicHamiltonian h_e{T, V, G};

    CoreHamiltonian non_default{t, v}, from_elec_ham{h_e};

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted == ElectronicHamiltonian{}); }
        SECTION("With Values") {
            REQUIRE(non_default == CoreHamiltonian{t, v});
        }
        SECTION("From ElectronicHamiltonian") {
            REQUIRE(from_elec_ham.has_term<ElectronKinetic>());
            REQUIRE(from_elec_ham.has_term<ElectronNuclearAttraction>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronKinetic>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronNuclearAttraction>());
            REQUIRE_FALSE(from_elec_ham.has_term<NElectronRepulsion>());
            REQUIRE(from_elec_ham == non_default);
        }
        SECTION("Copy") {
            CoreHamiltonian other(from_elec_ham);
            REQUIRE(other == non_default);
        }
        SECTION("Move") {
            CoreHamiltonian other(std::move(from_elec_ham));
            REQUIRE(other == non_default);
        }
        SECTION("Copy assignment") {
            CoreHamiltonian copy;
            auto pcopy = &(copy = from_elec_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
        SECTION("Move assignment") {
            CoreHamiltonian copy;
            auto pcopy = &(copy = std::move(from_elec_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
    }

    SECTION("Hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(defaulted);

            SECTION("LHS == RHS") {
                CoreHamiltonian rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("LHS != RHS") {
                REQUIRE(lhs != hash_objects(from_elec_ham));
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

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default.size() == 2);
    }
}
