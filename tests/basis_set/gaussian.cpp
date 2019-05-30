#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr Gaussian<double>") {
    constexpr std::array r0{1.1, 2.2, 3.3};
    constexpr Gaussian<double> g(2.34, 3.45, r0);
    STATIC_REQUIRE(g.coefficient() == 2.34);
    STATIC_REQUIRE(g.exponent() == 3.45);
    STATIC_REQUIRE(g.center()[0] == r0[0]);
    STATIC_REQUIRE(g.center()[1] == r0[1]);
    STATIC_REQUIRE(g.center()[2] == r0[2]);
    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr Gaussian<double> g2(2.34, 3.45, r0);
            STATIC_REQUIRE(g == g2);
            STATIC_REQUIRE_FALSE(g != g2);
        }
        SECTION("Different coefficients") {
            constexpr Gaussian<double> g2(1.11, 3.45, r0);
            STATIC_REQUIRE(g != g2);
            STATIC_REQUIRE_FALSE(g == g2);
        }
        SECTION("Different exponents") {
            constexpr Gaussian<double> g2(2.34, 1.11, r0);
            STATIC_REQUIRE(g != g2);
            STATIC_REQUIRE_FALSE(g == g2);
        }
        SECTION("Different centers") {
            constexpr std::array r1{0.0, 0.0, 0.0};
            constexpr Gaussian<double> g2(2.34, 3.45, r1);
            STATIC_REQUIRE(g != g2);
            STATIC_REQUIRE_FALSE(g == g2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr Gaussian<double> g2(g);
        STATIC_REQUIRE(g2 == g);
    }
    SECTION("Move Ctor") {
        constexpr Gaussian<double> g2(g);
        constexpr Gaussian<double> g3(std::move(g));
        STATIC_REQUIRE(g2 == g3);
    }
}
