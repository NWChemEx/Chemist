#include "libchemist/wavefunction/nonrelativistic.hpp"
#include "test_wavefunction.hpp"

using namespace libchemist::wavefunction;

using tuple_type = std::tuple<Reference, CanonicalReference, LocalReference,
                              CanonicalLocalReference>;

TEMPLATE_LIST_TEST_CASE("Nonrelativistic", "", tuple_type) {
    using wf_t         = TestType;
    using basis_set_t  = typename wf_t::basis_set_type;
    using occ_space_t  = typename basis_set_t::occupied_orbital_type;
    using virt_space_t = typename basis_set_t::virtual_orbital_type;
    using fock_t       = typename basis_set_t::fock_operator_type;

    wf_t defaulted;

    auto occ  = testing::make_space<occ_space_t>();
    auto virt = testing::make_space<virt_space_t>(2);
    basis_set_t space(occ, virt, fock_t{});
    wf_t non_default_space(space);
    wf_t non_default(space, 1.0);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.spin() == 0);
            REQUIRE(defaulted.basis_set() == basis_set_t{});
        }

        SECTION("Value") {
            REQUIRE(non_default_space.spin() == 0);
            REQUIRE(non_default_space.basis_set() == space);

            REQUIRE(non_default.spin() == 1);
            REQUIRE(non_default.basis_set() == space);
        }

        SECTION("Copy") {
            wf_t copy(non_default);
            REQUIRE(copy.spin() == 1);
            REQUIRE(copy.basis_set() == space);
        }

        SECTION("Move") {
            wf_t moved(std::move(non_default));
            REQUIRE(moved.spin() == 1);
            REQUIRE(moved.basis_set() == space);
        }

        SECTION("Copy assignment") {
            wf_t copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.spin() == 1);
            REQUIRE(copy.basis_set() == space);
        }

        SECTION("Move assignment") {
            wf_t moved;
            auto pmoved = &(moved = std::move(non_default));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.spin() == 1);
            REQUIRE(moved.basis_set() == space);
        }
    }

    SECTION("multiplicity") {
        REQUIRE(defaulted.multiplicity() == 1);
        REQUIRE(non_default_space.multiplicity() == 1);
        REQUIRE(non_default.multiplicity() == 3);
    }

    SECTION("spin") {
        REQUIRE(defaulted.spin() == 0);
        REQUIRE(non_default_space.spin() == 0);
        REQUIRE(non_default.spin() == 1);
    }

    SECTION("basis_set") {
        REQUIRE(defaulted.basis_set() == basis_set_t{});
        REQUIRE(non_default_space.basis_set() == space);
        REQUIRE(non_default.basis_set() == space);
    }

    // SECTION("hash") {
    //     SECTION("LHS == default") {
    //         auto lhs = pluginplay::hash_objects(defaulted);

    //         SECTION("RHS == default") {
    //             wf_t rhs;
    //             REQUIRE(lhs == pluginplay::hash_objects(rhs));
    //         }

    //         SECTION("different space") {
    //             REQUIRE(lhs != pluginplay::hash_objects(non_default_space));
    //         }

    //         SECTION("different spin") {
    //             wf_t rhs(basis_set_t{}, 1);
    //             REQUIRE(lhs != pluginplay::hash_objects(rhs));
    //         }
    //     }
    // }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("RHS == default") {
                wf_t rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("different space") {
                REQUIRE(defaulted != non_default_space);
                REQUIRE_FALSE(defaulted == non_default_space);
            }

            SECTION("different spin") {
                wf_t rhs(basis_set_t{}, 1);
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
        }
    }

    SECTION("make_wavefunction") {
        SECTION("Default spin") {
            auto wf = make_wavefunction(occ, virt, fock_t{});
            REQUIRE(wf == non_default_space);
        }

        SECTION("Non-default spin") {
            auto wf = make_wavefunction(occ, virt, fock_t{}, 1);
            REQUIRE(wf == non_default);
        }
    }
}
