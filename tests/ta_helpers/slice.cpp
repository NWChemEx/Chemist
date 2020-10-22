#include "libchemist/ta_helpers/slice.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::ta_helpers;
using scalar_types = std::tuple<float, double>;
using idx_type     = libchemist::sparse_map::ElementIndex;

TEST_CASE("is_empty_slice_") {
    using namespace libchemist::ta_helpers::detail_;

    idx_type e1{1}, e12{1, 2}, e2{2}, e23{2, 3};

    SECTION("Empty") {
        SECTION("Vector") { REQUIRE(is_empty_slice_(e1, e1)); }
        SECTION("Matrix") { REQUIRE(is_empty_slice_(e12, e12)); }
    }

    SECTION("Non-empty") {
        SECTION("Vector") { REQUIRE_FALSE(is_empty_slice_(e1, e2)); }
        SECTION("Matrix") { REQUIRE_FALSE(is_empty_slice_(e12, e23)); }
    }
}

TEST_CASE("get_slice_tile_indices_") {
    using namespace libchemist::ta_helpers::detail_;
    using tile_idx = libchemist::sparse_map::TileIndex;

    SECTION("vector") {
        TA::TiledRange tr{{0, 2, 4}};
        idx_type e0{0}, e1{1}, e2{2}, e3{3};
        SECTION("Block 0") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e0, e1);
            REQUIRE(r0 == tile_idx{0});
            REQUIRE(r1 == tile_idx{1});
        }

        SECTION("Block 1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e2, e3);
            REQUIRE(r0 == tile_idx{1});
            REQUIRE(r1 == tile_idx{2});
        }

        SECTION("Blocks 0 and 1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e0, e3);
            REQUIRE(r0 == tile_idx{0});
            REQUIRE(r1 == tile_idx{2});
        }
    }

    SECTION("matrix") {
        TA::TiledRange tr{{0, 2, 4}, {0, 3, 6}};
        idx_type e00{0, 0}, e03{0, 3}, e12{1, 2}, e15{1, 5};
        idx_type e20{2, 0}, e23{2, 3}, e32{3, 2}, e35{3, 5};
        SECTION("Block 0,0") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e00, e12);
            REQUIRE(r0 == tile_idx{0, 0});
            REQUIRE(r1 == tile_idx{1, 1});
        }

        SECTION("Block 0,1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e03, e15);
            REQUIRE(r0 == tile_idx{0, 1});
            REQUIRE(r1 == tile_idx{1, 2});
        }

        SECTION("Block 1,0") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e20, e32);
            REQUIRE(r0 == tile_idx{1, 0});
            REQUIRE(r1 == tile_idx{2, 1});
        }

        SECTION("Block 1,1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e23, e35);
            REQUIRE(r0 == tile_idx{1, 1});
            REQUIRE(r1 == tile_idx{2, 2});
        }

        SECTION("Blocks 0,0 and 0,1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e00, e15);
            REQUIRE(r0 == tile_idx{0, 0});
            REQUIRE(r1 == tile_idx{1, 2});
        }

        SECTION("Blocks 0,0 and 1,0") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e00, e32);
            REQUIRE(r0 == tile_idx{0, 0});
            REQUIRE(r1 == tile_idx{2, 1});
        }

        SECTION("Blocks 0,1 and 1,1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e03, e35);
            REQUIRE(r0 == tile_idx{0, 1});
            REQUIRE(r1 == tile_idx{2, 2});
        }

        SECTION("Blocks 0,0 through 1,1") {
            auto [r0, r1] = get_slice_tile_indices_(tr, e00, e35);
            REQUIRE(r0 == tile_idx{0, 0});
            REQUIRE(r1 == tile_idx{2, 2});
        }
    }
}

TEMPLATE_LIST_TEST_CASE("slice", "", scalar_types) {
    using tensor_type = TA::TSpArray<TestType>;
    auto& world       = TA::get_default_world();

    TA::detail::vector_il<TestType> vil{1, 2, 3, 4, 5, 6};
    TA::detail::matrix_il<TestType> mil{{1, 2, 3, 4, 5, 6},
                                        {7, 8, 9, 10, 11, 12},
                                        {13, 14, 15, 16, 17, 18},
                                        {19, 20, 21, 22, 23, 24}};
    std::map<std::string, tensor_type> vs;
    vs["one tile vector"] = tensor_type(world, vil);
    vs["two tile vector"] = tensor_type(world, TA::TiledRange{{0, 3, 6}}, vil);

    std::map<std::string, tensor_type> ms;
    TA::TiledRange mtr{{0, 2, 4}, {0, 3, 6}};
    ms["one tile matrix"]  = tensor_type(world, mil);
    ms["four tile matrix"] = tensor_type(world, mtr, mil);

    SECTION("Slice is entire tensor") {
        for(const auto& [k, t] : vs) {
            SECTION(k) {
                auto r = slice(t, idx_type{0}, idx_type{6});
                REQUIRE(allclose(r, t));
            }
        }

        for(const auto& [k, t] : ms) {
            SECTION(k) {
                auto r = slice(t, idx_type{0, 0}, idx_type{4, 6});
                REQUIRE(allclose(r, t));
            }
        }
    }

    SECTION("Slice is on tile boundaries") {
        SECTION("two tile vector") {
            const auto& t = vs.at("two tile vector");
            tensor_type corr;

            SECTION("Block 0") {
                auto r    = slice(t, idx_type{0}, idx_type{3});
                corr("i") = t("i").block({0}, {1});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 1") {
                auto r    = slice(t, idx_type{3}, idx_type{6});
                corr("i") = t("i").block({1}, {2});
                REQUIRE(allclose(r, corr));
            }
        }

        SECTION("four tile matrix") {
            const auto& t = ms.at("four tile matrix");
            tensor_type corr;

            SECTION("Block 0,0") {
                auto r      = slice(t, idx_type{0, 0}, idx_type{2, 3});
                corr("i,j") = t("i,j").block({0, 0}, {1, 1});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 0,1") {
                auto r      = slice(t, idx_type{0, 3}, idx_type{2, 6});
                corr("i,j") = t("i,j").block({0, 1}, {1, 2});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 1,0") {
                auto r      = slice(t, idx_type{2, 0}, idx_type{4, 3});
                corr("i,j") = t("i,j").block({1, 0}, {2, 1});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 1,1") {
                auto r      = slice(t, idx_type{2, 3}, idx_type{4, 6});
                corr("i,j") = t("i,j").block({1, 1}, {2, 2});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 0,0 and 0,1") {
                auto r      = slice(t, idx_type{0, 0}, idx_type{2, 6});
                corr("i,j") = t("i,j").block({0, 0}, {1, 2});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 0,0 and 1,0") {
                auto r      = slice(t, idx_type{0, 0}, idx_type{4, 3});
                corr("i,j") = t("i,j").block({0, 0}, {2, 1});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 0,1 and 1,1") {
                auto r      = slice(t, idx_type{0, 3}, idx_type{4, 6});
                corr("i,j") = t("i,j").block({0, 1}, {2, 2});
                REQUIRE(allclose(r, corr));
            }

            SECTION("Block 1,0 and 1,1") {
                auto r      = slice(t, idx_type{2, 0}, idx_type{4, 6});
                corr("i,j") = t("i,j").block({1, 0}, {2, 2});
                REQUIRE(allclose(r, corr));
            }
        }
    }

    SECTION("Lower index is on boundary") {
        SECTION("one tile vector") {
            const auto& t = vs.at("one tile vector");
            SECTION("[0, 1)") {
                auto r = slice(t, idx_type{0}, idx_type{1});
                tensor_type corr(world, {1});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[0, 2)") {
                auto r = slice(t, idx_type{0}, idx_type{2});
                tensor_type corr(world, {1, 2});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[0, 3)") {
                auto r = slice(t, idx_type{0}, idx_type{3});
                tensor_type corr(world, {1, 2, 3});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[0, 4)") {
                auto r = slice(t, idx_type{0}, idx_type{4});
                tensor_type corr(world, {1, 2, 3, 4});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[0, 5)") {
                auto r = slice(t, idx_type{0}, idx_type{5});
                tensor_type corr(world, {1, 2, 3, 4, 5});
                REQUIRE(allclose(r, corr));
            }
        }

        SECTION("one tile matrix") {
            const auto& t = ms.at("one tile matrix");
            SECTION("[(0,0), (1,1))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{1, 1});
                tensor_type corr(world, {{1}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (1,2))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{1, 2});
                tensor_type corr(world, {{1, 2}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (1,3))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{1, 3});
                tensor_type corr(world, {{1, 2, 3}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (1,4))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{1, 4});
                tensor_type corr(world, {{1, 2, 3, 4}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (1,5))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{1, 5});
                tensor_type corr(world, {{1, 2, 3, 4, 5}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (1,6))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{1, 6});
                tensor_type corr(world, {{1, 2, 3, 4, 5, 6}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (2,1))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{2, 1});
                tensor_type corr(world, {{1}, {7}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (2,2))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{2, 2});
                tensor_type corr(world, {{1, 2}, {7, 8}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (2,3))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{2, 3});
                tensor_type corr(world, {{1, 2, 3}, {7, 8, 9}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (2,4))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{2, 4});
                tensor_type corr(world, {{1, 2, 3, 4}, {7, 8, 9, 10}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (2,5))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{2, 5});
                tensor_type corr(world, {{1, 2, 3, 4, 5}, {7, 8, 9, 10, 11}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (2,6))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{2, 6});
                tensor_type corr(world, {{1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (3,1))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{3, 1});
                tensor_type corr(world, {{1}, {7}, {13}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (3,2))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{3, 2});
                tensor_type corr(world, {{1, 2}, {7, 8}, {13, 14}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (3,3))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{3, 3});
                tensor_type corr(world, {{1, 2, 3}, {7, 8, 9}, {13, 14, 15}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (3,4))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{3, 4});
                tensor_type corr(
                  world, {{1, 2, 3, 4}, {7, 8, 9, 10}, {13, 14, 15, 16}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (3,5))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{3, 5});
                tensor_type corr(
                  world,
                  {{1, 2, 3, 4, 5}, {7, 8, 9, 10, 11}, {13, 14, 15, 16, 17}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (3,6))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{3, 6});
                tensor_type corr(
                  world,
                  {{1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12}, {13, 14, 15, 16, 17, 18}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (4,1))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{4, 1});
                tensor_type corr(
                  world,
                  {{1}, {7}, {13}, {19}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (4,2))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{4, 2});
                tensor_type corr(
                  world,
                  {{1, 2}, {7, 8}, {13, 14}, {19, 20}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (4,3))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{4, 3});
                tensor_type corr(
                  world,
                  {{1, 2, 3}, {7, 8, 9}, {13, 14, 15}, {19, 20, 21}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (4,4))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{4, 4});
                tensor_type corr(
                  world,
                  {{1, 2, 3, 4}, {7, 8 , 9, 10}, {13, 14, 15, 16}, {19, 20, 21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,0), (4,5))") {
                auto r = slice(t, idx_type{0, 0}, idx_type{4, 5});
                tensor_type corr(
                  world,
                  {{1, 2, 3, 4, 5}, {7, 8 , 9, 10, 11}, {13, 14, 15, 16, 17}, {19, 20, 21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }
        }
    }

    SECTION("Upper index is on boundary") {
        SECTION("one tile vector") {
            const auto& t = vs.at("one tile vector");
            SECTION("[1, 6)") {
                auto r = slice(t, idx_type{1}, idx_type{6});
                tensor_type corr(world, {2, 3, 4, 5, 6});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[3, 6)") {
                auto r = slice(t, idx_type{3}, idx_type{6});
                tensor_type corr(world, {4, 5, 6});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[5, 6)") {
                auto r = slice(t, idx_type{5}, idx_type{6});
                tensor_type corr(world, {6});
                REQUIRE(allclose(r, corr));
            }
        }

        SECTION("one tile matrix") {
            const auto& t = ms.at("one tile matrix");
            SECTION("[(0,1), (4,6))") {
                auto r = slice(t, idx_type{0, 1}, idx_type{4, 6});
                tensor_type corr(world, {{2, 3, 4, 5, 6}, {8, 9, 10, 11, 12}, {14, 15, 16, 17, 18}, {20, 21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,2), (4,6))") {
                auto r = slice(t, idx_type{0, 2}, idx_type{4, 6});
                tensor_type corr(world, {{3, 4, 5, 6}, {9, 10, 11, 12}, {15, 16, 17, 18}, {21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,3), (4,6))") {
                auto r = slice(t, idx_type{0, 3}, idx_type{4, 6});
                tensor_type corr(world, {{4, 5, 6}, {10, 11, 12}, {16, 17, 18}, {22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,4), (4,6))") {
                auto r = slice(t, idx_type{0, 4}, idx_type{4, 6});
                tensor_type corr(world, {{5, 6}, {11, 12}, {17, 18}, {23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,5), (4,6))") {
                auto r = slice(t, idx_type{0, 5}, idx_type{4, 6});
                tensor_type corr(world, {{6}, {12}, {18}, {24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,1), (4,6))") {
                auto r = slice(t, idx_type{1, 1}, idx_type{4, 6});
                tensor_type corr(world, {{8, 9, 10, 11, 12}, {14, 15, 16, 17, 18}, {20, 21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,2), (4,6))") {
                auto r = slice(t, idx_type{1, 2}, idx_type{4, 6});
                tensor_type corr(world, {{9, 10, 11, 12}, {15, 16, 17, 18}, {21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,3), (4,6))") {
                auto r = slice(t, idx_type{1, 3}, idx_type{4, 6});
                tensor_type corr(world, {{10, 11, 12}, {16, 17, 18}, {22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,4), (4,6))") {
                auto r = slice(t, idx_type{1, 4}, idx_type{4, 6});
                tensor_type corr(world, {{11, 12}, {17, 18}, {23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,5), (4,6))") {
                auto r = slice(t, idx_type{1, 5}, idx_type{4, 6});
                tensor_type corr(world, {{12}, {18}, {24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,1), (4,6))") {
                auto r = slice(t, idx_type{2, 1}, idx_type{4, 6});
                tensor_type corr(world, {{14, 15, 16, 17, 18}, {20, 21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,2), (4,6))") {
                auto r = slice(t, idx_type{2, 2}, idx_type{4, 6});
                tensor_type corr(world, {{15, 16, 17, 18}, {21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,3), (4,6))") {
                auto r = slice(t, idx_type{2, 3}, idx_type{4, 6});
                tensor_type corr(world, {{16, 17, 18}, {22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,4), (4,6))") {
                auto r = slice(t, idx_type{2, 4}, idx_type{4, 6});
                tensor_type corr(world, {{17, 18}, {23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,5), (4,6))") {
                auto r = slice(t, idx_type{2, 5}, idx_type{4, 6});
                tensor_type corr(world, {{18}, {24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,1), (4,6))") {
                auto r = slice(t, idx_type{3, 1}, idx_type{4, 6});
                tensor_type corr(world, {{20, 21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,2), (4,6))") {
                auto r = slice(t, idx_type{3, 2}, idx_type{4, 6});
                tensor_type corr(world, {{21, 22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,3), (4,6))") {
                auto r = slice(t, idx_type{3, 3}, idx_type{4, 6});
                tensor_type corr(world, {{22, 23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,4), (4,6))") {
                auto r = slice(t, idx_type{3, 4}, idx_type{4, 6});
                tensor_type corr(world, {{23, 24}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,5), (4,6))") {
                auto r = slice(t, idx_type{3, 5}, idx_type{4, 6});
                tensor_type corr(world, {{24}});
                REQUIRE(allclose(r, corr));
            }
        }
    }

    SECTION("Neither index is on boundary") {
        SECTION("one tile vector") {
            const auto& t = vs.at("one tile vector");
            SECTION("[1, 3)") {
                auto r = slice(t, idx_type{1}, idx_type{3});
                tensor_type corr(world, {2, 3});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[1, 5)") {
                auto r = slice(t, idx_type{1}, idx_type{5});
                tensor_type corr(world, {2, 3, 4, 5});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[3, 5)") {
                auto r = slice(t, idx_type{3}, idx_type{5});
                tensor_type corr(world, {4, 5});
                REQUIRE(allclose(r, corr));
            }
        }

        SECTION("one tile matrix") {
            const auto& t = ms.at("one tile matrix");
            SECTION("[(0,1), (4,5))") {
                auto r = slice(t, idx_type{0, 1}, idx_type{4, 5});
                tensor_type corr(world, {{2, 3, 4, 5}, {8, 9, 10, 11}, {14, 15, 16, 17}, {20, 21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,2), (4,5))") {
                auto r = slice(t, idx_type{0, 2}, idx_type{4, 5});
                tensor_type corr(world, {{3, 4, 5}, {9, 10, 11}, {15, 16, 17}, {21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,3), (4,5))") {
                auto r = slice(t, idx_type{0, 3}, idx_type{4, 5});
                tensor_type corr(world, {{4, 5}, {10, 11}, {16, 17}, {22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,4), (4,5))") {
                auto r = slice(t, idx_type{0, 4}, idx_type{4, 5});
                tensor_type corr(world, {{5}, {11}, {17}, {23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,1), (4,5))") {
                auto r = slice(t, idx_type{1, 1}, idx_type{4, 5});
                tensor_type corr(world, {{8, 9, 10, 11}, {14, 15, 16, 17}, {20, 21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,2), (4,5))") {
                auto r = slice(t, idx_type{1, 2}, idx_type{4, 5});
                tensor_type corr(world, {{9, 10, 11}, {15, 16, 17}, {21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,3), (4,5))") {
                auto r = slice(t, idx_type{1, 3}, idx_type{4, 5});
                tensor_type corr(world, {{10, 11}, {16, 17}, {22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,4), (4,5))") {
                auto r = slice(t, idx_type{1, 4}, idx_type{4, 5});
                tensor_type corr(world, {{11}, {17}, {23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,1), (4,5))") {
                auto r = slice(t, idx_type{2, 1}, idx_type{4, 5});
                tensor_type corr(world, {{14, 15, 16, 17}, {20, 21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,2), (4,5))") {
                auto r = slice(t, idx_type{2, 2}, idx_type{4, 5});
                tensor_type corr(world, {{15, 16, 17}, {21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,3), (4,5))") {
                auto r = slice(t, idx_type{2, 3}, idx_type{4, 5});
                tensor_type corr(world, {{16, 17}, {22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,4), (4,5))") {
                auto r = slice(t, idx_type{2, 4}, idx_type{4, 5});
                tensor_type corr(world, {{17}, {23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,1), (4,5))") {
                auto r = slice(t, idx_type{3, 1}, idx_type{4, 5});
                tensor_type corr(world, {{20, 21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,2), (4,5))") {
                auto r = slice(t, idx_type{3, 2}, idx_type{4, 5});
                tensor_type corr(world, {{21, 22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,3), (4,5))") {
                auto r = slice(t, idx_type{3, 3}, idx_type{4, 5});
                tensor_type corr(world, {{22, 23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,4), (4,5))") {
                auto r = slice(t, idx_type{3, 4}, idx_type{4, 5});
                tensor_type corr(world, {{23}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,1), (4,4))") {
                auto r = slice(t, idx_type{0, 1}, idx_type{4, 4});
                tensor_type corr(world, {{2, 3, 4}, {8, 9, 10}, {14, 15, 16}, {20, 21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,2), (4,4))") {
                auto r = slice(t, idx_type{0, 2}, idx_type{4, 4});
                tensor_type corr(world, {{3, 4}, {9, 10}, {15, 16}, {21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(0,3), (4,4))") {
                auto r = slice(t, idx_type{0, 3}, idx_type{4, 4});
                tensor_type corr(world, {{4}, {10}, {16}, {22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,1), (4,4))") {
                auto r = slice(t, idx_type{1, 1}, idx_type{4, 4});
                tensor_type corr(world, {{8, 9, 10}, {14, 15, 16}, {20, 21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,2), (4,4))") {
                auto r = slice(t, idx_type{1, 2}, idx_type{4, 4});
                tensor_type corr(world, {{9, 10}, {15, 16}, {21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(1,3), (4,4))") {
                auto r = slice(t, idx_type{1, 3}, idx_type{4, 4});
                tensor_type corr(world, {{10}, {16}, {22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,1), (4,4))") {
                auto r = slice(t, idx_type{2, 1}, idx_type{4, 4});
                tensor_type corr(world, {{14, 15, 16}, {20, 21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,2), (4,4))") {
                auto r = slice(t, idx_type{2, 2}, idx_type{4, 4});
                tensor_type corr(world, {{15, 16}, {21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(2,3), (4,4))") {
                auto r = slice(t, idx_type{2, 3}, idx_type{4, 4});
                tensor_type corr(world, {{16}, {22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,1), (4,4))") {
                auto r = slice(t, idx_type{3, 1}, idx_type{4, 4});
                tensor_type corr(world, {{20, 21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,2), (4,4))") {
                auto r = slice(t, idx_type{3, 2}, idx_type{4, 4});
                tensor_type corr(world, {{21, 22}});
                REQUIRE(allclose(r, corr));
            }

            SECTION("[(3,3), (4,4))") {
                auto r = slice(t, idx_type{3, 3}, idx_type{4, 4});
                tensor_type corr(world, {{22}});
                REQUIRE(allclose(r, corr));
            }

            /* There's a lot more that I didn't test, but I'm tired...*/
        }
    }
}
