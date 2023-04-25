#include <catch2/catch.hpp>
#include <chemist/point/point_set.hpp>

using namespace chemist;

TEMPLATE_TEST_CASE("PointSet", "", float, double) {
    using set_type = PointSet<TestType>;

    set_type defaulted;

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }
    }
}
