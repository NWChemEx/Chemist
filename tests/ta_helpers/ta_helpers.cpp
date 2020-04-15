#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using tensor_type = TA::TSpArrayD;

TEST_CASE("TA Helpers") {

    auto& world = TA::get_default_world();
    tensor_type matrix(world,{{1.0,2.0},{3.0,4.0}});
    tensor_type corr(world,{{2.0,4.0},{6.0,8.0}});

    SECTION("apply_elementwise") {
        auto doubled = libchemist::apply_elementwise(matrix, [](const double& old) {
            return 2*old;
        });
        REQUIRE(libchemist::allclose(doubled,corr));
    }

    SECTION("apply_elementwise_inplace") {
        libchemist::apply_elementwise_inplace(matrix, [](double& value) {
            value *= 2;
        });
        REQUIRE(libchemist::allclose(matrix,corr));
    }
}