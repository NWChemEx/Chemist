#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_ao.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianAO") {
    constexpr std::array r0{1.23, 2.34, 3.45};
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr StaticGaussianAO<3> bf{cs, as, r0};
    STATIC_REQUIRE(bf.nprims() == 3);
    STATIC_REQUIRE(bf.coefficient(0) == 1.1);
    STATIC_REQUIRE(bf.coefficient(1) == 2.2);
    STATIC_REQUIRE(bf.coefficient(2) == 3.3);
    STATIC_REQUIRE(bf.exponent(0) == 4.4);
    STATIC_REQUIRE(bf.exponent(1) == 5.5);
    STATIC_REQUIRE(bf.exponent(2) == 6.6);
    STATIC_REQUIRE(bf.center()[0] == 1.23);
    STATIC_REQUIRE(bf.center()[1] == 2.34);
    STATIC_REQUIRE(bf.center()[2] == 3.45);
    STATIC_REQUIRE(bf.primitive(0).coefficient() == 1.1);
    STATIC_REQUIRE(bf.primitive(1).coefficient() == 2.2);
    STATIC_REQUIRE(bf.primitive(2).coefficient() == 3.3);
    STATIC_REQUIRE(bf.primitive(0).exponent() == 4.4);
    STATIC_REQUIRE(bf.primitive(1).exponent() == 5.5);
    STATIC_REQUIRE(bf.primitive(2).exponent() == 6.6);
    STATIC_REQUIRE(bf.primitive(0).center()[0] == 1.23);
    STATIC_REQUIRE(bf.primitive(1).center()[1] == 2.34);
    STATIC_REQUIRE(bf.primitive(2).center()[2] == 3.45);
}
