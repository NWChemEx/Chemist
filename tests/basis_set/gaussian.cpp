#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr Gaussian") {
    constexpr libchemist::type::coord<double> r0{1.23, 4.56, 7.89};
    constexpr double a = 2.34;
    constexpr double c = 5.67;
    constexpr auto g   = make_gaussian(c, a, r0);
    // operator== is not constexpr for some reason
    STATIC_REQUIRE(g.center()[0] == r0[0]);
    STATIC_REQUIRE(g.center()[1] == r0[1]);
    STATIC_REQUIRE(g.center()[2] == r0[2]);
    STATIC_REQUIRE(g.exp() == a);
    STATIC_REQUIRE(g.coef() == c);

    SECTION("Comparisons") {
        SECTION("Equal") {
            constexpr auto g2 = make_gaussian(c, a, r0);
            STATIC_REQUIRE(g == g2);
            STATIC_REQUIRE_FALSE(g != g2);
        }
        SECTION("Different centers") {
            constexpr libchemist::type::coord<double> r1{7.89, 1.23, 4.56};
            constexpr auto g2 = make_gaussian(c, a, r1);
            STATIC_REQUIRE(g != g2);
            STATIC_REQUIRE_FALSE(g == g2);
        }
        SECTION("Different exponents") {
            constexpr double a2 = 7.89;
            constexpr auto g2   = make_gaussian(c, a2, r0);
            STATIC_REQUIRE(g != g2);
            STATIC_REQUIRE_FALSE(g == g2);
        }
        SECTION("Different coefficients") {
            constexpr double c2 = 7.89;
            constexpr auto g2   = make_gaussian(c2, a, r0);
            STATIC_REQUIRE(g != g2);
            STATIC_REQUIRE_FALSE(g == g2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr Gaussian g2(g);
        STATIC_REQUIRE(g2 == g);
    }

    SECTION("Copy Ctor") {
        constexpr Gaussian g2(g);
        constexpr Gaussian g3(std::move(g));
        STATIC_REQUIRE(g3 == g);
    }
}

TEST_CASE("Gaussian") {
    constexpr libchemist::type::coord<double> r0{1.23, 4.56, 7.89};
    constexpr double a = 2.34;
    constexpr double c = 5.67;
    auto g             = make_gaussian(c, a, r0);
    REQUIRE(g.center() == r0);
    REQUIRE(g.exp() == a);
    REQUIRE(g.coef() == c);

    SECTION("Can change center") {
        g.center()[0] = 3.21;
        REQUIRE(g.center()[0] == 3.21);
    }
    SECTION("Can change exp") {
        g.exp() = 3.21;
        REQUIRE(g.exp() == 3.21);
    }
    SECTION("Can change coef") {
        g.coef() = 3.21;
        REQUIRE(g.coef() == 3.21);
    }
    SECTION("Copy Assignment") {
        auto g2  = make_gaussian(1.11, a, r0);
        auto pg2 = &(g2 = g);
        REQUIRE(g2 == g);
        REQUIRE(pg2 == &g2);
    }
    SECTION("Move Assignment") {
        auto g2 = make_gaussian(1.11, a, r0);
        Gaussian g3(g);
        auto pg2 = &(g2 = std::move(g3));
        REQUIRE(g2 == g);
        REQUIRE(pg2 == &g2);
    }
}
