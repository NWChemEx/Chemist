#include "../test_operator.hpp"
#include "chemist/operators/hamiltonian/electronic_hamiltonian.hpp"

using namespace chemist;
using namespace chemist::operators;

TEST_CASE("Electronic Hamiltonian") {
    ElectronicHamiltonian defaulted;

    auto nelec  = testing::non_default_parameter<ManyElectrons>();
    auto nuclei = testing::non_default_parameter<Nuclei>();

    auto T   = NElectronKinetic{nelec};
    auto V   = NElectronNuclearAttraction{nelec, nuclei};
    auto G   = NElectronRepulsion{nelec};
    auto V_n = NuclearRepulsion{nuclei};

    ElectronicHamiltonian non_default{T, V, G};

    Hamiltonian ham_input{T, V, G, V_n};
    ElectronicHamiltonian from_ham{ham_input};

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted == ElectronicHamiltonian{}); }
        SECTION("With Values") {
            REQUIRE(non_default == ElectronicHamiltonian{T, V, G});
        }
        SECTION("From Hamiltonian") {
            REQUIRE(from_ham.has_term<NElectronKinetic>());
            REQUIRE(from_ham.has_term<NElectronNuclearAttraction>());
            REQUIRE(from_ham.has_term<NElectronRepulsion>());
            REQUIRE_FALSE(from_ham.has_term<NuclearRepulsion>());
            REQUIRE(from_ham == non_default);
        }
        SECTION("Copy") {
            ElectronicHamiltonian other(from_ham);
            REQUIRE(other == non_default);
        }
        SECTION("Move") {
            ElectronicHamiltonian other(std::move(from_ham));
            REQUIRE(other == non_default);
        }
        SECTION("Copy assignment") {
            ElectronicHamiltonian copy;
            auto pcopy = &(copy = from_ham);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
        SECTION("Move assignment") {
            ElectronicHamiltonian copy;
            auto pcopy = &(copy = std::move(from_ham));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == non_default);
        }
    }

    SECTION("Hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(defaulted);

            SECTION("LHS == RHS") {
                ElectronicHamiltonian rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("LHS != RHS") { REQUIRE(lhs != hash_objects(from_ham)); }
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

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default.size() == 3);
    }
}
