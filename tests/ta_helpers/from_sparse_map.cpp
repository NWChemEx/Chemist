#include<catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist;




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
