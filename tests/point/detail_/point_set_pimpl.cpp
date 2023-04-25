#include <catch2/catch.hpp>
#include <chemist/point/detail_/point_set_pimpl.hpp>

using namespace chemist;
using namespace chemist::detail_;

TEMPLATE_TEST_CASE("PointSetPIMPL", "", float, double) {
    using pimpl_type = PointSetPIMPL<TestType>;
    using value_type = typename pimpl_type::value_type;

    value_type p0(1.0, 2.0, 3.0);
    value_type p1(4.0, 5.0, 6.0);

    pimpl_type defaulted, points;
    points.push_back(p0);
    points.push_back(p1);

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }
        SECTION("copy") {
            value_type p2(p1);
            REQUIRE(p1 == p2);
        }
    }

    SECTION("operator[]") {
        REQUIRE(points[0] == p0);
        REQUIRE(points[1] == p1);
    }

    SECTION("operator[] const") {
        REQUIRE(std::as_const(points)[0] == p0);
        REQUIRE(std::as_const(points)[1] == p1);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(points.size() == 2);
    }

    SECTION("operator==") {
        // Default == default
        REQUIRE(defaulted == pimpl_type());

        // Default != non-default
        REQUIRE_FALSE(defaulted == points);

        SECTION("non-default == non-default") {
            pimpl_type other;
            other.push_back(p0);
            other.push_back(p1);
            REQUIRE(points == other);
        }

        SECTION("different order") {
            pimpl_type other;
            other.push_back(p1);
            other.push_back(p0);
            REQUIRE_FALSE(points == other);
        }
    }
}
