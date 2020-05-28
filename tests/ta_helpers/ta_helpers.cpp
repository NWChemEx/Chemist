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

TEST_CASE("Retile") {
    using vector_t   = std::initializer_list<double>;
    using matrix_t   = std::initializer_list<vector_t>;

    auto& world = TA::get_default_world();

    matrix_t some_values = {{0.1, 0.2, 0.3, 0.4, 0.5},
                            {0.6, 0.7, 0.8, 0.9, 1.0},
                            {1.1, 1.2, 1.3, 1.4, 1.5},
                            {1.6, 1.7, 1.8, 1.9, 2.0},
                            {2.1, 2.2, 2.3, 2.4, 2.5}};

    auto range0  = TA::TiledRange1(0, 3, 5);
    auto range1  = TA::TiledRange1(0, 5);
    auto trange0 = TA::TiledRange({range0, range0});
    auto trange1 = TA::TiledRange({range0, range1});

    TA::TArrayD default_dense(world, some_values);
    TA::TArrayD specific_dense(world, trange0, some_values);
    TA::TArrayD another_dense(world, trange1, some_values);

    TA::TSpArrayD default_sparse(world, some_values);
    TA::TSpArrayD specific_sparse(world, trange0, some_values);
    TA::TSpArrayD another_sparse(world, trange1, some_values);

    auto result_dense = libchemist::retile(default_dense, trange0);
    REQUIRE(allclose(result_dense, specific_dense));

    result_dense = libchemist::retile(result_dense, trange1);
    REQUIRE(allclose(result_dense, another_dense));

    auto result_sparse = libchemist::retile(default_sparse, trange0);
    REQUIRE(allclose(result_sparse, specific_sparse));

    result_sparse = libchemist::retile(result_sparse, trange1);
    REQUIRE(allclose(result_sparse, another_sparse));
}
