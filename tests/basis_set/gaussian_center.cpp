#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_center.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr GaussianCenter") {
    constexpr std::array cs0{1.23, 2.34, 3.45};
    constexpr std::array as0{4.56, 5.67, 6.78};
    constexpr std::array cs1{7.89, 8.90};
    constexpr std::array as1{9.01, 0.12};
    constexpr GaussianShell s0{true, 2, cs0, as0};
    constexpr GaussianShell s1{false, 3, cs1, as1};
    constexpr GaussianCenter c{s0, s1};
    STATIC_REQUIRE_FALSE(c.has_center());
    STATIC_REQUIRE(c.nshells() == 2);
    STATIC_REQUIRE(c.shell(0) == s0);
    STATIC_REQUIRE(c.shell(1) == s1);

    SECTION("Comparisons") {
        SECTION("Same") {
            constexpr GaussianCenter c2{s0, s1};
            STATIC_REQUIRE(c == c2);
            STATIC_REQUIRE_FALSE(c != c2);
        }
        SECTION("Different Number of Shells") {
            constexpr GaussianCenter c2{s0};
            STATIC_REQUIRE(c != c2);
            STATIC_REQUIRE_FALSE(c == c2);
        }
        SECTION("Different Shells") {
            constexpr GaussianCenter c2{s0, s0};
            STATIC_REQUIRE(c != c2);
            STATIC_REQUIRE_FALSE(c == c2);
        }
    }
}

TEST_CASE("GaussianCenter()") {
    constexpr std::array cs0{1.23, 2.34, 3.45};
    constexpr std::array as0{4.56, 5.67, 6.78};
    GaussianCenter c;

    SECTION("Throws if center not set") {
        REQUIRE_THROWS_AS(c.center(), std::bad_optional_access);
    }

    SECTION("Can set center") {
        c.set_center(cs0);
        REQUIRE(c.center() == cs0);
    }
}

TEST_CASE("GaussianCenter(s0, s1)") {
    constexpr std::array cs0{1.23, 2.34, 3.45};
    constexpr std::array as0{4.56, 5.67, 6.78};
    constexpr std::array cs1{7.89, 8.90};
    constexpr std::array as1{9.01, 0.12};
    constexpr GaussianShell s0{true, 2, cs0, as0};
    constexpr GaussianShell s1{false, 3, cs1, as1};
    GaussianCenter c{s0, s1};
    REQUIRE_FALSE(c.has_center());
    REQUIRE(c.nshells() == 2);
    REQUIRE(c.shell(0) == s0);
    REQUIRE(c.shell(1) == s1);

    SECTION("Can change purity") {
        c.shell(0).pure() = false;
        REQUIRE(c.shell(0).naos() == 6);
    }
    SECTION("Can change l") {
        c.shell(0).l() = 4;
        REQUIRE(c.shell(0).l() == 4);
    }
}
