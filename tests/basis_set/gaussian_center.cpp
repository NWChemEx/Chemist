#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_center.hpp>

using namespace libchemist::basis_set;

TEST_CASE("constexpr GaussianCenter") {
    constexpr std::array cs{1.23, 2.34, 3.45};
    constexpr std::array as{4.56, 5.67, 6.78};
    constexpr std::array r0{1.1, 2.2, 3.3};
    constexpr StaticGaussianShell<double, 3> s(2, cs, as, r0);
    constexpr std::array ss{s, s, s};
    constexpr auto center = make_gaussian_center(r0, ss);
    STATIC_REQUIRE(center.size() == 3);
    STATIC_REQUIRE(center.center()[0] == r0[0]);
    STATIC_REQUIRE(center.center()[1] == r0[1]);
    STATIC_REQUIRE(center.center()[2] == r0[2]);
    // STATIC_REQUIRE(center[0] == s);

    //    SECTION("Can change l"){
    //        s.l() = 4;
    //        REQUIRE(s.l() == 4);
    //    }
    //    SECTION("Can't change coef"){
    //        using coef_type = std::remove_reference_t<decltype(s.coef(0, 0))>;
    //        STATIC_REQUIRE(std::is_const_v<coef_type>);
    //    }
    //    SECTION("Can't change exp"){
    //        using exp_type = std::remove_reference_t<decltype(s.exp(0, 0))>;
    //        STATIC_REQUIRE(std::is_const_v<exp_type>);
    //    }
    //    SECTION("Can change center"){
    //        std::array r1{4.4, 5.5, 6.6};
    //        s.center() = r1;
    //        REQUIRE(s.center() == r1);
    //    }
}
