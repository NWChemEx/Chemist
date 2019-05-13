#include <catch2/catch.hpp>
#include <libchemist/basis_set/primitive.hpp>

using namespace libchemist::basis_set;

TEST_CASE("Gaussian<double>") {
    constexpr Gaussian<double> g1(1.23, 2.45);
    SECTION("Primary Ctor") {
        STATIC_REQUIRE(g1.coef() == 1.23);
        STATIC_REQUIRE(g1.exp() == 2.45);
    }
    SECTION("Copy Ctor") {
        constexpr Gaussian<double> g2(g1);
        STATIC_REQUIRE(g2.coef() == 1.23);
        STATIC_REQUIRE(g2.exp() == 2.45);
    }
}

TEST_CASE("GaussianAlias<double>") {
    double alpha{2.45};
    double coef{1.23};
    GaussianAlias<double> g1(&coef, &alpha);
    SECTION("Primary Ctor") {
        REQUIRE(&(g1.coef()) == &coef);
        REQUIRE(&(g1.exp()) == &alpha);
    }
}

TEST_CASE("GaussianAlias<const double>") {
    double alpha{2.45};
    double coef{1.23};
    GaussianAlias<const double> g1(&coef, &alpha);
    SECTION("Primary Ctor") {
        REQUIRE(&(g1.coef()) == &coef);
        REQUIRE(&(g1.exp()) == &alpha);
    }
}
