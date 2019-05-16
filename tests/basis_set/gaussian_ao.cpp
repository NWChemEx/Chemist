#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_ao.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr AOBasisFunction") {
    constexpr libchemist::type::coord<double> r0{1.23, 3.45, 4.56};
    constexpr auto g1 = make_gaussian(1.1, 2.2, r0);
    constexpr auto g2 = make_gaussian(3.3, 4.4, r0);
    constexpr auto bf = make_gaussian_ao(g1, g2);

    STATIC_REQUIRE(bf.size() == 2);
    STATIC_REQUIRE(bf.coef(0) == 1.1);
    STATIC_REQUIRE(bf.coef(1) == 3.3);
    STATIC_REQUIRE(bf.exp(0) == 2.2);
    STATIC_REQUIRE(bf.exp(1) == 4.4);
    STATIC_REQUIRE(bf.center()[0] == r0[0]);
    STATIC_REQUIRE(bf.center()[1] == r0[1]);
    STATIC_REQUIRE(bf.center()[2] == r0[2]);

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr auto bf2 = make_gaussian_ao(g1, g2);
            STATIC_REQUIRE(bf == bf2);
            STATIC_REQUIRE_FALSE(bf != bf2);
        }
        SECTION("Different order") {
            constexpr auto bf2 = make_gaussian_ao(g2, g1);
            STATIC_REQUIRE(bf != bf2);
            STATIC_REQUIRE_FALSE(bf == bf2);
        }
        SECTION("Different primitives") {
            constexpr auto bf2 = make_gaussian_ao(g1, g1);
            STATIC_REQUIRE(bf != bf2);
            STATIC_REQUIRE_FALSE(bf == bf2);
        }
    }
}

TEST_CASE("AOBasisFunction") {
    constexpr libchemist::type::coord<double> r0{1.23, 3.45, 4.56};
    constexpr auto g1 = make_gaussian(1.1, 2.2, r0);
    constexpr auto g2 = make_gaussian(3.3, 4.4, r0);
    auto bf           = make_gaussian_ao(g1, g2);

    SECTION("Operator[]") {
        REQUIRE(bf[0] == g1);
        REQUIRE(bf[1] == g2);

        SECTION("Can change coef") {
            bf[0].coef() = 9.9;
            REQUIRE(bf.coef(0) == 9.9);
        }
        SECTION("Can change exponent") {
            bf[1].exp() = 9.9;
            REQUIRE(bf.exp(1) == 9.9);
        }
        SECTION("Can't change center") {
            using center_type   = decltype(bf[0].center());
            using no_ref_center = std::remove_reference_t<center_type>;
            STATIC_REQUIRE(std::is_const_v<no_ref_center>);
        }
    }
}
