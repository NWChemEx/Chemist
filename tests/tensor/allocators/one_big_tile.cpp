#include "libchemist/tensor/allocators/one_big_tile.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

/* Testing strategy:
 *
 * OneBigTile has no state. So there's not much to test aside from
 * ensuring clone makes a polymorphic copy (verified by calling
 * make_tiled_range) and that make_tiled_range works as expected.
 *
 */
TEST_CASE("OneBigTile") {
    using extents_type = typename OneBigTile::extents_type;
    using tr_type      = typename OneBigTile::tiled_range_type;

    OneBigTile a;

    extents_type shape{3, 4, 5};
    tr_type corr{{0, 3}, {0, 4}, {0, 5}};

    SECTION("make_tiled_range") { REQUIRE(a.make_tiled_range(shape) == corr); }

    SECTION("clone") {
        auto pb = a.clone();
        REQUIRE(pb->make_tiled_range(shape) == corr);
    }
}
