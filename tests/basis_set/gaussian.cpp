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
            constexpr Gaussian<double> g2(2.34, 3.45, std::array{.1, .2, .3});
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
        REQUIRE(g2 == g);
        REQUIRE(&g2 == pg2);
    }
    SECTION("Move Ctor") {
        Gaussian<double> g2(g);
        Gaussian<double> g3(1.1, 2.2, r0);
        auto* pg3 = &(g3 = std::move(g));
        REQUIRE(&g3 == pg3);
        REQUIRE(g3 == g2);
    }
}

TEST_CASE("Gaussian<GaussianTraits<double*, double*, const coord<double>*>") {
    const coord<double> r0{1.23, 4.56, 7.89};
    double c{2.34};
    double a{3.45};
    auto g = make_gaussian(&c, &a, &r0);

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
    SECTION("Can not change coord") {
        using no_ref = std::remove_reference_t<decltype(g.center())>;
        STATIC_REQUIRE(std::is_const_v<no_ref>);
        REQUIRE(g.center() == r0);
    }
}
