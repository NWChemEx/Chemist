#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/dependent_space.hpp"
#include <catch2/catch.hpp>

/* Testing Strategy:
 *
 * DependentSpace is an abstract class because it still does not implement the
 * transform method from BaseSpace. Compared to BaseSpace DependentSpace adds:
 * - sparse_map_type
 * - a sparse map to the state
 * - implementations for size, hash, and equal
 *   - These implementations are only accessible from derived classes
 * - operator== / operator!=
 *
 * Like BaseSpace we test by making an instance of AOSpace and testing the
 * publically accessible functionality added by the DependentSpace class.
 */

using namespace libchemist::orbital_space;

TEST_CASE("DependentSpace") {
    using sparse_map_type = type::sparse_map;
    using index_type      = typename sparse_map_type::key_type;
    using basis_type      = typename AOSpaceD::basis_type;
    using center_type     = typename basis_type::value_type;

    SECTION("Typedefs") {
        SECTION("size_type") {
            using type = typename DependentSpace::size_type;
            using corr = typename BaseSpace::size_type;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }

        SECTION("sparse_map_type") {
            using corr = type::sparse_map;
            using type = typename DependentSpace::sparse_map_type;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
    }

    DepAOSpaceD defaulted;
    SECTION("Default ctor") {
        REQUIRE(defaulted.sparse_map() == sparse_map_type{});
    }

    sparse_map_type sm0{{index_type{0}, {index_type{0}}}};
    DepAOSpaceD nondefault_sm(basis_type{}, sm0);

    SECTION("Value ctor") { REQUIRE(nondefault_sm.sparse_map() == sm0); }

    SECTION("Copy ctor") {
        DepAOSpaceD copy(nondefault_sm);
        REQUIRE(copy.sparse_map() == sm0);
    }

    SECTION("Move ctor") {
        DepAOSpaceD moved(std::move(nondefault_sm));
        REQUIRE(moved.sparse_map() == sm0);
    }

    SECTION("Copy assignment") {
        DepAOSpaceD copy;
        auto pcopy = &(copy = nondefault_sm);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.sparse_map() == sm0);
    }

    SECTION("Move assignment") {
        DepAOSpaceD moved;
        auto pmove = &(moved = std::move(nondefault_sm));
        REQUIRE(pmove == &moved);
        REQUIRE(moved.sparse_map() == sm0);
    }

    SECTION("sparse_map") { REQUIRE(nondefault_sm.sparse_map() == sm0); }

    SECTION("sparse_map const") {
        const DependentSpace& const_nondefault_sm = nondefault_sm;
        REQUIRE(const_nondefault_sm.sparse_map() == sm0);
    }

    SECTION("hash") {
        SECTION("Same") {
            DepAOSpaceD copy(nondefault_sm);
            REQUIRE(pluginplay::hash_objects(nondefault_sm) ==
                    pluginplay::hash_objects(copy));
        }

        SECTION("Different") {
            DepAOSpaceD copy(nondefault_sm);
            REQUIRE(pluginplay::hash_objects(defaulted) !=
                    pluginplay::hash_objects(nondefault_sm));
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == Default") {
            const DependentSpace& lhs = defaulted;
            SECTION("RHS == default") {
                DepAOSpaceD other;
                const DependentSpace& rhs = other;
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }

            SECTION("RHS has different sparse map") {
                const DependentSpace& rhs = nondefault_sm;
                REQUIRE_FALSE(lhs == rhs);
                REQUIRE(lhs != rhs);
            }

            SECTION("RHS has different derived state") {
                basis_type non_empty;
                non_empty.add_center(center_type{});
                DepAOSpaceD nondefault_bs(non_empty);
                const DependentSpace& rhs = nondefault_bs;
                REQUIRE(lhs == rhs);
                REQUIRE_FALSE(lhs != rhs);
            }
        }
    }
}
