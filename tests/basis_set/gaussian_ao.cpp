#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_ao.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianAO(c, a)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr GaussianAO bf(cs, as);
    constexpr std::array gs{Gaussian{1.1, 4.4}, Gaussian{2.2, 5.5},
                            Gaussian{3.3, 6.6}};
    STATIC_REQUIRE_FALSE(bf.has_center());
    STATIC_REQUIRE(bf.nprims() == 3);
    STATIC_REQUIRE(bf.primitive(0) == gs[0]);
    STATIC_REQUIRE(bf.primitive(1) == gs[1]);
    STATIC_REQUIRE(bf.primitive(2) == gs[2]);

    SECTION("Comparisions") {
        SECTION("Same") {
            constexpr GaussianAO bf2(cs, as);
            STATIC_REQUIRE(bf == bf2);
            STATIC_REQUIRE_FALSE(bf != bf2);
        }
        SECTION("Different coefficients") {
            constexpr GaussianAO bf2(r0, as);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different exponents") {
            constexpr GaussianAO bf2(cs, r0);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different centered-ness") {
            constexpr GaussianAO bf2(cs, as, r0);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr GaussianAO bf2{bf};
        STATIC_REQUIRE(bf == bf2);
    }
    SECTION("Move Ctor") {
        constexpr GaussianAO bf2{bf};
        constexpr GaussianAO bf3{std::move(bf)};
        STATIC_REQUIRE(bf3 == bf2);
    }
}

TEST_CASE("constexpr GaussianAO(c, a, r)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr GaussianAO bf(cs, as, r0);
    constexpr std::array gs{Gaussian{1.1, 4.4, r0}, Gaussian{2.2, 5.5, r0},
                            Gaussian{3.3, 6.6, r0}};
    STATIC_REQUIRE(bf.has_center());
    STATIC_REQUIRE(bf.center()[0] == r0[0]);
    STATIC_REQUIRE(bf.center()[1] == r0[1]);
    STATIC_REQUIRE(bf.center()[2] == r0[2]);
    STATIC_REQUIRE(bf.nprims() == 3);
    STATIC_REQUIRE(bf.primitive(0) == gs[0]);
    STATIC_REQUIRE(bf.primitive(1) == gs[1]);
    STATIC_REQUIRE(bf.primitive(2) == gs[2]);

    SECTION("Comparisions") {
        SECTION("Same") {
            constexpr GaussianAO bf2(cs, as, r0);
            STATIC_REQUIRE(bf == bf2);
            STATIC_REQUIRE_FALSE(bf != bf2);
        }
        SECTION("Different coefficients") {
            constexpr GaussianAO bf2(r0, as, r0);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different exponents") {
            constexpr GaussianAO bf2(cs, r0, r0);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different centered-ness") {
            constexpr GaussianAO bf2(cs, as);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr GaussianAO bf2(bf);
        STATIC_REQUIRE(bf == bf2);
    }
    SECTION("Move Ctor") {
        constexpr GaussianAO bf2(bf);
        constexpr GaussianAO bf3(std::move(bf));
        STATIC_REQUIRE(bf3 == bf2);
    }
}

TEST_CASE("constexpr GaussianAO(Gaussian...)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr GaussianAO bf(cs, as, r0);
    constexpr GaussianAO bf2{Gaussian{1.1, 4.4, r0}, Gaussian{2.2, 5.5, r0},
                             Gaussian{3.3, 6.6, r0}};
    STATIC_REQUIRE(bf == bf2);
}

TEST_CASE("GaussianAO(Gaussian...)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    Gaussian g0{1.1, 4.4, r0};
    SECTION("Different center-ness") {
        Gaussian g1{2.2, 5.5};
        REQUIRE_THROWS_AS(GaussianAO(g0, g1), std::logic_error);
    }
    SECTION("Different centers") {
        Gaussian g2{2.2, 5.5, cs};
        REQUIRE_THROWS_AS(GaussianAO(g0, g2), std::logic_error);
    }
}

TEST_CASE("GaussianAO<std::vector<double>>") {
    GaussianAO<std::vector<double>> bf;
    REQUIRE(bf.nprims() == 0);
    REQUIRE_FALSE(bf.has_center());
    REQUIRE_THROWS_AS(bf.center(), std::bad_optional_access);
    SECTION("Can add a primitive") {
        Gaussian g{1.1, 2.2};
        bf.add_primitive(g);
        GaussianAO bf2(g);
        REQUIRE(bf == bf2);
    }
}

TEST_CASE("GaussianAO(c, a)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    GaussianAO bf(cs, as);
    std::array gs{Gaussian{1.1, 4.4}, Gaussian{2.2, 5.5}, Gaussian{3.3, 6.6}};
    REQUIRE_FALSE(bf.has_center());
    REQUIRE(bf.nprims() == 3);
    REQUIRE(bf.primitive(0) == gs[0]);
    REQUIRE(bf.primitive(1) == gs[1]);
    REQUIRE(bf.primitive(2) == gs[2]);
    REQUIRE_THROWS_AS(bf.primitive(3), std::out_of_range);

    SECTION("Can change coefficient") {
        bf.primitive(0).coefficient() = 2.2;
        REQUIRE(bf.primitive(0).coefficient() == 2.2);
    }

    SECTION("Can change exponent") {
        bf.primitive(0).exponent() = 2.2;
        REQUIRE(bf.primitive(0).exponent() == 2.2);
    }
    SECTION("Can add a center") {
        std::array r0{7.7, 8.8, 9.9};
        bf.set_center(r0);
        Gaussian corr{1.1, 4.4, r0};
        REQUIRE(bf.primitive(0) == corr);
    }
    SECTION("Copy Assignment") {
        decltype(bf) bf2;
        auto pbf2 = &(bf2 = bf);
        REQUIRE(bf == bf2);
        REQUIRE(pbf2 == &bf2);
    }
    SECTION("Move Ctor") {
        GaussianAO bf2{bf};
        decltype(bf) bf3;
        auto pbf3 = &(bf3 = std::move(bf));
        REQUIRE(bf3 == bf2);
        REQUIRE(pbf3 == &bf3);
    }
}

TEST_CASE("GaussianAO(c, a, r)") {
    constexpr std::array cs{1.1, 2.2, 3.3};
    constexpr std::array as{4.4, 5.5, 6.6};
    constexpr std::array r0{7.7, 8.8, 9.9};
    GaussianAO bf(cs, as, r0);
    constexpr std::array gs{Gaussian{1.1, 4.4, r0}, Gaussian{2.2, 5.5, r0},
                            Gaussian{3.3, 6.6, r0}};
    REQUIRE(bf.has_center());
    REQUIRE(bf.nprims() == 3);
    REQUIRE(bf.primitive(0) == gs[0]);
    REQUIRE(bf.primitive(1) == gs[1]);
    REQUIRE(bf.primitive(2) == gs[2]);

    SECTION("Can change coefficient") {
        bf.primitive(0).coefficient() = 2.2;
        REQUIRE(bf.primitive(0).coefficient() == 2.2);
    }

    SECTION("Can change exponent") {
        bf.primitive(0).exponent() = 2.2;
        REQUIRE(bf.primitive(0).exponent() == 2.2);
    }
    SECTION("Can change center through GaussianAO") {
        bf.center()[0] = 1.1;
        REQUIRE(bf.primitive(0).center()[0] == 1.1);
    }
    SECTION("Can't change center through primitive") {
        using coord_type = decltype(bf.primitive(0).center()[0]);
        using no_ref     = std::remove_reference_t<coord_type>;
        STATIC_REQUIRE(std::is_const_v<no_ref>);
    }
    SECTION("Copy Assignment") {
        decltype(bf) bf2;
        auto pbf2 = &(bf2 = bf);
        REQUIRE(bf == bf2);
        REQUIRE(pbf2 == &bf2);
    }
    SECTION("Move Ctor") {
        GaussianAO bf2{bf};
        decltype(bf) bf3;
        auto pbf3 = &(bf3 = std::move(bf));
        REQUIRE(bf3 == bf2);
        REQUIRE(pbf3 == &bf3);
    }
}
