#include <catch2/catch.hpp>
#include <libchemist/basis_set/detail_/basis_set_traits.hpp>

using namespace libchemist::basis_set::detail_;

TEST_CASE("get_value") {
    SECTION("double&") {
        double x{1.23};
        REQUIRE(&(get_value(x)) == &x);
    }
    SECTION("const double&") {
        const double x{1.23};
        REQUIRE(&(get_value(x)) == &x);
    }
    SECTION("constexpr double&") {
        constexpr const double x{1.23};
        STATIC_REQUIRE(&(get_value(x)) == &x);
    }
    SECTION("double*") {
        double x{1.23};
        REQUIRE(&(get_value(&x)) == &x);
    }
    SECTION("const double*") {
        const double x{1.23};
        REQUIRE(&(get_value(&x)) == &x);
    }
    SECTION("constexpr double*") {
        constexpr double x{1.23};
        STATIC_REQUIRE(&(get_value(&x)) == &x);
    }
    SECTION("std::array<double, 3>&") {
        std::array<double, 3> r0{1.23, 4.56, 7.89};
        REQUIRE(&(get_value(r0)) == &r0);
    }
    SECTION("const std::array<double, 3>&") {
        std::array<double, 3> r0{1.23, 4.56, 7.89};
        REQUIRE(&(get_value(r0)) == &r0);
    }
    SECTION("constexpr std::array<double, 3>&") {
        constexpr std::array<double, 3> r0{1.23, 4.56, 7.89};
        STATIC_REQUIRE(&(get_value(r0)) == &r0);
    }
    SECTION("std::array<double, 3>*") {
        std::array<double, 3> r0{1.23, 4.56, 7.89};
        REQUIRE(&(get_value(&r0)) == &r0);
    }
    SECTION("const std::array<double, 3>*") {
        std::array<double, 3> r0{1.23, 4.56, 7.89};
        REQUIRE(&(get_value(&r0)) == &r0);
    }
    SECTION("constexpr std::array<double, 3>*") {
        constexpr std::array<double, 3> r0{1.23, 4.56, 7.89};
        STATIC_REQUIRE(&(get_value(&r0)) == &r0);
    }
}
