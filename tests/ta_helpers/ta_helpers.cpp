#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist;

TEST_CASE("add_tiled_dimension"){
    TA::TiledRange1 tr1{0, 1, 2};
    SECTION("Add to empty range"){
        auto r = add_tiled_dimension(TA::TiledRange{}, tr1);
        TA::TiledRange corr{tr1};
        REQUIRE(corr == r);
    }

    SECTION("Add to vector range"){
        TA::TiledRange tr{{0, 1}};

        SECTION("Before existing"){
            auto r = add_tiled_dimension(tr, tr1);
            TA::TiledRange corr{{0, 1, 2}, {0, 1}};
            REQUIRE(corr == r);
        }

        SECTION("After existing"){
            auto r = add_tiled_dimension(tr, tr1, 1);
            TA::TiledRange corr{{0, 1}, {0, 1, 2}};
            REQUIRE(corr == r);
        }
    }

    SECTION("Add to matrix range"){
        TA::TiledRange tr{{0, 1}, {0, 1, 2, 3}};

        SECTION("Before existing"){
            auto r = add_tiled_dimension(tr, tr1);
            TA::TiledRange corr{{0, 1, 2}, {0, 1}, {0, 1, 2, 3}};
            REQUIRE(corr == r);
        }

        SECTION("Between existing"){
            auto r = add_tiled_dimension(tr, tr1, 1);
            TA::TiledRange corr{{0, 1}, {0, 1, 2}, {0, 1, 2, 3}};
            REQUIRE(corr == r);
        }

        SECTION("After existing"){
            auto r = add_tiled_dimension(tr, tr1, 2);
            TA::TiledRange corr{{0, 1}, {0, 1, 2, 3}, {0, 1, 2}};
            REQUIRE(corr == r);
        }
    }
}


TEST_CASE("allclose"){
    auto& world = TA::get_default_world();
    SECTION("identical"){
        TA::TSpArrayD lhs(world, {{0.0, 1.1}, {2.2, 3.3}});
        TA::TSpArrayD corr(world, {{0.0, 1.1}, {2.2, 3.3}});
        REQUIRE(allclose(lhs, corr));
    }

    SECTION("Absolute difference 0.1"){
        TA::TSpArrayD lhs(world, {{0.0, 1.1}, {2.2, 3.3}});
        TA::TSpArrayD corr(world, {{0.0, 1.2}, {2.2, 3.3}});

        SECTION("Close if atol == 0.1") {
            REQUIRE(allclose(lhs, corr, 0.0, 0.1));
        }

        SECTION("Not close if atol < 0.1"){
            REQUIRE_FALSE(allclose(lhs, corr, 0.0, 0.09));
        }
    }

    SECTION("Relative difference 0.1"){
        TA::TSpArrayD lhs(world, {{0.0, 1.09}, {2.2, 3.3}});
        TA::TSpArrayD corr(world, {{0.0, 1.2}, {2.2, 3.3}});

        SECTION("Close if rtol == 0.1") {
            REQUIRE(allclose(lhs, corr, 0.1));
        }

        SECTION("Not close if rtol < 0.1"){
            REQUIRE_FALSE(allclose(lhs, corr, 0.09));
        }
    }

    SECTION("Actual is Sparse Tensor with missing blocks"){
        TA::TiledRange trange{{0, 2}, {0, 2}};

        auto l = [](TA::TensorD&, const TA::Range&){ return 0.0; };
        auto lhs = TA::make_array<TA::TSpArrayD>(world, trange, l);

        SECTION("Close to explicitly zero tensor") {
            TA::TSpArrayD corr(world, {{0.0, 0.0}, {0.0, 0.0}});
            REQUIRE(allclose(lhs, corr));
        }

        SECTION("Not close to non-zero tensor") {
            TA::TSpArrayD corr(world, {{0.0, 1.1}, {2.2, 3.3}});
            REQUIRE_FALSE(allclose(lhs, corr));
        }
    }

    SECTION("Correct is Sparse Tensor with missing blocks"){
        TA::TiledRange trange{{0, 2}, {0, 2}};

        auto l = [](TA::TensorD&, const TA::Range&){ return 0.0; };
        auto corr = TA::make_array<TA::TSpArrayD>(world, trange, l);

        SECTION("Close to explicitly zero tensor") {
            TA::TSpArrayD lhs(world, {{0.0, 0.0}, {0.0, 0.0}});
            REQUIRE(allclose(lhs, corr));
        }

        SECTION("Not close to non-zero tensor") {
            TA::TSpArrayD lhs(world, {{0.0, 1.1}, {2.2, 3.3}});
            // TODO: Re-enable after TA #184 is resolved
            //REQUIRE_FALSE(allclose(lhs, corr));
        }
    }
}
