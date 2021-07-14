#include "libchemist/orbital_space/type_traits.hpp"
#include "libchemist/wavefunction/determinant_space.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::wavefunction;

using tuple_type = std::tuple<DeterminantD, CanonicalDeterminantD,
                              LocalDeterminantD, CanonicalLocalDeterminantD>;

namespace {

// Makes dummy tensors
template<typename TensorType>
auto make_tensor(double seed = 1.0) {
    using value_type = typename TensorType::value_type;

    auto& world = TA::get_default_world();
    if constexpr(TA::detail::is_tensor_of_tensor_v<value_type>) {
        using inner_type = typename value_type::value_type;
        inner_type inner(TA::Range{2, 1}, {seed, seed + 1.0});
        return TensorType(world, {inner, inner});
    } else {
        return TensorType(world, {seed, seed + 1.0});
    }
}

// Makes dummy spaces
template<typename space>
auto make_space(double seed = 1.0) {
    using transform_type  = typename space::transform_type;
    using from_space_type = typename space::from_space_type;

    if constexpr(libchemist::orbital_space::is_canonical_space_v<space>) {
        return space(make_tensor<transform_type>(seed));
    } else {
        return space(make_tensor<transform_type>(seed), from_space_type{});
    }
}
} // namespace

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
    auto occ  = make_space<occ_space_t>(1.0);
    auto virt = make_space<virt_space_t>(2.0);
    space_t nondefault(occ, virt);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.occupied_orbitals() == occ_space_t{});
            REQUIRE(defaulted.virtual_orbitals() == virt_space_t{});
            // REQUIRE(defaulted.fock_operator() == fock_op_t{});
        }

        SECTION("value") {
            REQUIRE(nondefault.occupied_orbitals() == occ);
            REQUIRE(nondefault.virtual_orbitals() == virt);
        }

        SECTION("copy") {
            space_t copy(nondefault);
            REQUIRE(copy.occupied_orbitals() == occ);
            REQUIRE(copy.virtual_orbitals() == virt);
        }

        SECTION("move") {
            space_t moved(std::move(nondefault));
            REQUIRE(moved.occupied_orbitals() == occ);
            REQUIRE(moved.virtual_orbitals() == virt);
        }

        SECTION("copy assignment") {
            space_t copy;
            auto pcopy = &(copy = nondefault);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.occupied_orbitals() == occ);
            REQUIRE(copy.virtual_orbitals() == virt);
        }

        SECTION("move assignment") {
            space_t moved;
            auto pmoved = &(moved = nondefault);
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.occupied_orbitals() == occ);
            REQUIRE(moved.virtual_orbitals() == virt);
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

    SECTION("hash") {
        SECTION("LHS is default") {
            const auto lhs = sde::hash_objects(defaulted);
            SECTION("Same value") {
                REQUIRE(lhs == sde::hash_objects(space_t{}));
            }
            SECTION("Different occupied") {
                space_t rhs(occ, virt_space_t{});
                REQUIRE(lhs != sde::hash_objects(rhs));
            }
            SECTION("Different virtual") {
                space_t rhs(occ_space_t{}, virt);
                REQUIRE(lhs != sde::hash_objects(rhs));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS is default") {
            SECTION("Same value") {
                REQUIRE(defaulted == space_t{});
                REQUIRE_FALSE(defaulted != space_t{});
            }
            SECTION("Different occupied") {
                space_t rhs(occ, virt_space_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different virtual") {
                space_t rhs(occ_space_t{}, virt);
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
        }
    }
}
