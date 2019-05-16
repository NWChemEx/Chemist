#include <catch2/catch.hpp>
#include <libchemist/basis_set/detail_/gaussian_ao_pimpl.hpp>
#include <libchemist/basis_set/gaussian.hpp>

using namespace libchemist::basis_set::detail_;
using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianAOPIMPL") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr auto g1 = make_gaussian(1.1, 2.2, r0);
    constexpr auto g2 = make_gaussian(3.3, 4.4, r0);
    constexpr auto bf = make_gaussian_ao_pimpl(g1, g2);
    STATIC_REQUIRE(bf.size() == 2);
    STATIC_REQUIRE(bf.coef(0) == 1.1);
    STATIC_REQUIRE(bf.coef(1) == 3.3);
    STATIC_REQUIRE(bf.exp(0) == 2.2);
    STATIC_REQUIRE(bf.exp(1) == 4.4);
    STATIC_REQUIRE(bf.center()[0] == r0[0]);
    STATIC_REQUIRE(bf.center()[1] == r0[1]);
    STATIC_REQUIRE(bf.center()[2] == r0[2]);
}

TEST_CASE("GaussianAOPIMPL") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr auto g1 = make_gaussian(1.1, 2.2, r0);
    constexpr auto g2 = make_gaussian(3.3, 4.4, r0);
    auto bf           = make_gaussian_ao_pimpl(g1, g2);

    REQUIRE(bf.size() == 2);
    REQUIRE(bf.coef(0) == 1.1);
    REQUIRE(bf.coef(1) == 3.3);
    REQUIRE(bf.exp(0) == 2.2);
    REQUIRE(bf.exp(1) == 4.4);
    REQUIRE(bf.center()[0] == r0[0]);
    REQUIRE(bf.center()[1] == r0[1]);
    REQUIRE(bf.center()[2] == r0[2]);

    SECTION("Can change coef") {
        bf.coef(0) = 9.9;
        REQUIRE(bf.coef(0) == 9.9);
    }
    SECTION("Can change exp") {
        bf.exp(0) = 9.9;
        REQUIRE(bf.exp(0) == 9.9);
    }
    SECTION("Can change function's center") {
        std::array r1{1.1, 2.2, 3.3};
        bf.center() = r1;
        REQUIRE(bf.center() == r1);
    }
}
