#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist;

TEST_CASE("get_block_idx"){
    auto& world = TA::get_default_world();
    SECTION("Vector"){
        TA::TiledRange trange{{0, 1, 2}};
        TA::TSpArrayD t(world, trange, {0, 1});
        for(std::size_t i = 0; i < 2; ++i){
            auto idx = get_block_idx(t, t.find(i).get());
            std::vector<std::size_t> corr{i};
            REQUIRE(idx == corr);
        }
    }

    SECTION("Matrix"){
        TA::TiledRange trange{{0, 1, 2}, {0, 1, 2}};
        TA::TSpArrayD t(world, trange, {{0, 1}, {2, 3}});
        for(std::size_t i = 0; i < 2; ++i){
            for(std::size_t j = 0; j < 2; ++j){
                std::vector<std::size_t> corr{i, j};
                auto idx = get_block_idx(t, t.find(corr).get());
                REQUIRE(idx == corr);
            }
        }
    }

    SECTION("Tensor"){
        TA::TiledRange trange{{0, 1, 2}, {0, 1, 2}, {0, 1, 2}};
        TA::TSpArrayD t(world, trange, {{{0, 1}, {2, 3}},
                                        {{4, 5}, {6, 7}}});
        for(std::size_t i = 0; i < 2; ++i){
            for(std::size_t j = 0; j < 2; ++j){
                for(std::size_t k = 0; k < 2; ++k) {
                    std::vector<std::size_t> corr{i, j, k};
                    auto idx = get_block_idx(t, t.find(corr).get());
                    REQUIRE(idx == corr);
                }
            }
        }
    }
}

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

TEST_CASE("from_sparse_map(SparseMap, DistArray, TiledRange, size_t)"){
    using namespace sparse_map;
    using index_type = typename SparseMap::key_type;

    auto& world = TA::get_default_world();
    TA::TiledRange trange{{0, 1, 2}, {0, 1, 2}};
    TA::TSpArrayD t(world, trange, {{1, 2}, {3, 4}});

    SECTION("One independent index") {
        TA::TiledRange corr_trange{{0, 1}, {0, 1, 2}, {0, 1, 2}};
        SECTION("One block") {
            TA::TSpArrayD corr(world, corr_trange, {{{0, 0}, {0, 4}}});
            SparseMap sm{{index_type{0}, {index_type{1, 1}}}};
            auto r = from_sparse_map(sm, t, TA::TiledRange1{0, 1});
            REQUIRE(allclose(r, corr));
        }

        SECTION("Two blocks") {
            TA::TSpArrayD corr(world, corr_trange, {{{0, 2}, {0, 4}}});
            SparseMap sm{{index_type{0}, {index_type{0, 1}, index_type{1, 1}}}};
            auto r = from_sparse_map(sm, t, TA::TiledRange1{0, 1});
            REQUIRE(allclose(r, corr));
        }
    }

    SECTION("Two independent indices") {
        TA::TiledRange corr_trange{{0, 1, 2}, {0, 1, 2}, {0, 1, 2}};
        SECTION("One block") {
            TA::TSpArrayD corr(world, corr_trange, {{{0, 0}, {0, 4}},
                                                    {{1, 0}, {0, 0}}});
            SparseMap sm{{index_type{0}, {index_type{1, 1}}},
                         {index_type{1}, {index_type{0, 0}}}};
            auto r = from_sparse_map(sm, t, TA::TiledRange1{0, 1, 2});
            REQUIRE(allclose(r, corr));
        }

        SECTION("Two blocks") {
            TA::TSpArrayD corr(world, corr_trange, {{{0, 0}, {3, 4}},
                                                     {{1, 2}, {0, 0}}});
            SparseMap sm{{index_type{0}, {index_type{1, 0}, index_type{1, 1}}},
                         {index_type{1}, {index_type{0, 0}, index_type{0, 1}}}};
            auto r = from_sparse_map(sm, t, TA::TiledRange1{0, 1, 2});
            REQUIRE(allclose(r, corr));
        }
    }
}

TEST_CASE("from_sparse_map(SparseMap, DistArray, size_t)"){
    using namespace sparse_map;
    using index_type = typename SparseMap::key_type;

    auto& world = TA::get_default_world();
    TA::TiledRange trange{{0, 1, 2}, {0, 1, 2}};
    TA::TSpArrayD t(world, trange, {{1, 2}, {3, 4}});

    SECTION("One independent index") {
        SECTION("One block") {
            SparseMap sm{{index_type{0}, {index_type{1}}}};

            SECTION("mode == 0") {
                TA::TSpArrayD corr(world, trange, {{0, 2}, {0, 0}});
                auto r = from_sparse_map(sm, t, 0);
                REQUIRE(allclose(r, corr));
            }

            SECTION("mode == 1") {
                TA::TSpArrayD corr(world, trange, {{0, 0}, {3, 0}});
                auto r = from_sparse_map(sm, t, 1);
                REQUIRE(allclose(r, corr));
            }
        }

        SECTION("Two blocks") {
            SparseMap sm{{index_type{0}, {index_type{0}, index_type{1}}}};

            SECTION("mode == 0") {
                TA::TSpArrayD corr(world, trange, {{1, 2}, {0, 0}});
                auto r = from_sparse_map(sm, t, 0);
                REQUIRE(allclose(r, corr));
            }

            SECTION("mode == 1") {
                TA::TSpArrayD corr(world, trange, {{1, 0}, {3, 0}});
                auto r = from_sparse_map(sm, t, 1);
                REQUIRE(allclose(r, corr));
            }
        }
    }

    SECTION("Two independent indices") {
        TA::TiledRange corr_trange{{0, 1, 2}, {0, 1, 2}};
        SECTION("One block") {
            SparseMap sm{{index_type{0}, {index_type{1}}},
                         {index_type{1}, {index_type{0}}}};

            SECTION("mode == 0") {
                TA::TSpArrayD corr(world, trange, {{0, 2}, {3, 0}});
                auto r = from_sparse_map(sm, t, 0);
                REQUIRE(allclose(r, corr));
            }

            SECTION("mode == 1") {
                TA::TSpArrayD corr(world, trange, {{0, 2}, {3, 0}});
                auto r = from_sparse_map(sm, t, 1);
                REQUIRE(allclose(r, corr));
            }
        }

        SECTION("Two blocks") {
            TA::TSpArrayD corr(world, corr_trange, {{1, 2}, {3, 4}});
            SparseMap sm{{index_type{0}, {index_type{0}, index_type{1}}},
                         {index_type{1}, {index_type{0}, index_type{1}}}};

            SECTION("mode == 0") {
                auto r = from_sparse_map(sm, t, 0);
                REQUIRE(allclose(r, corr));
            }

            SECTION("mode == 1") {
                auto r = from_sparse_map(sm, t, 1);
                REQUIRE(allclose(r, corr));
            }
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