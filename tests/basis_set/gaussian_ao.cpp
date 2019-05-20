#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_ao.hpp>

using namespace libchemist::basis_set;
using namespace libchemist::type;

TEST_CASE("constexpr GaussianAO") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr std::array cs{2.34, 3.45, 4.56};
    constexpr std::array as{5.67, 6.78, 7.89};
    constexpr StaticGaussianAO<double, 3> bf(cs, as, r0);
    constexpr Gaussian<double> p1(cs[0], as[0], r0);
    constexpr Gaussian<double> p2(cs[1], as[1], r0);
    constexpr Gaussian<double> p3(cs[2], as[2], r0);
    STATIC_REQUIRE(bf.size() == 3);
    STATIC_REQUIRE(bf[0] == p1);
    STATIC_REQUIRE(bf[1] == p2);
    STATIC_REQUIRE(bf[2] == p3);
    STATIC_REQUIRE(bf.center()[0] == r0[0]);
    STATIC_REQUIRE(bf.center()[1] == r0[1]);
    STATIC_REQUIRE(bf.center()[2] == r0[2]);

    SECTION("Comparisons") {
        SECTION("Equal") {
            constexpr StaticGaussianAO<double, 3> bf2(cs, as, r0);
            STATIC_REQUIRE(bf == bf2);
            STATIC_REQUIRE_FALSE(bf != bf2);
        }
        SECTION("Different number of primitives") {
            constexpr std::array cs2{2.34, 3.45};
            constexpr std::array as2{5.67, 6.78};
            constexpr StaticGaussianAO<double, 2> bf2(cs2, as2, r0);
            STATIC_REQUIRE(bf != bf2);
            STATIC_REQUIRE_FALSE(bf == bf2);
        }
        SECTION("Different primitives") {
            constexpr std::array cs2{2.34, 3.45, 7.89};
            constexpr std::array as2{5.67, 6.78, 4.56};
            constexpr StaticGaussianAO<double, 3> bf2(cs2, as2, r0);
            STATIC_REQUIRE(bf != bf2);
            STATIC_REQUIRE_FALSE(bf == bf2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr StaticGaussianAO<double, 3> bf2(bf);
        STATIC_REQUIRE(bf2 == bf);
    }

    SECTION("Move Ctor") {
        constexpr StaticGaussianAO<double, 3> bf2(bf);
        constexpr StaticGaussianAO<double, 3> bf3(std::move(bf));
        STATIC_REQUIRE(bf2 == bf3);
    }
}

TEST_CASE("GaussianAO") {
    constexpr coord<double> r0{1.23, 4.56, 7.89};
    constexpr std::array cs{2.34, 3.45, 4.56};
    constexpr std::array as{5.67, 6.78, 7.89};
    std::array ps{Gaussian<double>(cs[0], as[0], r0),
                  Gaussian<double>(cs[1], as[1], r0),
                  Gaussian<double>(cs[2], as[2], r0)};
    StaticGaussianAO<double, 3> bf(cs, as, r0);

    REQUIRE(bf.size() == 3);
    for(std::size_t i = 0; i < 3; ++i) REQUIRE(bf[i] == ps[i]);

    REQUIRE(bf.center()[0] == r0[0]);
    REQUIRE(bf.center()[1] == r0[1]);
    REQUIRE(bf.center()[2] == r0[2]);

    SECTION("Iterators") {
        std::size_t i = 0;
        for(auto pi : bf) REQUIRE(pi == ps[i++]);
        i = 0;
        for(auto pi = bf.cbegin(); pi != bf.cend(); ++pi, ++i)
            REQUIRE(*pi == ps[i]);
    }

    SECTION("Can change coef") {
        bf[0].coef() = 9.9;
        REQUIRE(bf[0].coef() == 9.9);
    }
    SECTION("Can change exp") {
        bf[0].exp() = 8.8;
        REQUIRE(bf[0].exp() == 8.8);
    }
    SECTION("Can change function's center") {
        std::array r1{1.1, 2.2, 3.3};
        bf.center() = r1;
        REQUIRE(bf.center() == r1);
        for(auto prim : bf) REQUIRE(prim.center() == r1);
        SECTION("Can't change through prim") {
            using center_type = decltype(bf[0].center());
            using no_ref      = std::remove_reference_t<center_type>;
            STATIC_REQUIRE(std::is_const_v<no_ref>);
        }
    }
    SECTION("Copy Assignment") {
        StaticGaussianAO<double, 3> bf2(as, cs, r0);
        auto pbf2 = &(bf2 = bf);
        REQUIRE(pbf2 == &bf2);
        REQUIRE(bf2 == bf);
    }
    SECTION("Move Assignment") {
        StaticGaussianAO<double, 3> bf2(bf);
        StaticGaussianAO<double, 3> bf3(as, cs, r0);
        auto pbf3 = &(bf3 = std::move(bf));
        REQUIRE(pbf3 == &bf3);
        REQUIRE(bf2 == bf3);
    }
}
