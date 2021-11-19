#include "../test_orbital_space.hpp"
#include "../transform_data.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "chemist/orbital_space/derived_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist::orbital_space;
using namespace chemist::test;

TEMPLATE_TEST_CASE("tensor_transform", "", double, float) {
    using tensor_type = type::tensor_of_tensors<TestType>;
    using from_space  = BaseSpace_<tensor_type>;
    using base_type   = BaseSpace_<tensor_type>;
    using space_type  = DerivedSpace_<tensor_type, from_space, base_type>;
    using scalar_type = typename tensor_type::scalar_type;

    TA::World& world = TA::get_default_world();

    TA::TSpArray<scalar_type> I(world, {{1.0, 2.0}, {3.0, 4.0}});
    auto c23 = make_space23<scalar_type>(world);
    auto c24 = make_space24<scalar_type>(world);

    SECTION("modes{}") { REQUIRE(compare_tensors(c23.transform(I, {}), I)); }

    SECTION("modes{0}") {
        auto corr = TA::TSpArray<scalar_type>(
          world, {{1.8, 2.48}, {2.24, 3.14}, {2.79, 4.02}});
        REQUIRE(compare_tensors(c23.transform(I, {0}), corr));
    }
    SECTION("modes{1}") {
        auto corr = TA::TSpArray<scalar_type>(
          world, {{1.46, 1.79, 2.23, 2.36}, {3.04, 3.81, 4.91, 5.28}});
        REQUIRE(compare_tensors(c24.transform(I, {1}), corr));
    }
    SECTION("modes{0,1}") {
        auto corr = TA::TSpArray<scalar_type>(world, {{1.6048, 2.0756, 2.7444},
                                                      {2.0272, 2.619, 3.4572},
                                                      {2.586, 3.3351, 4.3911}});
        REQUIRE(compare_tensors(c23.transform(I, {0, 1}), corr));
    }
    SECTION("modes{0,2}") {
        REQUIRE_THROWS_AS(c23.transform(I, {0, 2}), std::runtime_error);
    }
}