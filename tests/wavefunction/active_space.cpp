#include "chemist/operators/kinetic.hpp"
#include "chemist/operators/multipole.hpp"
#include "chemist/wavefunction/active_space.hpp"
#include "test_wavefunction.hpp"

using namespace chemist::wavefunction;
using namespace chemist::orbital_space;

using tuple_type =
  std::tuple<DerivedActiveSpace>;

/* Testing strategy:
 *
 * We assume that the orbital space classes are thoroughly tested and work as
 * expected. Ditto for the FockOperator class. Thus with respect to the values
 * of the members the only thing that matters to this class is we can get and
 * set them and that they are taken into account in comparisons.
 */

TEMPLATE_LIST_TEST_CASE("ActiveSpace", "", tuple_type) {
    using space_t         = TestType;
    using inact_space_t   = typename TestType::inactive_orbital_type;
    using act_space_t     = typename TestType::active_orbital_type;
    using virt_space_t    = typename TestType::virtual_orbital_type;
    using core_space_t    = typename TestType::core_orbital_type;
    using fock_op_t       = typename TestType::fock_operator_type;
    using inact_tensor_t  = typename inact_space_t::transform_type;
    using act_tensor_t    = typename act_space_t::transform_type;
    using virt_tensor_t   = typename virt_space_t::transform_type;
    using core_tensor_t   = typename core_space_t::transform_type;

    auto& world = TA::get_default_world();

    // A default ActiveSpace
    space_t defaulted;

    // Makes a non-default ActiveSpace
    auto inact  = testing::make_space<inact_space_t>(1.0);
    auto act    = testing::make_space<act_space_t>(1.2);
    auto virt   = testing::make_space<virt_space_t>(2.0);
    auto core   = testing::make_space<core_space_t>(3.5);
    fock_op_t inact_fock(chemist::operators::ElectronKinetic{});
    fock_op_t act_fock(chemist::operators::ElectricDipole{});
    space_t nondefault(inact, act, virt, core, inact_fock, act_fock);

    SECTION("CTors") {
        SECTION("value") {
            REQUIRE(nondefault.inactive_orbitals() == inact);
            REQUIRE(nondefault.active_orbitals() == act);
            REQUIRE(nondefault.virtual_orbitals() == virt);
            REQUIRE(nondefault.core_orbitals() == core);
            REQUIRE(nondefault.inactive_fock_operator() == inact_fock);
            REQUIRE(nondefault.active_fock_operator() == act_fock);
        }

        SECTION("copy") {
            space_t copy(nondefault);
            REQUIRE(copy.inactive_orbitals() == inact);
            REQUIRE(copy.active_orbitals() == act);
            REQUIRE(copy.virtual_orbitals() == virt);
            REQUIRE(copy.core_orbitals() == core);
            REQUIRE(copy.inactive_fock_operator() == inact_fock);
            REQUIRE(copy.active_fock_operator() == act_fock);
        }

        SECTION("move") {
            space_t moved(std::move(nondefault));
            REQUIRE(moved.inactive_orbitals() == inact);
            REQUIRE(moved.active_orbitals() == act);
            REQUIRE(moved.virtual_orbitals() == virt);
            REQUIRE(moved.core_orbitals() == core);
            REQUIRE(moved.inactive_fock_operator() == inact_fock);
            REQUIRE(moved.active_fock_operator() == act_fock);
        }

        SECTION("copy assignment") {
            space_t copy;
            auto pcopy = &(copy = nondefault);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.inactive_orbitals() == inact);
            REQUIRE(copy.active_orbitals() == act);
            REQUIRE(copy.virtual_orbitals() == virt);
            REQUIRE(copy.core_orbitals() == core);
            REQUIRE(copy.inactive_fock_operator() == inact_fock);
            REQUIRE(copy.active_fock_operator() == act_fock);
        }

        SECTION("move assignment") {
            space_t moved;
            auto pmoved = &(moved = nondefault);
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.inactive_orbitals() == inact);
            REQUIRE(moved.active_orbitals() == act);
            REQUIRE(moved.virtual_orbitals() == virt);
            REQUIRE(moved.core_orbitals() == core);
            REQUIRE(moved.inactive_fock_operator() == inact_fock);
            REQUIRE(moved.active_fock_operator() == act_fock);
        }
    }

    SECTION("inactive_orbitals") {
        REQUIRE_THROWS_AS(defaulted.inactive_orbitals(), std::runtime_error);
        REQUIRE(nondefault.inactive_orbitals() == inact);
    }

    SECTION("active_orbitals") {
        REQUIRE_THROWS_AS(defaulted.active_orbitals(), std::runtime_error);
        REQUIRE(nondefault.active_orbitals() == act);
    }

    SECTION("virtual_orbitals") {
        REQUIRE_THROWS_AS(defaulted.virtual_orbitals(), std::runtime_error);
        REQUIRE(nondefault.virtual_orbitals() == virt);
    }

    SECTION("core_orbitals") {
        REQUIRE_THROWS_AS(defaulted.core_orbitals(), std::runtime_error);
        REQUIRE(nondefault.core_orbitals() == core);
    }

    SECTION("inactive_fock_operator") {
        REQUIRE_THROWS_AS(defaulted.inactive_fock_operator(), std::runtime_error);
        REQUIRE(nondefault.inactive_fock_operator() == inact_fock);
    }

    SECTION("active_fock_operator") {
        REQUIRE_THROWS_AS(defaulted.active_fock_operator(), std::runtime_error);
        REQUIRE(nondefault.active_fock_operator() == act_fock);
    }


    SECTION("comparisons") {
        SECTION("LHS is default") {
            SECTION("Same value") {
                REQUIRE(defaulted == space_t{});
                REQUIRE_FALSE(defaulted != space_t{});
            }
            SECTION("Different inactive") {
                space_t rhs(inact, act_space_t{},  virt_space_t{}, core_space_t{}, fock_op_t{}, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different active") {
                space_t rhs(inact_space_t{}, act,  virt_space_t{}, core_space_t{}, fock_op_t{}, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different virtual") {
                space_t rhs(inact_space_t{}, act_space_t{}, virt, core_space_t{}, fock_op_t{}, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different core") {
                space_t rhs(inact_space_t{}, act_space_t{}, virt_space_t{}, core, fock_op_t{}, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different inactive fock operators") {
                space_t rhs(inact_space_t{}, act_space_t{}, virt_space_t{}, core_space_t{}, inact_fock, fock_op_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different active fock operators") {
                space_t rhs(inact_space_t{}, act_space_t{}, virt_space_t{}, core_space_t{}, fock_op_t{}, act_fock);
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
        }
    }
}

