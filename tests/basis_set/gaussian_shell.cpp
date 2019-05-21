#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_shell.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr GaussianShell") {
    constexpr std::array cs{1.23, 2.34, 3.45};
    constexpr std::array as{4.56, 5.67, 6.78};
    constexpr std::array r0{1.1, 2.2, 3.3};
    constexpr StaticGaussianShell<double, 3> s(2, cs, as, r0, true);
    constexpr StaticGaussianAO<double, 3> bf(cs, as, r0);
    STATIC_REQUIRE(s.l() == 2);
    STATIC_REQUIRE(s.size() == 5);
    STATIC_REQUIRE(s[0] == bf);
    STATIC_REQUIRE(s.center()[0] == r0[0]);
    STATIC_REQUIRE(s.center()[1] == r0[1]);
    STATIC_REQUIRE(s.center()[2] == r0[2]);
    STATIC_REQUIRE(s.pure());

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr StaticGaussianShell<double, 3> s2(2, cs, as, r0, true);
            STATIC_REQUIRE(s == s2);
            STATIC_REQUIRE_FALSE(s != s2);
        }
        SECTION("Different l values") {
            constexpr StaticGaussianShell<double, 3> s2(3, cs, as, r0, true);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different coefficient values") {
            constexpr StaticGaussianShell<double, 3> s2(2, as, as, r0, true);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different exponent values") {
            constexpr StaticGaussianShell<double, 3> s2(2, cs, cs, r0, true);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different centers") {
            constexpr StaticGaussianShell<double, 3> s2(2, cs, as, as, true);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different purity") {
            constexpr StaticGaussianShell<double, 3> s2(2, cs, as, r0, false);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr StaticGaussianShell<double, 3> s2(s);
        REQUIRE(s == s2);
    }
    SECTION("Move Ctor") {
        constexpr StaticGaussianShell<double, 3> s2(std::move(s));
        REQUIRE(s == s2);
    }
}

TEST_CASE("GaussianShell<double>") {
    constexpr std::array cs{1.23, 2.34, 3.45};
    constexpr std::array as{4.56, 5.67, 6.78};
    constexpr std::array r0{1.1, 2.2, 3.3};
    StaticGaussianShell<double, 3> s(2, cs, as, r0, false);
    StaticGaussianAO<double, 3> bf(cs, as, r0);
    REQUIRE(s.l() == 2);
    REQUIRE(s.size() == 6);
    REQUIRE(s.center() == r0);
    REQUIRE_FALSE(s.pure());
    REQUIRE(s[0] == bf);
    for(const auto& bfi : s) REQUIRE(bfi == bf);
    SECTION("Can change l") {
        s.l() = 4;
        REQUIRE(s.l() == 4);
    }
    SECTION("Can change purity") {
        s.pure() = true;
        REQUIRE(s.pure());
    }
    SECTION("Can change center") {
        s.center() = cs;
        REQUIRE(s.center() == cs);
    }
    SECTION("Can't change coef") {
        using coef_type = std::remove_reference_t<decltype(s[0][0].coef())>;
        STATIC_REQUIRE(std::is_const_v<coef_type>);
    }
    SECTION("Can't change exp") {
        using exp_type = std::remove_reference_t<decltype(s[0][0].exp())>;
        STATIC_REQUIRE(std::is_const_v<exp_type>);
    }
    SECTION("Copy Assignment") {
        StaticGaussianShell<double, 3> s2(3, cs, as, r0, false);
        auto ps2 = &(s2 = s);
        REQUIRE(s == s2);
        REQUIRE(ps2 == &s2);
    }
    SECTION("Move Assignment") {
        StaticGaussianShell<double, 3> s2(3, cs, as, r0, false);
        StaticGaussianShell<double, 3> s3(s);
        auto ps2 = &(s2 = std::move(s));
        REQUIRE(s3 == s2);
        REQUIRE(ps2 == &s2);
    }
}
