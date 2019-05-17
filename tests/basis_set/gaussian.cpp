#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr Gaussian<double>") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr Gaussian<double> g(2.34, 3.45, r0);
    STATIC_REQUIRE(g.coef() == 2.34);
    STATIC_REQUIRE(g.exp() == 3.45);
    STATIC_REQUIRE(g.center()[0] == r0[0]);
    STATIC_REQUIRE(g.center()[1] == r0[1]);
    STATIC_REQUIRE(g.center()[2] == r0[2]);

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr Gaussian<double> g2(2.34, 3.45, r0);
            STATIC_REQUIRE(g == g2);
            STATIC_REQUIRE_FALSE(g != g2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr Gaussian<double> g2(g);
        STATIC_REQUIRE(g2.coef() == 2.34);
        STATIC_REQUIRE(g2.exp() == 3.45);
        STATIC_REQUIRE(g2.center()[0] == 1.23);
        STATIC_REQUIRE(g2.center()[1] == 4.56);
        STATIC_REQUIRE(g2.center()[2] == 7.89);
    }
    SECTION("Move Ctor") {
        constexpr Gaussian<double> g2(std::move(g));
        STATIC_REQUIRE(g2.coef() == 2.34);
        STATIC_REQUIRE(g2.exp() == 3.45);
        STATIC_REQUIRE(g2.center()[0] == 1.23);
        STATIC_REQUIRE(g2.center()[1] == 4.56);
        STATIC_REQUIRE(g2.center()[2] == 7.89);
    }
}

TEST_CASE("Gaussian<double>") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    Gaussian<double> g(2.34, 3.45, r0);
    REQUIRE(g.coef() == 2.34);
    REQUIRE(g.exp() == 3.45);
    REQUIRE(g.center() == r0);

    SECTION("Can change coef") {
        g.coef() = 4.56;
        REQUIRE(g.coef() == 4.56);
    }
    SECTION("Can change exp") {
        g.exp() = 4.56;
        REQUIRE(g.exp() == 4.56);
    }
    SECTION("Can change center") {
        coord<double> r1{1.1, 2.2, 3.3};
        g.center() = r1;
        REQUIRE(g.center() == r1);
    }

    SECTION("Copy Assignment") {
        Gaussian<double> g2(1.1, 2.2, r0);
        auto* pg2 = &(g2 = g);
        REQUIRE(g2.coef() == 2.34);
        REQUIRE(g2.exp() == 3.45);
        REQUIRE(g2.center()[0] == 1.23);
        REQUIRE(g2.center()[1] == 4.56);
        REQUIRE(g2.center()[2] == 7.89);
        REQUIRE(&g2 == pg2);
    }
    SECTION("Move Ctor") {
        Gaussian<double> g2(1.1, 2.2, r0);
        auto* pg2 = &(g2 = std::move(g));
        REQUIRE(g2.coef() == 2.34);
        REQUIRE(g2.exp() == 3.45);
        REQUIRE(g2.center()[0] == 1.23);
        REQUIRE(g2.center()[1] == 4.56);
        REQUIRE(g2.center()[2] == 7.89);
        REQUIRE(&g2 == pg2);
    }
}

TEST_CASE("Gaussian<GaussianTraits<double*, double*, const coord<double>*>") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    double c{2.34};
    double a{3.45};
    using traits =
      detail_::GaussianTraits_<double*, double*, const coord<double>*>;
    Gaussian_<traits> g(&c, &a, &r0);

    SECTION("Can change coef") {
        g.coef() = 9.99;
        REQUIRE(g.coef() == 9.99);
        REQUIRE(c == 9.99);
    }
    SECTION("Can change exponent") {
        g.exp() = 9.99;
        REQUIRE(g.exp() == 9.99);
        REQUIRE(a == 9.99);
    }
}
