#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr Gaussian<double>()") {
    constexpr Gaussian<double> g;
    STATIC_REQUIRE(g.coefficient() == 0.0);
    STATIC_REQUIRE(g.exponent() == 0.0);
    STATIC_REQUIRE_FALSE(g.has_center());
}

TEST_CASE("constexpr Gaussian<double>(c, a)") {
    constexpr Gaussian<double> g(1.23, 4.56);
    STATIC_REQUIRE(g.coefficient() == 1.23);
    STATIC_REQUIRE(g.exponent() == 4.56);
    STATIC_REQUIRE_FALSE(g.has_center());
    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr Gaussian<double> g2(1.23, 4.56);
            STATIC_REQUIRE(g == g2);
            STATIC_REQUIRE_FALSE(g != g2);
        }
        SECTION("Different coefficent") {
            constexpr Gaussian<double> g2(7.89, 4.56);
            STATIC_REQUIRE(g != g2);
        }
        SECTION("Different exponents") {
            constexpr Gaussian<double> g2(1.23, 7.89);
            STATIC_REQUIRE(g != g2);
        }
        SECTION("Different having r0") {
            constexpr std::array r0{1.1, 2.2, 3.3};
            constexpr Gaussian<double> g2(1.23, 4.56, r0);
            STATIC_REQUIRE(g != g2);
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

TEST_CASE("constexpr Gaussian<double>(c, a, r)") {
    constexpr std::array r0{1.1, 2.2, 3.3};
    constexpr Gaussian<double> g(1.23, 4.56, r0);
    STATIC_REQUIRE(g.coefficient() == 1.23);
    STATIC_REQUIRE(g.exponent() == 4.56);
    STATIC_REQUIRE(g.has_center());
    STATIC_REQUIRE(g.center()[0] == r0[0]);
    STATIC_REQUIRE(g.center()[1] == r0[1]);
    STATIC_REQUIRE(g.center()[2] == r0[2]);
    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr Gaussian<double> g2(1.23, 4.56, r0);
            STATIC_REQUIRE(g == g2);
            STATIC_REQUIRE_FALSE(g != g2);
        }
        SECTION("Different coefficent") {
            constexpr Gaussian<double> g2(7.89, 4.56, r0);
            STATIC_REQUIRE(g != g2);
        }
        SECTION("Different exponents") {
            constexpr Gaussian<double> g2(1.23, 7.89, r0);
            STATIC_REQUIRE(g != g2);
        }
        SECTION("Different r0") {
            constexpr std::array r1{4.4, 5.5, 6.6};
            constexpr Gaussian<double> g2(1.23, 4.56, r1);
            STATIC_REQUIRE(g != g2);
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

TEST_CASE("Gaussian<double>(c, a)") {
    Gaussian<double> g(1.23, 4.56);
    SECTION("Can change coefficient") {
        constexpr Gaussian<double> g2(7.89, 4.56);
        g.coefficient() = 7.89;
        REQUIRE(g == g2);
    }
    SECTION("Can change exponent") {
        constexpr Gaussian<double> g2(1.23, 7.89);
        g.exponent() = 7.89;
        REQUIRE(g == g2);
    }
    SECTION("Can add a center") {
        constexpr std::array r0{1.1, 2.2, 3.3};
        constexpr Gaussian<double> g2(1.23, 4.56, r0);
        g.set_center(r0);
        REQUIRE(g == g2);
    }
    SECTION("Getting the center throws if there is no center") {
        REQUIRE_THROWS_AS(g.center(), std::bad_optional_access);
    }
    SECTION("Copy assignment") {
        Gaussian<double> g2;
        auto pg2 = &(g2 = g);
        REQUIRE(pg2 == &g2);
        REQUIRE(g2 == g);
    }
    SECTION("Move assignment") {
        Gaussian<double> g2;
        Gaussian<double> g3(g);
        auto pg2 = &(g2 = std::move(g));
        REQUIRE(pg2 == &g2);
        REQUIRE(g2 == g3);
    }
}
