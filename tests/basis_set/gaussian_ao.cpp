#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_ao.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianAO") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr std::array cs{2.34, 3.45, 4.56};
    constexpr std::array as{5.67, 6.78, 7.89};
    constexpr StaticGaussianAO<double, 3> bf(cs, as, r0);
    STATIC_REQUIRE(bf.size() == 3);
    STATIC_REQUIRE(bf.coef(0) == cs[0]);
    STATIC_REQUIRE(bf.coef(1) == cs[1]);
    STATIC_REQUIRE(bf.coef(2) == cs[2]);
    STATIC_REQUIRE(bf.exp(0) == as[0]);
    STATIC_REQUIRE(bf.exp(1) == as[1]);
    STATIC_REQUIRE(bf.exp(2) == as[2]);
    STATIC_REQUIRE(bf.center()[0] == r0[0]);
    STATIC_REQUIRE(bf.center()[1] == r0[1]);
    STATIC_REQUIRE(bf.center()[2] == r0[2]);
}

TEST_CASE("GaussianAO") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr std::array cs{2.34, 3.45, 4.56};
    constexpr std::array as{5.67, 6.78, 7.89};
    StaticGaussianAO<double, 3> bf(cs, as, r0);

    REQUIRE(bf.size() == 3);
    REQUIRE(bf.coef(0) == 2.34);
    REQUIRE(bf.coef(1) == 3.45);
    REQUIRE(bf.exp(0) == 5.67);
    REQUIRE(bf.exp(1) == 6.78);
    REQUIRE(bf.center()[0] == r0[0]);
    REQUIRE(bf.center()[1] == r0[1]);
    REQUIRE(bf.center()[2] == r0[2]);

    SECTION("Can change coef") {
        bf.coef(0) = 9.9;
        REQUIRE(bf.coef(0) == 9.9);
        SECTION("Can go through prim") {
            bf[0].coef() = 8.8;
            REQUIRE(bf.coef(0) == 8.8);
        }
    }
    SECTION("Can change exp") {
        bf.exp(0) = 9.9;
        REQUIRE(bf.exp(0) == 9.9);
        SECTION("Can go through prim") {
            bf[0].exp() = 8.8;
            REQUIRE(bf.exp(0) == 8.8);
        }
    }
    SECTION("Can change function's center") {
        std::array r1{1.1, 2.2, 3.3};
        bf.center() = r1;
        REQUIRE(bf.center() == r1);
        SECTION("Can't change through prim") {
            using center_type = decltype(bf[0].center());
            using no_ref      = std::remove_reference_t<center_type>;
            STATIC_REQUIRE(std::is_const_v<no_ref>);
        }
    }
}
