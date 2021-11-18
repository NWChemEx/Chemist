#include "libchemist/tensor/allocators/allocators.hpp"
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
    using field_type   = libchemist::tensor::field::Scalar;
    using alloc_type   = OneBigTile<field_type>;
    using extents_type = typename alloc_type::extents_type;
    using tr_type      = typename alloc_type::tiled_range_type;

    alloc_type a;

    extents_type shape{3, 4, 5};
    tr_type corr{{0, 3}, {0, 4}, {0, 5}};

    SECTION("make_tiled_range") { REQUIRE(a.make_tiled_range(shape) == corr); }

    SECTION("clone") {
        auto pb = a.clone();
        REQUIRE(pb->make_tiled_range(shape) == corr);
    }

    SECTION("is_equal") {
        SECTION("Both OneBigTile") {
            alloc_type b;
            REQUIRE(a.is_equal(b));
            REQUIRE(b.is_equal(a));
        }

        SECTION("Different derived types") {
            SingleElementTiles<field_type> c;
            REQUIRE_FALSE(c.is_equal(a));
            REQUIRE_FALSE(a.is_equal(c));
        }
    }
}
