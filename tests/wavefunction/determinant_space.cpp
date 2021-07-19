#include "libchemist/wavefunction/determinant_space.hpp"
#include "test_wavefunction.hpp"

using namespace libchemist::wavefunction;

using tuple_type = std::tuple<DeterminantD, CanonicalDeterminantD,
                              LocalDeterminantD, CanonicalLocalDeterminantD>;

/* Testing strategy:
 *
 * We assume that the orbital space classes are thoroughly tested and work as
 * expected. Ditto for the FockOperator class. Thus with respect to the values
 * of the members the only thing that matters to this class is we can get and
 * set them and that they are taken into account in comparisons.
 */

TEMPLATE_LIST_TEST_CASE("DeterminantSpace", "", tuple_type) {
    using space_t       = TestType;
    using occ_space_t   = typename TestType::occupied_orbital_type;
    using virt_space_t  = typename TestType::virtual_orbital_type;
    using fock_op_t     = typename TestType::fock_operator_type;
    using occ_tensor_t  = typename occ_space_t::transform_type;
    using virt_tensor_t = typename virt_space_t::transform_type;

    auto& world = TA::get_default_world();

    // A default DeterminantSpace
    space_t defaulted;

    // Makes a non-default DeterminantSpace
    auto occ  = testing::make_space<occ_space_t>(1.0);
    auto virt = testing::make_space<virt_space_t>(2.0);
    fock_op_t fock(libchemist::ElectronKinetic{});
    space_t nondefault(occ, virt, fock);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.occupied_orbitals() == occ_space_t{});
            REQUIRE(defaulted.virtual_orbitals() == virt_space_t{});
            REQUIRE(defaulted.fock_operator() == fock_op_t{});
        }

        SECTION("value") {
            REQUIRE(nondefault.occupied_orbitals() == occ);
            REQUIRE(nondefault.virtual_orbitals() == virt);
            REQUIRE(nondefault.fock_operator() == fock);
        }

        SECTION("copy") {
            space_t copy(nondefault);
            REQUIRE(copy.occupied_orbitals() == occ);
            REQUIRE(copy.virtual_orbitals() == virt);
            REQUIRE(copy.fock_operator() == fock);
        }

        SECTION("move") {
            space_t moved(std::move(nondefault));
            REQUIRE(moved.occupied_orbitals() == occ);
            REQUIRE(moved.virtual_orbitals() == virt);
            REQUIRE(moved.fock_operator() == fock);
        }

        SECTION("copy assignment") {
            space_t copy;
            auto pcopy = &(copy = nondefault);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.occupied_orbitals() == occ);
            REQUIRE(copy.virtual_orbitals() == virt);
            REQUIRE(copy.fock_operator() == fock);
        }

        SECTION("move assignment") {
            space_t moved;
            auto pmoved = &(moved = nondefault);
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.occupied_orbitals() == occ);
            REQUIRE(moved.virtual_orbitals() == virt);
            REQUIRE(moved.fock_operator() == fock);
        }
    }

    SECTION("occupied_orbitals") {
        REQUIRE(defaulted.occupied_orbitals() == occ_space_t{});
        REQUIRE(nondefault.occupied_orbitals() == occ);
    }

    SECTION("virtual_orbitals") {
        REQUIRE(defaulted.virtual_orbitals() == virt_space_t{});
        REQUIRE(nondefault.virtual_orbitals() == virt);
    }

    SECTION("fock_operator") {
        REQUIRE(defaulted.fock_operator() == fock_op_t{});
        REQUIRE(nondefault.fock_operator() == fock);
    }

    // SECTION("hash") {
    //     SECTION("LHS is default") {
    //         const auto lhs = sde::hash_objects(defaulted);
    //         SECTION("Same value") {
    //             REQUIRE(lhs == sde::hash_objects(space_t{}));
    //         }
    //         SECTION("Different occupied") {
    //             space_t rhs(occ, virt_space_t{}, fock_op_t{});
    //             REQUIRE(lhs != sde::hash_objects(rhs));
    //         }
    //         SECTION("Different virtual") {
    //             space_t rhs(occ_space_t{}, virt, fock_op_t{});
    //             REQUIRE(lhs != sde::hash_objects(rhs));
    //         }
    //         SECTION("Different fock operators") {
    //             space_t rhs(occ_space_t{}, virt_space_t{}, fock);
    //             REQUIRE(lhs != sde::hash_objects(rhs));
    //         }
    //     }
    // }

    SECTION("comparisons") {
        SECTION("LHS is default") {
            SECTION("Same value") {
                REQUIRE(defaulted == space_t{});
                REQUIRE_FALSE(defaulted != space_t{});
            }
            SECTION("Different occupied") {
                space_t rhs(occ, virt_space_t{}, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different virtual") {
                space_t rhs(occ_space_t{}, virt, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different fock operators") {
                space_t rhs(occ_space_t{}, virt_space_t{}, fock);
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
        }
    }
}
