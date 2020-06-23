#include <catch2/catch.hpp>
#include "libchemist/sparse_map/detail_/from_sparse_map_helpers.hpp"

using namespace libchemist::sparse_map::detail_;
using index_type = std::vector<std::size_t>;

TEST_CASE("extract_dep_range_"){

    SECTION("Vector"){
        TA::Range r{{1}, {3}};

        SECTION("All modes are dependent"){
            ToTMapper mapper(1);
            auto rv = extract_dep_range_(r, mapper);
            REQUIRE(rv == r);
        }

        SECTION("Mode 0 is independent"){
            ToTMapper mapper(1, {{0, 0}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr;
            REQUIRE(rv == corr);
        }
    }

    SECTION("Matrix"){
        TA::Range r{{1, 2}, {5, 4}};

        SECTION("All modes dependent"){
            ToTMapper mapper(2);
            auto rv = extract_dep_range_(r, mapper);
            REQUIRE(rv == r);
        }

        SECTION("Mode 0 is independent"){
            ToTMapper mapper(2, {{0, 0}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{2}, {4}};
            REQUIRE(rv == corr);
        }

        SECTION("Mode 1 is independent"){
            ToTMapper mapper(2, {{0, 1}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{1}, {5}};
            REQUIRE(rv == corr);
        }

        SECTION("Mode 0 and 1 are independent"){
            ToTMapper mapper(2, {{0, 0}, {1, 1}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr;
            REQUIRE(rv == corr);
        }
    }

    SECTION("Rank 3 tensor") {
        TA::Range r{{1, 2, 3}, {6, 5, 4}};

        SECTION("all modes dependent") {
            ToTMapper mapper(3);
            auto rv = extract_dep_range_(r, mapper);
            REQUIRE(rv == r);
        }

        SECTION("Mode 0 is independent"){
            ToTMapper mapper(3, {{1, 0}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{2, 3}, {5, 4}};
            REQUIRE(rv == corr);
        }

        SECTION("Mode 1 is independent"){
            ToTMapper mapper(3, {{1, 1}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{1, 3}, {6, 4}};
            REQUIRE(rv == corr);
        }

        SECTION("Mode 2 is independent"){
            ToTMapper mapper(3, {{1, 2}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{1, 2}, {6, 5}};
            REQUIRE(rv == corr);
        }

        SECTION("Modes 0 and 1 are independent"){
            ToTMapper mapper(3, {{0, 0}, {1, 1}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{3}, {4}};
            REQUIRE(rv == corr);
        }

        SECTION("Modes 0 and 2 are independent"){
            ToTMapper mapper(3, {{0, 0}, {1, 2}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{2}, {5}};
            REQUIRE(rv == corr);
        }

        SECTION("Modes 1 and 2 are independent"){
            ToTMapper mapper(3, {{0, 2}, {1, 1}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr{{1}, {6}};
            REQUIRE(rv == corr);
        }

        SECTION("All modes are independent"){
            ToTMapper mapper(3,{{0, 0}, {1, 1}, {2, 2}});
            auto rv = extract_dep_range_(r, mapper);
            TA::Range corr;
            REQUIRE(rv == corr);
        }
    } // Rank 3 Tensor
} // extract_dep_range_

/*
 * The mapping of the input index goes through the mapper, so as long as the
 * mapper works we don't need to test permutations of the input index etc.
 */
TEST_CASE("get_tiles_"){
    auto& world = TA::get_default_world();

    SECTION("Vector"){
        TA::detail::vector_il<double> il{0, 0, 1, 1, 1, 2, 2, 2, 2};
        TA::TSpArrayD v(world, TA::TiledRange{{0, 2, 5, 9}}, il);

        SECTION("All dependent"){
            libchemist::sparse_map::Domain d{{0}, {2}};
            ToTMapper mapper(1);
            TA::TensorD t0(TA::Range{2}, {0, 0});
            TA::TensorD t2(TA::Range{{5}, {9}}, {2, 2, 2, 2});
            std::vector<TA::TensorD> corr{t0, t2};
            auto r = get_tiles_(index_type{0}, d, v, mapper);
            REQUIRE(corr == r);
        }
    }

    SECTION("Matrix"){
        TA::detail::matrix_il<double> il{{0, 0, 1, 2, 2},
                                         {0, 0, 1, 2, 2},
                                         {3, 3, 4, 5, 5},
                                         {6, 6, 7, 8, 8},
                                         {6, 6, 7, 8, 8}};
        TA::TiledRange1 r{0, 2, 3, 5};
        TA::TSpArrayD m(world, TA::TiledRange{r, r}, il);

        SECTION("All dependent"){
            libchemist::sparse_map::Domain d{{0, 1}, {1, 1}, {2, 0}};
            ToTMapper mapper(2);
            TA::TensorD t01(TA::Range{{0, 2}, {2, 3}}, {1, 1});
            TA::TensorD t11(TA::Range{{2, 2}, {3, 3}}, {4});
            TA::TensorD t21(TA::Range{{3, 0}, {5, 2}}, {6, 6, 6, 6});
            std::vector<TA::TensorD> corr{t01, t11, t21};
            auto r = get_tiles_(index_type{0}, d, m, mapper);
            REQUIRE(corr == r);
        }

        SECTION("Mode 0 is independent"){
            libchemist::sparse_map::Domain d{{0}, {2}};
            ToTMapper mapper(2, {{1, 0}});
            TA::TensorD t00(TA::Range{{0, 0}, {2, 2}}, {0, 0, 0, 0});
            TA::TensorD t02(TA::Range{{0, 3}, {2, 5}}, {2, 2, 2, 2});
            std::vector<TA::TensorD> corr{t00, t02};
            auto r = get_tiles_(index_type{1, 0}, d, m, mapper);
            REQUIRE(corr == r);
        }

        SECTION("Mode 1 is independent"){
            libchemist::sparse_map::Domain d{{0}, {2}};
            ToTMapper mapper(2, {{1, 1}});
            TA::TensorD t00(TA::Range{{0, 0}, {2, 2}}, {0, 0, 0, 0});
            TA::TensorD t20(TA::Range{{3, 0}, {5, 2}}, {6, 6, 6, 6});
            std::vector<TA::TensorD> corr{t00, t20};
            auto r = get_tiles_(index_type{1, 0}, d, m, mapper);
            REQUIRE(corr == r);
        }
    }

    SECTION("Rank 3 tensor"){
        TA::detail::tensor3_il<double> il{{{0, 0, 1}, {0, 0, 1}, {3, 3, 4}},
                                          {{0, 0, 1}, {0, 0, 1}, {3, 3, 4}},
                                          {{5, 5, 6}, {5, 5, 6}, {7, 7, 8}}};
        TA::TiledRange1 r{0, 2, 3};
        TA::TSpArrayD t(world, TA::TiledRange{r, r, r}, il);

        SECTION("All dependent"){
            libchemist::sparse_map::Domain d{{0, 0, 0}, {1, 1, 1}};
            ToTMapper mapper(3);
            TA::TensorD t000(TA::Range{{0, 0, 0}, {2, 2, 2}}, {0, 0, 0, 0, 0, 0, 0, 0});
            TA::TensorD t111(TA::Range{{2, 2, 2}, {3, 3, 3}}, {8});
            std::vector<TA::TensorD> corr{t000, t111};
            auto r = get_tiles_(index_type{0}, d, t, mapper);
            REQUIRE(corr == r);
        }
    }
}

TEST_CASE("make_inner_tile_range_"){

    SECTION("Vector"){
        TA::TensorD t0(TA::Range({0}, {3}), {0, 0, 0});
        TA::TensorD t1(TA::Range({3}, {5}), {1, 1});
        std::vector tiles{t0, t1};
        ToTMapper mapper(1);
        auto r = make_inner_tile_range_(tiles, mapper);
        TA::Range corr({5});
        REQUIRE(r == corr);
    }

    SECTION("Matrix"){
        TA::TensorD t01(TA::Range{{0, 2}, {2, 3}}, {1, 1});
        TA::TensorD t11(TA::Range{{2, 2}, {3, 3}}, {4});
        TA::TensorD t21(TA::Range{{3, 0}, {5, 2}}, {6, 6, 6, 6});
        std::vector tiles{t01, t11, t21};
        SECTION("All dependent"){
            ToTMapper mapper(2);
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0, 0}, {5, 3});
            REQUIRE(r == corr);
        }

        SECTION("Mode 0 is independent"){
            ToTMapper mapper(2, {{1, 0}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0}, {3});
            REQUIRE(r == corr);
        }

        SECTION("Mode 1 is independent"){
            ToTMapper mapper(2, {{1, 1}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0}, {5});
            REQUIRE(r == corr);
        }

    }

    SECTION("Rank 3 tensor"){
        TA::TensorD t000(TA::Range{{0, 0, 0}, {2, 2, 2}}, {0, 0, 0, 0, 0, 0, 0, 0});
        TA::TensorD t111(TA::Range{{2, 2, 2}, {3, 3, 3}}, {8});
        std::vector tiles{t000, t111};

        SECTION("All dependent"){
            ToTMapper mapper(3);
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0, 0, 0}, {3, 3, 3});
            REQUIRE(corr == r);
        }

        SECTION("Mode 0 is independent"){
            ToTMapper mapper(3, {{0, 0}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0, 0}, {3, 3});
            REQUIRE(corr == r);
        }

        SECTION("Mode 1 is independent"){
            ToTMapper mapper(3, {{0, 1}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0, 0}, {3, 3});
            REQUIRE(corr == r);
        }

        SECTION("Mode 2 is independent"){
            ToTMapper mapper(3, {{0, 2}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0, 0}, {3, 3});
            REQUIRE(corr == r);
        }

        SECTION("Modes 0 and 1 are independent"){
            ToTMapper mapper(3, {{0, 0}, {1, 1}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0}, {3});
            REQUIRE(corr == r);
        }

        SECTION("Modes 0 and 2 are independent"){
            ToTMapper mapper(3, {{0, 0}, {1, 2}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0}, {3});
            REQUIRE(corr == r);
        }

        SECTION("Modes 1 and 2 are independent"){
            ToTMapper mapper(3, {{0, 1}, {1, 2}});
            auto r = make_inner_tile_range_(tiles, mapper);
            TA::Range corr({0}, {3});
            REQUIRE(corr == r);
        }
    }

}

TEST_CASE("make_inner_tile_"){

    SECTION("Vector"){
        TA::TensorD t0(TA::Range({0}, {3}), {0, 0, 0});
        TA::TensorD t1(TA::Range({3}, {5}), {1, 1});
        std::vector tiles{t0, t1};

        SECTION("All dependent modes") {
            ToTMapper mapper(1);
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::TensorD corr(TA::Range{5}, {0, 0, 0, 1, 1});
            REQUIRE(r == corr);
        }
    }

    SECTION("Matrix"){
        TA::TensorD t01(TA::Range{{0, 2}, {2, 3}}, {1, 1});
        TA::TensorD t11(TA::Range{{2, 2}, {3, 3}}, {4});
        TA::TensorD t21(TA::Range{{3, 0}, {5, 2}}, {6, 6, 6, 6});


        SECTION("All dependent modes"){
            std::vector tiles{t01, t11, t21};
            ToTMapper mapper(2);
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::Range corr_range({0, 0}, {5, 3});
            TA::TensorD corr(corr_range, {0, 0, 1,
                                             0, 0, 1,
                                             0, 0, 4,
                                             6, 6, 0,
                                             6, 6, 0});
            REQUIRE(r == corr);
        }

        SECTION("Mode 0 is independent"){
            std::vector tiles{t01};
            ToTMapper mapper(2, {{0, 0}});
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::Range corr_range({0}, {1});
            TA::TensorD corr(corr_range, {1});
            REQUIRE(r == corr);
        }

        SECTION("Mode 1 is independent"){
            std::vector tiles{t21};
            ToTMapper mapper(2, {{0, 1}});
            auto r = make_inner_tile_(index_type{1}, tiles, mapper);
            TA::Range corr_range({0}, {2});
            TA::TensorD corr(corr_range, {6, 6});
            REQUIRE(r == corr);
        }
    }

    SECTION("Rank 3 tensor"){
        TA::TensorD t000(TA::Range{{0, 0, 0}, {2, 2, 2}}, {1, 2, 3, 4, 5, 6, 7, 8});
        TA::TensorD t111(TA::Range{{2, 2, 2}, {3, 3, 3}}, {9});

        SECTION("All dependent"){
            std::vector tiles{t000, t111};
            ToTMapper mapper(3);
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::Range corr_range({0, 0, 0}, {3, 3, 3});
            TA::TensorD corr(corr_range, {1, 2, 0, 3, 4, 0, 0, 0, 0,
                                          5, 6, 0, 7, 8, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 9});
            REQUIRE(corr == r);
        }

        SECTION("Mode 0 is independent"){
            std::vector tiles{t000};
            ToTMapper mapper(3, {{0, 0}});
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::Range corr_range({0, 0}, {2, 2});
            TA::TensorD corr(corr_range, {1, 2, 3, 4});
        }

        SECTION("Mode 1 is independent"){
            std::vector tiles{t000};
            ToTMapper mapper(3, {{0, 1}});
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::Range corr_range({0, 0}, {2, 2});
            TA::TensorD corr(corr_range, {1, 2, 5, 6});
        }

        SECTION("Mode 2 is independent"){
            std::vector tiles{t000};
            ToTMapper mapper(3, {{0, 2}});
            auto r = make_inner_tile_(index_type{0}, tiles, mapper);
            TA::Range corr_range({0, 0}, {2, 2});
            TA::TensorD corr(corr_range, {1, 3, 5, 7});
        }

        SECTION("Mode 0 and 1 are independent"){
            std::vector tiles{t000};
            ToTMapper mapper(3, {{0, 0}, {1, 1}});
            auto r = make_inner_tile_(index_type{0, 0}, tiles, mapper);
            TA::Range corr_range({0}, {2});
            TA::TensorD corr(corr_range, {1, 2});
        }

        SECTION("Mode 0 and 2 are independent"){
            std::vector tiles{t000};
            ToTMapper mapper(3, {{0, 0}, {1, 2}});
            auto r = make_inner_tile_(index_type{0, 0}, tiles, mapper);
            TA::Range corr_range({0}, {2});
            TA::TensorD corr(corr_range, {1, 3});
        }

        SECTION("Mode 1 and 2 are independent"){
            std::vector tiles{t000};
            ToTMapper mapper(3, {{0, 1}, {1, 2}});
            auto r = make_inner_tile_(index_type{0, 0}, tiles, mapper);
            TA::Range corr_range({0}, {2});
            TA::TensorD corr(corr_range, {1, 5});
        }
    }
}

TEST_CASE("make_tot_trange_"){
    using mode_map = std::map<std::size_t, std::size_t>;
    using range_map = std::map<std::size_t, TA::TiledRange1>;

    SECTION("Vector"){
        TA::TiledRange trange({{0, 1, 3, 5}});
        mode_map m;
        range_map r{{0, TA::TiledRange1{0, 1, 2, 3}}};
        auto rv = make_tot_trange_(trange, m, r);
        TA::TiledRange corr({r[0]});
        REQUIRE(rv == corr);
    }

    SECTION("Matrix") {
        TA::TiledRange trange({{0, 1, 3, 5}, {0, 2, 4, 6}});

        SECTION("All free independent modes") {
            mode_map m;
            range_map r{{0, TA::TiledRange1{0, 1, 2, 3}},
                        {1, TA::TiledRange1{0, 5, 9}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({r[0], r[1]});
            REQUIRE(rv == corr);
        }

        SECTION("Mode 0 is independent"){
            mode_map m{{0, 0}};
            range_map r{{1, TA::TiledRange1{0, 5, 9}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({trange.dim(0), r[1]});
            REQUIRE(rv == corr);
        }

        SECTION("Mode 1 is independent"){
            mode_map m{{1, 0}};
            range_map r{{0, TA::TiledRange1{0, 1, 2, 3}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({r[0], trange.dim(0)});
            REQUIRE(rv == corr);
        }
    }

    SECTION("Rank 3 Tensor") {
        TA::TiledRange trange({{0, 1, 3, 5}, {0, 2, 4, 6}, {0, 3, 6, 9}});

        SECTION("All free independent modes") {
            mode_map m;
            range_map r{{0, TA::TiledRange1{0, 1, 2, 3}},
                        {1, TA::TiledRange1{0, 5, 9}},
                        {2, TA::TiledRange1{0, 8, 16}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({r[0], r[1], r[2]});
            REQUIRE(rv == corr);
        }

        SECTION("Mode 0 is independent"){
            mode_map m{{0, 0}};
            range_map r{{1, TA::TiledRange1{0, 5, 9}},
                        {2, TA::TiledRange1{0, 8, 16}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({trange.dim(0), r[1], r[2]});
            REQUIRE(rv == corr);
        }

        SECTION("Mode 1 is independent"){
            mode_map m{{1, 0}};
            range_map r{{0, TA::TiledRange1{0, 1, 2, 3}},
                        {2, TA::TiledRange1{0, 8, 16}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({r[0], trange.dim(0), r[2]});
            REQUIRE(rv == corr);
        }

        SECTION("Mode 2 is independent"){
            mode_map m{{2, 0}};
            range_map r{{0, TA::TiledRange1{0, 1, 2, 3}},
                        {1, TA::TiledRange1{0, 8, 16}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({r[0], r[1], trange.dim(0)});
            REQUIRE(rv == corr);
        }

        SECTION("Modes 0 and 1 are independent"){
            mode_map m{{0, 0}, {1, 1}};
            range_map r{{2, TA::TiledRange1{0, 8, 16}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({trange.dim(0), trange.dim(1), r[2]});
            REQUIRE(rv == corr);
        }

        SECTION("Modes 0 and 2 are independent"){
            mode_map m{{0, 0}, {2, 1}};
            range_map r{{1, TA::TiledRange1{0, 1, 2, 3}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({trange.dim(0), r[1], trange.dim(1)});
            REQUIRE(rv == corr);
        }

        SECTION("Modes 1 and 2 are independent"){
            mode_map m{{1, 1}, {2, 0}};
            range_map r{{0, TA::TiledRange1{0, 1, 2, 3}}};
            auto rv = make_tot_trange_(trange, m, r);
            TA::TiledRange corr({r[0], trange.dim(1), trange.dim(0)});
            REQUIRE(rv == corr);
        }
    }
}
