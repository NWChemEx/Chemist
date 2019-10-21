#include <catch2/catch.hpp>
#include <libchemist/basis_set/center/center.hpp>

using namespace libchemist;

TEST_CASE("Center : default ctor") {
    Center<double> c;
    SECTION("Size") { REQUIRE(c.size() == 0); }
}

TEST_CASE("Center : x, y, z ctor") {
    Center<double> c(1.0, 2.0, 3.0);
    SECTION("State") {
        SECTION("Size") { REQUIRE(c.size() == 0); }
        SECTION("Coordinates") {
            REQUIRE(c.x() == 1.0);
            REQUIRE(c.y() == 2.0);
            REQUIRE(c.z() == 3.0);
        }
    }
}

TEST_CASE("Center : add_shell") {
    Center<double> c(1.0, 2.0, 3.0);
    std::vector<double> cs{4.0};
    std::vector<double> es{5.0};
    c.add_shell(true, 2, cs, es);
    SECTION("State") {
        SECTION("Size") { REQUIRE(c.size() == 1); }
        SECTION("Shells") {
            REQUIRE(c[0] == Shell<double>(true, 2, cs, es, 1.0, 2.0, 3.0));
        }
    }
}
