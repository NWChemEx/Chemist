#include "chemist/orbital_space/orbital_space.hpp"
#include "chemist/types.hpp"
#include "test_orbital_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::orbital_space;

/* Testing strategy:
 *
 * We assume that the object his class derives from is tested and works
 * correctly. The tests focus on adding sparse maps and differentiating them.
 */

TEST_CASE("IndependentSpace") {
    using space_type      = CanonicalIndSpace;
    using sparse_map_type = typename space_type::sparse_map_type;
    using index_type      = typename sparse_map_type::key_type;

    SECTION("Typedefs") {
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr      = typename BaseSpace::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }

        SECTION("sparse_map_type") {
            using corr = type::sparse_map;
            STATIC_REQUIRE(std::is_same_v<sparse_map_type, corr>);
        }

        SECTION("sparse_map_register") {
            using corr = std::map<std::string, sparse_map_type>;
            using sparse_map_register =
              typename space_type::sparse_map_register;
            STATIC_REQUIRE(std::is_same_v<corr, sparse_map_register>);
        }
    }

    // Declare two spaces so we can have a sparsity relation
    auto space1 = testing::get_canonical();
    decltype(space1) space2(space1.orbital_energies());

    // We'll let space1 be the independent space
    space_type no_sms(space1);
    space_type has_sms(space1);

    // Here we declare two sparse maps
    sparse_map_type sm0({{index_type{0}, {index_type{0}}}});
    sparse_map_type sm1;
    has_sms.add_sparse_map(space1, sm0);
    has_sms.add_sparse_map(space2, sm1);

    SECTION("count") {
        REQUIRE(no_sms.count(space1) == 0);
        REQUIRE(no_sms.count(space2) == 0);

        REQUIRE(has_sms.count(space1) == 1);
#ifdef BPHASH_USE_TYPEID
        REQUIRE(has_sms.count(has_sms) == 0);
#else
        REQUIRE(has_sms.count(has_sms) == 1);
#endif
        REQUIRE(has_sms.count(space2) == 1);
    }

    SECTION("sparse_map") {
        REQUIRE(has_sms.sparse_map(space1) == sm0);
#ifndef BPHASH_USE_TYPEID
        REQUIRE(has_sms.sparse_map(has_sms) == sm0);
#endif
        REQUIRE(has_sms.sparse_map(space2) == sm1);
    }
}
