#include <catch2/catch.hpp>
#include <libchemist/basis_set/detail_/gaussian_pimpl.hpp>

using namespace libchemist::basis_set::detail_;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianPIMPL<double>") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr auto g = make_guassian_pimpl(2.34, 3.45, r0);
    STATIC_REQUIRE(g.coef() == 2.34);
    STATIC_REQUIRE(g.exp() == 3.45);
    STATIC_REQUIRE(g.center()[0] == r0[0]);
    STATIC_REQUIRE(g.center()[1] == r0[1]);
    STATIC_REQUIRE(g.center()[2] == r0[2]);

    SECTION("Copy Ctor") {
        constexpr GaussianPIMPL g2(g);
        STATIC_REQUIRE(g2.coef() == 2.34);
        STATIC_REQUIRE(g2.exp() == 3.45);
        STATIC_REQUIRE(g2.center()[0] == 1.23);
        STATIC_REQUIRE(g2.center()[1] == 4.56);
        STATIC_REQUIRE(g2.center()[2] == 7.89);
    }
    SECTION("Move Ctor") {
        constexpr GaussianPIMPL g2(std::move(g));
        STATIC_REQUIRE(g2.coef() == 2.34);
        STATIC_REQUIRE(g2.exp() == 3.45);
        STATIC_REQUIRE(g2.center()[0] == 1.23);
        STATIC_REQUIRE(g2.center()[1] == 4.56);
        STATIC_REQUIRE(g2.center()[2] == 7.89);
    }
}

TEST_CASE("GaussianPIMPL<double>") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    auto g = make_guassian_pimpl(2.34, 3.45, r0);
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
        GaussianPIMPL g2 = make_guassian_pimpl(1.1, 2.2, r0);
        auto* pg2        = &(g2 = g);
        REQUIRE(g2.coef() == 2.34);
        REQUIRE(g2.exp() == 3.45);
        REQUIRE(g2.center()[0] == 1.23);
        REQUIRE(g2.center()[1] == 4.56);
        REQUIRE(g2.center()[2] == 7.89);
        REQUIRE(&g2 == pg2);
    }
    SECTION("Move Ctor") {
        GaussianPIMPL g2 = make_guassian_pimpl(1.1, 2.2, r0);
        auto* pg2        = &(g2 = std::move(g));
        REQUIRE(g2.coef() == 2.34);
        REQUIRE(g2.exp() == 3.45);
        REQUIRE(g2.center()[0] == 1.23);
        REQUIRE(g2.center()[1] == 4.56);
        REQUIRE(g2.center()[2] == 7.89);
        REQUIRE(&g2 == pg2);
    }
}
