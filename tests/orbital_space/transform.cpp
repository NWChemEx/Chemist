#include "libchemist/orbital_space/transform.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;
using namespace libchemist::detail_;

/* Testing strategy.
 *
 * The transform functions only really rely on the transformation matrices of
 * the DerivedSpace instances. AOSpace instances are essentially placeholders
 * so that we know which modes to transform. For simplicity we assume that the
 * modes of the tensor that we are transforming are all of length 2. We do let
 * the
 */

namespace {

template<typename T>
auto make_space22(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23}, {0.34, 0.45}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space23(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23, 0.45}, {0.56, 0.67, 0.78}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space32(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23}, {0.45, 0.56}, {0.67, 0.78}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space24(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23, 0.45, 0.56}, {0.67, 0.78, 0.89, 0.90}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space43(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23, 0.45},
                              {0.56, 0.67, 0.78},
                              {0.89, 0.90, 0.01},
                              {0.12, 0.23, 0.34}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space25(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23, 0.45, 0.56, 0.67},
                              {0.67, 0.78, 0.89, 0.90, 0.01}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

}

using scalar_types = std::tuple<float, double>;

TEMPLATE_LIST_TEST_CASE("do_which_first (2 spaces)", "", scalar_types){
    using T = TestType;
    auto& world = TA::get_default_world();
    auto s23 = make_space23<T>(world);
    auto s24 = make_space24<T>(world);

    SECTION("Same size"){
        REQUIRE_FALSE(do_which_first(s23, s23));
    }

    SECTION("rows less than columns") {
        SECTION("First is smaller") { REQUIRE_FALSE(do_which_first(s23, s24)); }
        SECTION("Second is smaller") { REQUIRE(do_which_first(s24, s23)); }
    }

    SECTION("columns less than rows") {
        auto s32 = make_space32<T>(world);
        auto s43 = make_space43<T>(world);

        SECTION("First is smaller"){ REQUIRE_FALSE(do_which_first(s32, s43)); }
        SECTION("Second is smaller") { REQUIRE(do_which_first(s43, s32)); }
    }
}

TEMPLATE_LIST_TEST_CASE("do_which_first (3 spaces)", "", scalar_types){
    using T = TestType;
    auto& world = TA::get_default_world();
    auto s22 = make_space22<T>(world);
    auto s23 = make_space23<T>(world);
    auto s24 = make_space24<T>(world);

    SECTION("Same size"){
        REQUIRE(do_which_first(s23, s23, s23) == 0);
    }

    SECTION("First is smallest") {
        REQUIRE(do_which_first(s22, s23, s24) == 0);
        REQUIRE(do_which_first(s22, s24, s23) == 0);
    }

    SECTION("Second is smallest") {
        REQUIRE(do_which_first(s23, s22, s24) == 1);
        REQUIRE(do_which_first(s24, s22, s23) == 1);
    }
    SECTION("Third is smallest") {
        REQUIRE(do_which_first(s23, s24, s22) == 2);
        REQUIRE(do_which_first(s24, s23, s22) == 2);
    }
}

TEMPLATE_LIST_TEST_CASE("do_which_first (4 spaces)", "", scalar_types){
    using T = TestType;
    auto& world = TA::get_default_world();
    auto s22 = make_space22<T>(world);
    auto s23 = make_space23<T>(world);
    auto s24 = make_space24<T>(world);
    auto s25 = make_space25<T>(world);
    SECTION("Same size"){
        REQUIRE(do_which_first(s23, s23, s23, s23) == 0);
    }

    SECTION("First is smallest") {
        REQUIRE(do_which_first(s22, s23, s24, s25) == 0);
        REQUIRE(do_which_first(s22, s23, s25, s24) == 0);
        REQUIRE(do_which_first(s22, s24, s23, s25) == 0);
        REQUIRE(do_which_first(s22, s24, s25, s23) == 0);
        REQUIRE(do_which_first(s22, s25, s23, s24) == 0);
        REQUIRE(do_which_first(s22, s25, s24, s23) == 0);
    }

    SECTION("Second is smallest") {
        REQUIRE(do_which_first(s23, s22, s24, s25) == 1);
        REQUIRE(do_which_first(s23, s22, s25, s24) == 1);
        REQUIRE(do_which_first(s24, s22, s23, s25) == 1);
        REQUIRE(do_which_first(s24, s22, s25, s23) == 1);
        REQUIRE(do_which_first(s25, s22, s23, s24) == 1);
        REQUIRE(do_which_first(s25, s22, s24, s23) == 1);
    }
    SECTION("Third is smallest") {
        REQUIRE(do_which_first(s23, s24, s22, s25) == 2);
        REQUIRE(do_which_first(s23, s25, s22, s24) == 2);
        REQUIRE(do_which_first(s24, s23, s22, s25) == 2);
        REQUIRE(do_which_first(s24, s25, s22, s23) == 2);
        REQUIRE(do_which_first(s25, s23, s22, s24) == 2);
        REQUIRE(do_which_first(s25, s24, s22, s24) == 2);
    }
    SECTION("Fourth is smallest") {
        REQUIRE(do_which_first(s23, s24, s25, s22) == 3);
        REQUIRE(do_which_first(s23, s25, s24, s22) == 3);
        REQUIRE(do_which_first(s24, s23, s25, s22) == 3);
        REQUIRE(do_which_first(s24, s25, s23, s22) == 3);
        REQUIRE(do_which_first(s25, s23, s24, s22) == 3);
        REQUIRE(do_which_first(s25, s24, s23, s22) == 3);
    }
}
