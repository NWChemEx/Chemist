#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_center.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr GaussianCenter") {
    constexpr std::array cs{1.23, 2.34, 3.45};
    constexpr std::array as{4.56, 5.67, 6.78};
    constexpr std::array nprim{0ul, 2ul, 3ul};
    constexpr std::array pure{true, false};
    constexpr std::array l{2, 3};
    constexpr std::array r0{1.1, 2.2, 3.3};
    constexpr auto center = make_gaussian_center(r0, l, pure, nprim, cs, as);
    constexpr auto s1     = make_gaussian_shell(2, std::array{1.23, 2.34},
                                            std::array{4.56, 5.67}, r0, true);
    constexpr auto s2 =
      make_gaussian_shell(3, std::array{3.45}, std::array{6.78}, r0, false);
    STATIC_REQUIRE(center.size() == 2);
    STATIC_REQUIRE(center.center()[0] == r0[0]);
    STATIC_REQUIRE(center.center()[1] == r0[1]);
    STATIC_REQUIRE(center.center()[2] == r0[2]);
    STATIC_REQUIRE(center[0] == s1);
    STATIC_REQUIRE(center[1] == s2);

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr auto c2 =
              make_gaussian_center(r0, l, pure, nprim, cs, as);
            STATIC_REQUIRE(center == c2);
            STATIC_REQUIRE_FALSE(center != c2);
        }
        SECTION("Different number of shells") {
            constexpr auto c2 = make_gaussian_center(
              r0, l, pure, std::array{0ul, 1ul, 2ul, 3ul}, cs, as);
            STATIC_REQUIRE(center != c2);
            STATIC_REQUIRE_FALSE(center == c2);
        }
        SECTION("Different shells") {
            constexpr auto c2 =
              make_gaussian_center(r0, l, pure, nprim, as, cs);
            STATIC_REQUIRE(center != c2);
            STATIC_REQUIRE_FALSE(center == c2);
        }
        SECTION("Different centers") {
            constexpr std::array r1{4.4, 5.5, 6.6};
            constexpr auto c2 =
              make_gaussian_center(r1, l, pure, nprim, cs, as);
            STATIC_REQUIRE(center != c2);
            STATIC_REQUIRE_FALSE(center == c2);
        }
    }

    SECTION("Copy Ctor") {
        constexpr GaussianCenter_ c2(center);
        STATIC_REQUIRE(center == c2);
    }
    SECTION("Move Ctor") {
        constexpr GaussianCenter_ c2(center);
        constexpr GaussianCenter_ c3(std::move(center));
        STATIC_REQUIRE(c2 == c3);
    }
}

TEST_CASE("GaussianCenter") {
    const std::array cs{1.23, 2.34, 3.45};
    const std::array as{4.56, 5.67, 6.78};
    std::array nprim{0ul, 2ul, 3ul};
    std::array pure{true, false};
    std::array l{2, 3};
    std::array r0{1.1, 2.2, 3.3};
    auto center = make_gaussian_center(r0, l, pure, nprim, &cs, &as);
    auto s1     = make_gaussian_shell(2, std::array{1.23, 2.34},
                                  std::array{4.56, 5.67}, r0, true);
    auto s2 =
      make_gaussian_shell(3, std::array{3.45}, std::array{6.78}, r0, false);

    REQUIRE(center.center() == r0);
    REQUIRE(center.size() == 2);
    std::size_t counter = 0;
    for(const auto& shelli : center) {
        if(counter == 0)
            REQUIRE(shelli == s1);
        else
            REQUIRE(shelli == s2);
        ++counter;
    }

    SECTION("Can change center") {
        center.center()[0] = 9.9;
        REQUIRE(center.center()[0] == 9.9);
    }
    SECTION("Can't change center through shell") {
        using c_type = std::remove_reference_t<decltype(center[0].center())>;
        STATIC_REQUIRE(std::is_const_v<c_type>);
    }
    SECTION("Can change shell's angular momentum") {
        center[0].l() = 4;
        REQUIRE(center[0].l() == 4);
    }
    SECTION("Can change shell's purity") {
        center[0].pure() = false;
        REQUIRE_FALSE(center[0].pure());
    }
    SECTION("Can't change coef") {
        using c_type = decltype(center[0][0][0].coef());
        using no_ref = std::remove_reference_t<c_type>;
        STATIC_REQUIRE(std::is_const_v<no_ref>);
    }
    SECTION("Can't change exp") {
        using e_type = decltype(center[0][0][0].exp());
        using no_ref = std::remove_reference_t<e_type>;
        STATIC_REQUIRE(std::is_const_v<no_ref>);
    }
    SECTION("Copy Assignment") {
        auto c2  = make_gaussian_center(r0, l, pure, nprim, &as, &as);
        auto pc2 = &(c2 = center);
        REQUIRE(pc2 == &c2);
        REQUIRE(center == c2);
    }
    SECTION("Move Ctor") {
        GaussianCenter_ c2(center);
        GaussianCenter_ c3 = make_gaussian_center(r0, l, pure, nprim, &as, &as);
        auto pc3           = &(c3 = std::move(center));
        REQUIRE(&c3 == pc3);
        REQUIRE(c2 == c3);
    }
}
