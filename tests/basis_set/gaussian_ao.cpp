#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_ao.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianAO(c, a)") {
    constexpr auto bf =
      make_gaussian_ao(std::array{1.1, 2.2, 3.3}, std::array{4.4, 5.5, 6.6});
    constexpr std::array gs{Gaussian<double>{1.1, 4.4},
                            Gaussian<double>{2.2, 5.5},
                            Gaussian<double>{3.3, 6.6}};
    STATIC_REQUIRE_FALSE(bf.has_center());
    STATIC_REQUIRE(bf.nprims() == 3);
    STATIC_REQUIRE(bf.primitive(0) == gs[0]);
    STATIC_REQUIRE(bf.primitive(1) == gs[1]);
    STATIC_REQUIRE(bf.primitive(2) == gs[2]);

    SECTION("Comparisions") {
        SECTION("Same") {
            constexpr auto bf2 = make_gaussian_ao(std::array{1.1, 2.2, 3.3},
                                                  std::array{4.4, 5.5, 6.6});
            STATIC_REQUIRE(bf == bf2);
            STATIC_REQUIRE_FALSE(bf != bf2);
        }
        SECTION("Different coefficients") {
            constexpr auto bf2 = make_gaussian_ao(std::array{7.7, 8.8, 9.9},
                                                  std::array{4.4, 5.5, 6.6});
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different exponents") {
            constexpr auto bf2 = make_gaussian_ao(std::array{1.1, 2.2, 3.3},
                                                  std::array{7.7, 8.8, 9.9});
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different centered-ness") {
            constexpr std::array r0{7.7, 8.8, 9.9};
            constexpr auto bf2 = make_gaussian_ao(
              std::array{1.1, 2.2, 3.3}, std::array{4.4, 5.5, 6.6}, r0);
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
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr auto bf = make_gaussian_ao(std::array{1.1, 2.2, 3.3},
                                         std::array{4.4, 5.5, 6.6}, r0);
    constexpr std::array gs{Gaussian<double>{1.1, 4.4, r0},
                            Gaussian<double>{2.2, 5.5, r0},
                            Gaussian<double>{3.3, 6.6, r0}};
    STATIC_REQUIRE(bf.has_center());
    STATIC_REQUIRE(bf.center()[0] == 7.7);
    STATIC_REQUIRE(bf.center()[1] == 8.8);
    STATIC_REQUIRE(bf.center()[2] == 9.9);
    STATIC_REQUIRE(bf.nprims() == 3);
    STATIC_REQUIRE(bf.primitive(0) == gs[0]);
    STATIC_REQUIRE(bf.primitive(1) == gs[1]);
    STATIC_REQUIRE(bf.primitive(2) == gs[2]);

    SECTION("Comparisions") {
        SECTION("Same") {
            constexpr auto bf2 = make_gaussian_ao(
              std::array{1.1, 2.2, 3.3}, std::array{4.4, 5.5, 6.6}, r0);
            STATIC_REQUIRE(bf == bf2);
            STATIC_REQUIRE_FALSE(bf != bf2);
        }
        SECTION("Different coefficients") {
            constexpr auto bf2 = make_gaussian_ao(
              std::array{7.7, 8.8, 9.9}, std::array{4.4, 5.5, 6.6}, r0);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different exponents") {
            constexpr auto bf2 = make_gaussian_ao(
              std::array{1.1, 2.2, 3.3}, std::array{7.7, 8.8, 9.9}, r0);
            STATIC_REQUIRE_FALSE(bf == bf2);
            STATIC_REQUIRE(bf != bf2);
        }
        SECTION("Different centered-ness") {
            constexpr auto bf2 = make_gaussian_ao(std::array{1.1, 2.2, 3.3},
                                                  std::array{4.4, 5.5, 6.6});
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

TEST_CASE("constexpr GaussianAO(Gaussian...)") {
    constexpr std::array r0{7.7, 8.8, 9.9};
    constexpr auto bf  = make_gaussian_ao(std::array{1.1, 2.2, 3.3},
                                         std::array{4.4, 5.5, 6.6}, r0);
    constexpr auto bf2 = make_gaussian_ao(Gaussian<double>{1.1, 4.4, r0},
                                          Gaussian<double>{2.2, 5.5, r0},
                                          Gaussian<double>{3.3, 6.6, r0});
    STATIC_REQUIRE(bf == bf2);
}

TEST_CASE("GaussianAO<double, std::vector<double>>") {
    GaussianAO<double> bf;
    REQUIRE(bf.nprims() == 0);
    REQUIRE_FALSE(bf.has_center());
    REQUIRE_THROWS_AS(bf.center(), std::bad_optional_access);
    SECTION("Can add a primitive") {
        Gaussian<double> g{1.1, 2.2};
        bf.add_primitive(g);
        auto bf2 = make_gaussian_ao(g);
        REQUIRE(bf == bf2);
    }
}

TEST_CASE("GaussianAO(c, a)") {
    auto bf =
      make_gaussian_ao(std::array{1.1, 2.2, 3.3}, std::array{4.4, 5.5, 6.6});
    std::array gs{Gaussian<double>{1.1, 4.4}, Gaussian<double>{2.2, 5.5},
                  Gaussian<double>{3.3, 6.6}};
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
        Gaussian<double> corr{1.1, 4.4, r0};
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
    constexpr std::array r0{7.7, 8.8, 9.9};
    auto bf = make_gaussian_ao(std::array{1.1, 2.2, 3.3},
                               std::array{4.4, 5.5, 6.6}, r0);
    constexpr std::array gs{Gaussian<double>{1.1, 4.4, r0},
                            Gaussian<double>{2.2, 5.5, r0},
                            Gaussian<double>{3.3, 6.6, r0}};
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
