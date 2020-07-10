#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist;

TEST_CASE("allclose") {
    auto &world = TA::get_default_world();
    SECTION("identical") {
        TA::TSpArrayD lhs(world, {{0.0, 1.1},
                                  {2.2, 3.3}});
        TA::TSpArrayD corr(world, {{0.0, 1.1},
                                   {2.2, 3.3}});
        REQUIRE(allclose(lhs, corr));
    }

    SECTION("Absolute difference 0.1") {
        TA::TSpArrayD lhs(world, {{0.0, 1.1},
                                  {2.2, 3.3}});
        TA::TSpArrayD corr(world, {{0.0, 1.2},
                                   {2.2, 3.3}});

        SECTION("Close if atol == 0.1") {
            REQUIRE(allclose(lhs, corr, 0.0, 0.1));
        }

        SECTION("Not close if atol < 0.1") {
            REQUIRE_FALSE(allclose(lhs, corr, 0.0, 0.09));
        }
    }

    SECTION("Relative difference 0.1") {
        TA::TSpArrayD lhs(world, {{0.0, 1.09},
                                  {2.2, 3.3}});
        TA::TSpArrayD corr(world, {{0.0, 1.2},
                                   {2.2, 3.3}});

        SECTION("Close if rtol == 0.1") {
            REQUIRE(allclose(lhs, corr, 0.1));
        }

        SECTION("Not close if rtol < 0.1") {
            REQUIRE_FALSE(allclose(lhs, corr, 0.09));
        }
    }

    SECTION("Actual is Sparse Tensor with missing blocks") {
        TA::TiledRange trange{{0, 2},
                              {0, 2}};

        auto l = [](TA::TensorD &, const TA::Range &) { return 0.0; };
        auto lhs = TA::make_array<TA::TSpArrayD>(world, trange, l);

        SECTION("Close to explicitly zero tensor") {
            TA::TSpArrayD corr(world, {{0.0, 0.0},
                                       {0.0, 0.0}});
            REQUIRE(allclose(lhs, corr));
        }

        SECTION("Not close to non-zero tensor") {
            TA::TSpArrayD corr(world, {{0.0, 1.1},
                                       {2.2, 3.3}});
            REQUIRE_FALSE(allclose(lhs, corr));
        }
    }

    SECTION("Correct is Sparse Tensor with missing blocks") {
        TA::TiledRange trange{{0, 2},
                              {0, 2}};

        auto l = [](TA::TensorD &, const TA::Range &) { return 0.0; };
        auto corr = TA::make_array<TA::TSpArrayD>(world, trange, l);

        SECTION("Close to explicitly zero tensor") {
            TA::TSpArrayD lhs(world, {{0.0, 0.0},
                                      {0.0, 0.0}});
            REQUIRE(allclose(lhs, corr));
        }

        SECTION("Not close to non-zero tensor") {
            TA::TSpArrayD lhs(world, {{0.0, 1.1},
                                      {2.2, 3.3}});
            // TODO: Re-enable after TA #184 is resolved
            //REQUIRE_FALSE(allclose(lhs, corr));
        }
    }
}

TEST_CASE("Elementwise helpers") {

    auto& world = TA::get_default_world();
    TA::TSpArrayD matrix(world,{{1.0,2.0},{3.0,4.0}});
    TA::TSpArrayD corr(world,{{2.0,4.0},{6.0,8.0}});

    SECTION("apply_elementwise") {
        auto doubled = apply_elementwise(matrix, [](const double& old) {
            return 2*old;
        });
        REQUIRE(allclose(doubled,corr));
    }

    SECTION("apply_elementwise_inplace") {
        apply_elementwise_inplace(matrix, [](double& value) {
            value *= 2;
        });
        REQUIRE(allclose(matrix,corr));
    }
}
