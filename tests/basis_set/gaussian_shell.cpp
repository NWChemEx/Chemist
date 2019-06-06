#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_shell.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr GaussianShell(c, a)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr GaussianShell s(true, 2, cs, as);
    constexpr GaussianAO bf(cs, as);
    STATIC_REQUIRE(s.naos() == 5);
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
            constexpr GaussianShell s2(true, 2, cs, as);
            STATIC_REQUIRE(s == s2);
            STATIC_REQUIRE_FALSE(s != s2);
        }
        SECTION("Different purity") {
            constexpr GaussianShell s2(false, 2, cs, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different angular momentum") {
            constexpr GaussianShell s2(true, 3, cs, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different coefficients") {
            constexpr GaussianShell s2(true, 2, as, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different exponents") {
            constexpr GaussianShell s2(true, 2, cs, cs);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different center-ness") {
            constexpr std::array r0{7.7, 8.8, 9.9};
            constexpr GaussianShell s2(true, 2, cs, as, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
    }
    SECTION("Copy Ctor") {
        constexpr GaussianShell s2(s);
        STATIC_REQUIRE(s2 == s);
    }
    SECTION("Move Ctor") {
        constexpr GaussianShell s2(s);
        constexpr GaussianShell s3(std::move(s));
        STATIC_REQUIRE(s2 == s3);
    }
}

TEST_CASE("constexpr GaussianShell(c, a, r)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr GaussianShell s(false, 2, cs, as, r0);
    constexpr GaussianAO bf(cs, as, r0);
    STATIC_REQUIRE(s.naos() == 6);
    STATIC_REQUIRE(s.has_center());
    STATIC_REQUIRE_FALSE(s.pure());
    STATIC_REQUIRE(s.l() == 2);
    STATIC_REQUIRE(s.ao(0) == bf);
    STATIC_REQUIRE(s.ao(1) == bf);
    STATIC_REQUIRE(s.ao(2) == bf);
    STATIC_REQUIRE(s.ao(3) == bf);
    STATIC_REQUIRE(s.ao(4) == bf);
    STATIC_REQUIRE(s.ao(5) == bf);

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr GaussianShell s2(false, 2, cs, as, r0);
            STATIC_REQUIRE(s == s2);
            STATIC_REQUIRE_FALSE(s != s2);
        }
        SECTION("Different purity") {
            constexpr GaussianShell s2(true, 2, cs, as, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different angular momentum") {
            constexpr GaussianShell s2(false, 3, cs, as, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different coefficients") {
            constexpr GaussianShell s2(false, 2, as, as, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different exponents") {
            constexpr GaussianShell s2(false, 2, cs, cs, r0);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
        SECTION("Different center-ness") {
            constexpr GaussianShell s2(false, 2, cs, as);
            STATIC_REQUIRE(s != s2);
            STATIC_REQUIRE_FALSE(s == s2);
        }
    }
    SECTION("Copy Ctor") {
        constexpr GaussianShell s2(s);
        STATIC_REQUIRE(s2 == s);
    }
    SECTION("Move Ctor") {
        constexpr GaussianShell s2(s);
        constexpr GaussianShell s3(std::move(s));
        STATIC_REQUIRE(s2 == s3);
    }
}

TEST_CASE("constexpr GaussianShell(GaussianAO)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr GaussianAO bf(cs, as, r0);
    constexpr GaussianShell s(false, 2, bf);
    constexpr GaussianShell s2(false, 2, cs, as, r0);
    STATIC_REQUIRE(s == s2);
}

TEST_CASE("GaussianShell(c, a)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    GaussianShell s(true, 2, cs, as);
    GaussianAO bf(cs, as);
    REQUIRE(s.naos() == 5);
    REQUIRE_FALSE(s.has_center());
    REQUIRE(s.pure());
    REQUIRE(s.l() == 2);
    REQUIRE(s.ao(0) == bf);
    REQUIRE(s.ao(1) == bf);
    REQUIRE(s.ao(2) == bf);
    REQUIRE(s.ao(3) == bf);
    REQUIRE(s.ao(4) == bf);

    SECTION("Can Change purity") {
        s.pure() = false;
        GaussianShell s2(false, 2, cs, as);
        REQUIRE(s == s2);
    }
    SECTION("Can change angular momentum") {
        s.l() = 1;
        GaussianShell s2(true, 1, cs, as);
        REQUIRE(s == s2);
    }
    SECTION("Can't change coefficient through AO") {
        using c_type =
          std::remove_reference_t<decltype(s.ao(0).primitive(0).coefficient())>;
        STATIC_REQUIRE(std::is_const_v<c_type>);
    }
    SECTION("Can change coefficient") {
        s.coefficient(0) = as[0];
        s.coefficient(1) = as[1];
        s.coefficient(2) = as[2];
        GaussianShell s2(true, 2, as, as);
        REQUIRE(s == s2);
    }
    SECTION("Can't change exponent through AO") {
        using a_type =
          std::remove_reference_t<decltype(s.ao(0).primitive(0).exponent())>;
        STATIC_REQUIRE(std::is_const_v<a_type>);
    }
    SECTION("Can change exponent") {
        s.exponent(0) = cs[0];
        s.exponent(1) = cs[1];
        s.exponent(2) = cs[2];
        GaussianShell s2(true, 2, cs, cs);
        REQUIRE(s == s2);
    }
    SECTION("Can't change center through AO") {
        using r_type = std::remove_reference_t<decltype(s.ao(0).center())>;
        STATIC_REQUIRE(std::is_const_v<r_type>);
    }
    SECTION("Can't change center through primitive") {
        using r_type =
          std::remove_reference_t<decltype(s.ao(0).primitive(0).center())>;
        STATIC_REQUIRE(std::is_const_v<r_type>);
    }
    SECTION("Can set the center") {
        constexpr std::array r0{7.7, 8.8, 9.9};
        s.set_center(r0);
        GaussianShell s2(true, 2, cs, as, r0);
        REQUIRE(s == s2);
    }
    SECTION("Throws for bad AO access") {
        REQUIRE_THROWS_AS(s.ao(5), std::out_of_range);
    }
    SECTION("Throws for bad coefficient access") {
        REQUIRE_THROWS_AS(s.coefficient(5), std::out_of_range);
    }
    SECTION("Throws for bad exponent access") {
        REQUIRE_THROWS_AS(s.exponent(5), std::out_of_range);
    }
    SECTION("Copy Assignment") {
        decltype(s) s2;
        auto ps2 = &(s2 = s);
        REQUIRE(s2 == s);
        REQUIRE(ps2 == &s2);
    }
    SECTION("Move Assignment") {
        decltype(s) s2;
        GaussianShell s3(s);
        auto ps2 = &(s2 = std::move(s));
        REQUIRE(s2 == s3);
        REQUIRE(ps2 == &s2);
    }
}
