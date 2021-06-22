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
 * publically accessible functionality.
 */

using namespace libchemist::orbital_space;

TEST_CASE("DependentSpace") {
    using sparse_map_type = type::sparse_map;
    using index_type      = typename sparse_map_type::key_type;
    using value_type      = typename sparse_map_type::mapped_type;

    SECTION("Typedefs") {
        SECTION("size_type") {
            using type = typename DependentSpace::size_type;
            using corr = typename BaseSpace::size_type;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }

        SECTION("sparse_map_type") {
            using type = typename DependentSpace::sparse_map_type;
            using corr = type::sparse_map;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
    }

    DepAOSpaceD defaulted;
    SECTION("Default ctor") {
        REQUIRE(defaulted.sparse_map() == sparse_map_type{});
    }

    sparse_map_type sm0{index_type{0}, value_type{index_type{0}}};
    DepAOSpaceD nondefault(sm0);

    SECTION("Value ctor") { REQUIRE(nondefault.sparse_map() == sm0); }
}

//         SECTION("Copy") {
//             base_space bs(sm2, S);
//             REQUIRE(bs == base_space(bs));
//         }

//         SECTION("Move") {
//             base_space bs(sm2, S), bs2(sm2, S);
//             base_space bs3(std::move(bs));
//             REQUIRE(bs2 == bs3);
//         }

//         SECTION("Copy Assignment") {
//             base_space bs(sm2, S), bs2;
//             auto pbs2 = & (bs2 = bs);
//             SECTION("Value") { REQUIRE(bs == bs2); }
//             SECTION("Returns this") { REQUIRE(pbs2 == &bs2); }
//         }

//         SECTION("Move Assignment") {
//             base_space bs(sm2, S), bs2(sm2, S), bs3;
//             auto pbs3 = &(bs3 = std::move(bs));
//             SECTION("Value") { REQUIRE(bs2 == bs3); }
//             SECTION("Returns this") { REQUIRE(pbs3 == &bs3); }
//         }
//     }

//     base_space bs1(sm1, S), bs2(sm2, S2);

//     SECTION("sparse_map()")  {
//         REQUIRE(bs1.sparse_map() == sm1);
//     }

//     SECTION("sparse_map() const") {
//         REQUIRE(std::as_const(bs1).sparse_map() == sm1);
//     }

//     SECTION("S()") {
//         REQUIRE(libchemist::ta_helpers::allclose_tot(bs1.S(), S,
//         2));
//     }

//     SECTION("S() const") {
//         REQUIRE(libchemist::ta_helpers::allclose_tot(std::as_const(bs1).S(),
//         S, 2));
//     }

//     SECTION("Hash") {
//         SECTION("Same state") {
//             base_space bs3(sm1, S);
//             REQUIRE(sde::hash_objects(bs1) ==
//             sde::hash_objects(bs3));
//         }

//         SECTION("Different sparse map") {
//             base_space bs3(sm2, S);
//             REQUIRE(sde::hash_objects(bs1) !=
//             sde::hash_objects(bs3));
//         }

//         SECTION("Different overlap matrix") {
//             base_space bs3(sm1, S2);
//             REQUIRE(sde::hash_objects(bs1) !=
//             sde::hash_objects(bs3));
//         }
//     }

//     SECTION("Comparison") {
//         SECTION("Same state") {
//             base_space bs3(sm1, S);
//             REQUIRE(bs1 == bs3);
//             REQUIRE_FALSE(bs1 != bs3);
//         }

//         SECTION("Different sparse map") {
//             base_space bs3(sm2, S);
//             REQUIRE_FALSE(bs1 == bs3);
//             REQUIRE(bs1 != bs3);
//         }

//         SECTION("Different overlap matrix") {
//             base_space bs3(sm1, S2);
//             REQUIRE_FALSE(bs1 == bs3);
//             REQUIRE(bs1 != bs3);
//         }
//     }
// }
