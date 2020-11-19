#include "libchemist/orbital_space/transform.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;
using namespace libchemist::detail_;

/* Testing strategy.
 *
 * The transform functions only really rely on the transformation matrices of
 * the DerivedSpace instances. AOSpace instances are essentially placeholders
 * so that we know which modes to transform. For simplicity we assume that the
 * modes of the tensor that we are transforming are all of length 2. We do let
 * the transform lengths vary though.
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
    TA::TSpArray<T> c(world,
                      {{0.12, 0.23, 0.45, 0.56}, {0.67, 0.78, 0.89, 0.90}});
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
    TA::TSpArray<T> c(
      world, {{0.12, 0.23, 0.45, 0.56, 0.67}, {0.67, 0.78, 0.89, 0.90, 0.01}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

} // namespace

using ta_helpers::allclose;
using scalar_types = std::tuple<float, double>;

TEMPLATE_LIST_TEST_CASE("do_which_first (2 spaces)", "", scalar_types) {
    using T     = TestType;
    auto& world = TA::get_default_world();
    auto s23    = make_space23<T>(world);
    auto s24    = make_space24<T>(world);

    SECTION("Same size") { REQUIRE_FALSE(do_which_first(s23, s23)); }

    SECTION("rows less than columns") {
        SECTION("First is smaller") { REQUIRE_FALSE(do_which_first(s23, s24)); }
        SECTION("Second is smaller") { REQUIRE(do_which_first(s24, s23)); }
    }

    SECTION("columns less than rows") {
        auto s32 = make_space32<T>(world);
        auto s43 = make_space43<T>(world);

        SECTION("First is smaller") { REQUIRE_FALSE(do_which_first(s32, s43)); }
        SECTION("Second is smaller") { REQUIRE(do_which_first(s43, s32)); }
    }
}

TEMPLATE_LIST_TEST_CASE("do_which_first (3 spaces)", "", scalar_types) {
    using T     = TestType;
    auto& world = TA::get_default_world();
    auto s22    = make_space22<T>(world);
    auto s23    = make_space23<T>(world);
    auto s24    = make_space24<T>(world);

    SECTION("Same size") { REQUIRE(do_which_first(s23, s23, s23) == 0); }

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

TEMPLATE_LIST_TEST_CASE("do_which_first (4 spaces)", "", scalar_types) {
    using T     = TestType;
    auto& world = TA::get_default_world();
    auto s22    = make_space22<T>(world);
    auto s23    = make_space23<T>(world);
    auto s24    = make_space24<T>(world);
    auto s25    = make_space25<T>(world);
    SECTION("Same size") { REQUIRE(do_which_first(s23, s23, s23, s23) == 0); }

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

TEMPLATE_LIST_TEST_CASE("transform(matrix)", "", scalar_types) {
    using T     = TestType;
    auto& world = TA::get_default_world();
    TA::TSpArray<T> I(world, {{1.0, 2.0}, {3.0, 4.0}});

    auto c23 = make_space23<T>(world);
    auto c24 = make_space24<T>(world);
    auto aos = c23.from_space();
    SECTION("AO AO") { REQUIRE(allclose(I, transform(aos, aos, I))); }
    SECTION("Derived AO") {
        auto rv = transform(c23, aos, I);
        auto corr =
          TA::TSpArray<T>(world, {{1.8, 2.48}, {2.24, 3.14}, {2.79, 4.02}});
        REQUIRE(allclose(corr, rv));
    }
    SECTION("AO Derived") {
        auto rv   = transform(aos, c24, I);
        auto corr = TA::TSpArray<T>(
          world, {{1.46, 1.79, 2.23, 2.36}, {3.04, 3.81, 4.91, 5.28}});
        REQUIRE(allclose(corr, rv));
    }
    SECTION("Derived Derived") {
        auto rv   = transform(c23, c24, I);
        auto corr = TA::TSpArray<T>(world, {{1.8776, 2.3484, 3.0172, 3.24},
                                            {2.3726, 2.9644, 3.8026, 4.0804},
                                            {3.0282, 3.7773, 4.8333, 5.1804}});
        REQUIRE(allclose(corr, rv));
    }
}

TEMPLATE_LIST_TEST_CASE("transform(rank 3)", "", scalar_types) {
    using T     = TestType;
    auto& world = TA::get_default_world();
    TA::TSpArray<T> I(world,
                      {{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}});

    auto s22 = make_space22<T>(world);
    auto s23 = make_space23<T>(world);
    auto s24 = make_space24<T>(world);
    auto aos = s22.from_space();

    SECTION("AO AO AO") {
        REQUIRE(allclose(libchemist::transform(aos, aos, aos, I), I));
    }

    SECTION("Derived AO AO") {
        auto rv = transform(s22, aos, aos, I);
        TA::TSpArray<T> corr(
          world, {{{1.82, 2.28}, {2.74, 3.2}}, {{2.48, 3.16}, {3.84, 4.52}}});
        REQUIRE(allclose(corr, rv));
    }

    SECTION("AO Derived AO") {
        auto rv = transform(aos, s23, aos, I);
        TA::TSpArray<T> corr(world, {{{1.8000000000000003, 2.4800000000000004},
                                      {2.24, 3.14},
                                      {2.79, 4.0200000000000005}},
                                     {{4.5200000000000005, 5.2},
                                      {5.840000000000001, 6.74},
                                      {7.71, 8.940000000000001}}});
        REQUIRE(allclose(corr, rv));
    }

    SECTION("AO AO Derived") {
        auto rv = libchemist::transform(aos, aos, s24, I);
        TA::TSpArray<T> corr(
          world, {{{1.46, 1.79, 2.23, 2.36}, {3.04, 3.81, 4.91, 5.28}},
                  {{4.62, 5.83, 7.59, 8.2}, {6.2, 7.85, 10.27, 11.12}}});
        REQUIRE(allclose(corr, rv));
    }

    SECTION("Derived Derived AO") {
        auto rv = libchemist::transform(s22, s23, aos, I);
        TA::TSpArray<T> corr(world, {{{1.7528000000000001, 2.0656000000000003},
                                      {2.2544000000000004, 2.6684},
                                      {2.9562000000000004, 3.5220000000000002}},
                                     {{2.4480000000000004, 2.9104000000000005},
                                      {3.1432000000000007, 3.7552000000000003},
                                      {4.1112, 4.9476}}});
        REQUIRE(allclose(corr, rv));
    }

    SECTION("Derived AO Derived") {
        auto rv = libchemist::transform(s22, aos, s24, I);
        TA::TSpArray<T> corr(
          world,
          {{{1.746, 2.197, 2.8482000000000003, 3.0712},
            {2.4728000000000003, 3.1262000000000008, 4.081,
             4.4144000000000005}},
           {{2.4148000000000005, 3.0352000000000006, 3.9284, 4.232800000000001},
            {3.4892000000000003, 4.4088, 5.7508, 6.218400000000001}}});
        REQUIRE(allclose(corr, rv));
    }

    SECTION("AO Derived Derived") {
        auto rv = libchemist::transform(aos, s23, s24, I);
        TA::TSpArray<T> corr(world, {{{1.8776000000000002, 2.3484000000000003,
                                       3.0172000000000003, 3.2400000000000007},
                                      {2.3726000000000003, 2.9644000000000004,
                                       3.8026000000000004, 4.080400000000001},
                                      {3.0282000000000004, 3.7773000000000003,
                                       4.8332999999999995, 5.180400000000001}},
                                     {{4.026400000000001, 5.095600000000001,
                                       6.662000000000001, 7.211200000000002},
                                      {5.2166000000000015, 6.6004000000000005,
                                       8.6266, 9.336400000000001},
                                      {6.915000000000001, 8.746500000000001,
                                       11.4261, 12.363600000000002}}});
        REQUIRE(allclose(corr, rv));
    }

    SECTION("Derived Derived Derived") {
        auto rv = libchemist::transform(s22, s23, s24, I);
        TA::TSpArray<T> corr(world,
                             {{{1.5942880000000004, 2.0143120000000003,
                                2.6271440000000004, 2.8406080000000005},
                               {2.0583560000000003, 2.5998640000000006,
                                3.3893560000000007, 3.664024000000001},
                               {2.7144840000000006, 3.4270860000000005, 4.46487,
                                4.825272000000002}},
                              {{2.243728000000001, 2.8331520000000006,
                                3.6918560000000005, 3.990240000000001},
                               {2.8931680000000006, 3.651992000000001,
                                4.7565680000000015, 5.139872},
                               {3.808236000000001, 4.804704000000001,
                                6.2534040000000015, 6.7551120000000004}}});
        REQUIRE(allclose(corr, rv));
    }
}
