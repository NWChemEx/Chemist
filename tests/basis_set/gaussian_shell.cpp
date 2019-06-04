#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_shell.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr GaussianShell(c, a)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr auto s  = make_gaussian_shell(true, 2, cs, as);
    constexpr auto bf = make_gaussian_ao(cs, as);
    STATIC_REQUIRE(s.nao() == 5);
    STATIC_REQUIRE_FALSE(s.has_center());
    STATIC_REQUIRE(s.pure());
    STATIC_REQUIRE(s.l() == 2);
    STATIC_REQUIRE(s.ao(0) == bf);
    STATIC_REQUIRE(s.ao(1) == bf);
    STATIC_REQUIRE(s.ao(2) == bf);
    STATIC_REQUIRE(s.ao(3) == bf);
    STATIC_REQUIRE(s.ao(4) == bf);

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr auto s2 = make_gaussian_shell(true, 2, cs, as);
            STATIC_REQUIRE(s == s2);
            STATIC_REQUIRE_FALSE(s != s2);
        }
        SECTION("Different purity") {
            constexpr auto s2 = make_gaussian_shell(false, 2, cs, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different angular momentum") {
            constexpr auto s2 = make_gaussian_shell(true, 3, cs, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different coefficients") {
            constexpr auto s2 = make_gaussian_shell(true, 2, as, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different exponents") {
            constexpr auto s2 = make_gaussian_shell(true, 2, cs, cs);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different center-ness") {
            constexpr std::array r0{7.7, 8.8, 9.9};
            constexpr auto s2 = make_gaussian_shell(true, 2, cs, as, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
    }
}

TEST_CASE("constexpr GaussianShell(c, a, r)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r{7.7, 8.8, 9.9};
    constexpr auto s  = make_gaussian_shell(false, 2, cs, as, r);
    constexpr auto bf = make_gaussian_ao(cs, as, r);
    STATIC_REQUIRE(s.nao() == 6);
    STATIC_REQUIRE_FALSE(s.pure());
    STATIC_REQUIRE(s.has_center());
    STATIC_REQUIRE(s.center()[0] == 7.7);
    STATIC_REQUIRE(s.center()[1] == 8.8);
    STATIC_REQUIRE(s.center()[2] == 9.9);
    STATIC_REQUIRE(s.l() == 2);
    STATIC_REQUIRE(s.ao(0) == bf);
    STATIC_REQUIRE(s.ao(1) == bf);
    STATIC_REQUIRE(s.ao(2) == bf);
    STATIC_REQUIRE(s.ao(3) == bf);
    STATIC_REQUIRE(s.ao(4) == bf);
    STATIC_REQUIRE(s.ao(5) == bf);
    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr auto s2 = make_gaussian_shell(false, 2, cs, as, r);
            STATIC_REQUIRE(s == s2);
            STATIC_REQUIRE_FALSE(s != s2);
        }
        SECTION("Different purity") {
            constexpr auto s2 = make_gaussian_shell(true, 2, cs, as, r);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different angular momentum") {
            constexpr auto s2 = make_gaussian_shell(false, 3, cs, as, r);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different coefficients") {
            constexpr auto s2 = make_gaussian_shell(false, 2, as, as, r);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different exponents") {
            constexpr auto s2 = make_gaussian_shell(false, 2, cs, cs, r);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different center-ness") {
            constexpr std::array r0{7.7, 8.8, 9.9};
            constexpr auto s2 = make_gaussian_shell(false, 2, cs, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different centers") {
            constexpr std::array r0{8.8, 8.8, 9.9};
            constexpr auto s2 = make_gaussian_shell(false, 2, cs, as, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
    }
}
