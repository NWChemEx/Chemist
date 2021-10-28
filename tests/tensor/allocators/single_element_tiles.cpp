#include "libchemist/tensor/allocators/single_element_tiles.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

/* Testing strategy:
 *
 * SingleElementTiles has no state. So there's not much to test aside from
 * ensuring clone makes a polymorphic copy (verified by calling
 * make_tiled_range) and that make_tiled_range works as expected.
 *
 */
TEST_CASE("SingleElementTiles") {
    using extents_type = typename SingleElementTiles::extents_type;
    using tr_type      = typename SingleElementTiles::tiled_range_type;

    SingleElementTiles a;

    extents_type shape{3, 4, 5};
    tr_type corr{{0, 1, 2, 3}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4, 5}};

    SECTION("make_tiled_range") { REQUIRE(a.make_tiled_range(shape) == corr); }

    SECTION("clone") {
        auto pb = a.clone();
        REQUIRE(pb->make_tiled_range(shape) == corr);
    }
}
