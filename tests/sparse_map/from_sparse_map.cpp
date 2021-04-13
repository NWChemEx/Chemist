#include "libchemist/sparse_map/from_sparse_map.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;
using namespace libchemist::sparse_map;
using namespace libchemist::sparse_map::detail_;

using tot_type   = type::tensor_of_tensors<double>;
using tot_tile   = typename tot_type::value_type;
using inner_tile = typename tot_tile::value_type;
using ei         = ElementIndex;
using ti         = TileIndex;

TEST_CASE("uninject_index_") {
    using inject_map_t = std::map<std::size_t, std::size_t>;
    ei i0, i1(1), i2(1, 2);

    SECTION("Empty injection map") {
        inject_map_t m;

        SECTION("Rank 0 index") { REQUIRE(uninject_index_(i0, m) == i0); }

        SECTION("Rank 1 index") { REQUIRE(uninject_index_(i1, m) == i1); }

        SECTION("Rank 2 index") { REQUIRE(uninject_index_(i2, m) == i2); }
    }

    SECTION("Uninject mode 0") {
        inject_map_t m{{0, 0}};

        SECTION("Rank 0 index") {
            REQUIRE_THROWS_AS(uninject_index_(i0, m), std::runtime_error);
        }

        SECTION("Rank 1 index") { REQUIRE(uninject_index_(i1, m) == i0); }

        SECTION("Rank 2 index") { REQUIRE(uninject_index_(i2, m) == ei(2)); }
    }

    SECTION("Uninject mode 1") {
        inject_map_t m{{1, 0}};

        SECTION("Rank 0 index") {
            REQUIRE_THROWS_AS(uninject_index_(i0, m), std::runtime_error);
        }

        SECTION("Rank 1 index") {
            REQUIRE_THROWS_AS(uninject_index_(i1, m), std::runtime_error);
        }

        SECTION("Rank 2 index") { REQUIRE(uninject_index_(i2, m) == i1); }
    }

    SECTION("Uninject modes 0 and 1") {
        inject_map_t m{{0, 1}, {1, 0}};

        SECTION("Rank 0 index") {
            REQUIRE_THROWS_AS(uninject_index_(i0, m), std::runtime_error);
        }

        SECTION("Rank 1 index") {
            REQUIRE_THROWS_AS(uninject_index_(i1, m), std::runtime_error);
        }

        SECTION("Rank 2 index") { REQUIRE(uninject_index_(i2, m) == i0); }
    }
}

TEST_CASE("make_tot_tile_") {
    auto& world = TA::get_default_world();

    SECTION("Vector to vector-of-vectors") {
        // Make the sparse map
        ei e0{0}, e1{1}, e2{2}, e3{3};
        SparseMap<ei, ei> sm({{e0, {e1, e3}}, {e1, {e1, e2}}, {e2, {e0}}});

        // Make t
        TA::TiledRange trange{{0, 2, 4, 6, 8}};
        TA::detail::vector_il<double> il{0, 1, 2, 3, 4, 5, 6, 7, 8};
        TA::TSpArrayD t(world, trange, il);

        SECTION("Tile 0") {
            tot_tile tile(TA::Range{1});
            tot_tile corr(TA::Range{1}, inner_tile{TA::Range{2}, {1, 3}});
            tile = make_tot_tile_(tile, sm, t);
            REQUIRE(tile == corr);
        }

        SECTION("Tile 1") {
            TA::Range r{{1, 2}};
            tot_tile tile(r);
            tot_tile corr(r, inner_tile{TA::Range{2}, {1, 2}});
            tile = make_tot_tile_(tile, sm, t);
            REQUIRE(tile == corr);
        }

        SECTION("Tile 2") {
            TA::Range r{{2, 3}};
            tot_tile tile(r);
            tot_tile corr(r, inner_tile{TA::Range{1}, {0}});
            tile = make_tot_tile_(tile, sm, t);
            REQUIRE(tile == corr);
        }
    }

    SECTION("Matrix") {
        TA::TiledRange trange{{0, 2, 3}, {0, 2, 3, 6}};
        TA::detail::matrix_il<double> il{
          {0, 1, 2, 3, 4, 5}, {6, 7, 8, 9, 10, 11}, {12, 13, 14, 15, 15, 17}};
        TA::TSpArrayD t(world, trange, il);
        SECTION("To vector-of-matrices") {
            // Make the sparse map
            ei e0{0}, e1{1}, e3{3}, e01{0, 1}, e12{1, 2}, e02{0, 2}, e11{1, 1},
              e00{0, 0};
            SparseMap<ei, ei> sm(
              {{e0, {e01, e12}}, {e1, {e02, e12}}, {e3, {e00, e11}}});

            SECTION("Tile 0") {
                TA::Range r(1);
                tot_tile tile(r);
                tot_tile corr(r, inner_tile{TA::Range({2, 2}), {1, 0, 0, 8}});
                tile = make_tot_tile_(tile, sm, t);
                REQUIRE(tile == corr);
            }

            SECTION("Tile 1") {
                TA::Range r({1}, {2});
                tot_tile tile(r);
                tot_tile corr(r, inner_tile{TA::Range({2, 1}), {2, 8}});
                tile = make_tot_tile_(tile, sm, t);
                REQUIRE(tile == corr);
            }

            SECTION("Tile 2") {
                TA::Range r({2}, {3});
                tot_tile tile(r);
                tot_tile corr(r, inner_tile{TA::Range({1}), {0}});
                tile = make_tot_tile_(tile, sm, t);
                REQUIRE(tile == corr);
            }

            SECTION("Tile 3") {
                TA::Range r({3}, {4});
                tot_tile tile(r);
                tot_tile corr(r, inner_tile{TA::Range({2, 2}), {0, 0, 0, 7}});
                tile = make_tot_tile_(tile, sm, t);
                REQUIRE(tile == corr);
            }
        }

        SECTION("To vector-of-vectors") {
            ei e0{0}, e1{1}, e2{2};
            SparseMap<ei, ei> sm(
              {{e0, {e1, e2}}, {e1, {e0, e2}}, {e2, {e0, e1}}});

            SECTION("mode 0 is independent") {
                std::map<std::size_t, std::size_t> tot2t{{0, 0}};

                SECTION("Tile 0") {
                    TA::Range r(1);
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {1, 2}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 1") {
                    TA::Range r({1}, {2});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {6, 8}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 2") {
                    TA::Range r({2}, {3});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {12, 13}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }
            }

            SECTION("mode 1 is independent") {
                std::map<std::size_t, std::size_t> tot2t{{0, 1}};

                SECTION("Tile 0") {
                    TA::Range r(1);
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {6, 12}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 1") {
                    TA::Range r({1}, {2});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {1, 13}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 2") {
                    TA::Range r({2}, {3});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {2, 8}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }
            }
        }

        SECTION("To matrix-of-vectors") {
            // Make the sparse map
            ei e0{0}, e1{1}, e2{2}, e00{0, 0}, e01{0, 1}, e10{1, 0}, e11{1, 1},
              e32{3, 2};
            SparseMap<ei, ei> sm({{e00, {e1, e2}},
                                  {e01, {e1, e2}},
                                  {e10, {e0, e2}},
                                  {e11, {e0, e2}},
                                  {e32, {e0, e1}}});
            SECTION("mode 1 of independent index is mode 0 of tensor") {
                std::map<std::size_t, std::size_t> tot2t{{1, 0}};

                SECTION("Tile 0, 0") {
                    TA::Range r({1, 1});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {1, 2}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 0, 1") {
                    TA::Range r({0, 1}, {1, 2});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {7, 8}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 0, 2") {
                    TA::Range r({0, 2}, {1, 3});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 1, 0") {
                    TA::Range r({1, 0}, {2, 1});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {0, 2}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 1, 1") {
                    TA::Range r({1, 1}, {2, 2});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {6, 8}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 1, 2") {
                    TA::Range r({1, 2}, {2, 3});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 2, 0") {
                    TA::Range r({2, 0}, {3, 1});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 2, 1") {
                    TA::Range r({2, 1}, {3, 2});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 2, 2") {
                    TA::Range r({2, 2}, {3, 3});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0.0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 3, 0") {
                    TA::Range r({3, 0}, {4, 1});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0.0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 3, 1") {
                    TA::Range r({3, 1}, {4, 2});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range({1, 1}), {0.0}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }

                SECTION("Tile 3, 2") {
                    TA::Range r({3, 2}, {4, 3});
                    tot_tile tile(r);
                    tot_tile corr(r, inner_tile{TA::Range(2), {12, 13}});
                    tile = make_tot_tile_(tile, sm, t, tot2t);
                    REQUIRE(tile == corr);
                }
            }
        }
    }
}

TEST_CASE("from_sparse_map(SparseMap<ElementIndex, ElementIndex>") {
    auto& world = TA::get_default_world();

    /* Here we apply a sparse map (independent indices are element indices,
     * dependent indices are element indices):
     * 0 -> {1, 3}
     * 1 -> {1, 2}
     * 2 -> {0}
     *
     * to a vector (lines denote tile boundaries):
     *
     * [0 1 | 2 3 | 4 5 | 6 7]
     *
     * This says that our resulting ToT is a vector with three non-zero
     * elements. Those elements are also vectors and respectively are:
     *
     * [1 3]
     * [1 2]
     * [0]
     *
     * Since the independent index does not appear in the tensor, the tiling of
     * the ToT and lengths of the independent mode are arbitrary.
     */
    SECTION("Vector") {
        // Make the sparse map
        ei e0{0}, e1{1}, e2{2}, e3{3};
        SparseMap<ei, ei> sm({{e0, {e1, e3}}, {e1, {e1, e2}}, {e2, {e0}}});

        // Make t
        TA::TiledRange trange{{0, 2, 4, 6, 8}};
        TA::detail::vector_il<double> il{0, 1, 2, 3, 4, 5, 6, 7, 8};
        TA::TSpArrayD t(world, trange, il);

        // Partition the outer vector somewhat arbitrarily into a single tile
        TA::TiledRange corr_trange{{0, 3}};

        // Make the correct answer
        TA::detail::vector_il<inner_tile> corr_il{
          inner_tile{TA::Range{2}, {1, 3}}, inner_tile{TA::Range{2}, {1, 2}},
          inner_tile{TA::Range{1}, {0}}};
        tot_type corr(world, corr_trange, corr_il);

        auto rv         = from_sparse_map(sm, t, corr_trange);
        const bool good = ta_helpers::allclose_tot(rv, corr, 1);
        REQUIRE(good);
    }

    SECTION("Matrix") {
        // Make t
        TA::TiledRange trange{{0, 2, 3}, {0, 2, 3, 6}};
        TA::detail::matrix_il<double> il{
          {0, 1, 2, 3, 4, 5}, {6, 7, 8, 9, 10, 11}, {12, 13, 14, 15, 15, 17}};
        TA::TSpArrayD t(world, trange, il);

        /* Here we apply a sparse map (independent indices are element indices,
         * dependent indices are tile indices):
         *
         * 0 -> {{0, 1}, {1, 2}}
         * 1 -> {{0, 2}, {1, 2}}
         * 3 -> {{0, 0}, {1, 1}}
         *
         * to a matrix (lines denote tile boundaries)
         *
         *  0  1 |  2 |  3  4  5
         *  6  7 |  8 |  9 10 11
         * ---------------------
         * 12 13 | 14 | 15 16 17
         *
         * the resulting ToT is a vector of matrices, which has at least four
         * vector elements (element 2 is the zero matrix). Respectively the
         * three non-zero elements are:
         *
         * 1 0
         * 0 8
         *
         * 2
         * 8
         *
         * 0 0
         * 0 7
         */
        // SECTION("All modes dependent") {
        //     // Make the sparse map
        //     ei e0{0}, e1{1}, e3{3}, e01{0, 1}, e12{1, 2}, e02{0, 2}, e11{1,
        //     1},
        //       e00{0, 0};
        //     SparseMap<ei, ei> sm(
        //       {{e0, {e01, e12}}, {e1, {e02, e12}}, {e3, {e00, e11}}});

        //     // Partition the outer vector somewhat arbitrarily into a single
        //     tile TA::TiledRange corr_trange{{0, 2, 3, 4}};

        //     // Make the correct answer
        //     TA::detail::vector_il<inner_tile> corr_il{
        //       inner_tile{TA::Range{2, 2}, {1, 0, 0, 8}},
        //       inner_tile{TA::Range{2, 1}, {2, 8}}, inner_tile{},
        //       inner_tile{TA::Range{2, 2}, {0, 0, 0, 7}}};
        //     tot_type corr(world, corr_trange, corr_il);

        //     auto rv = from_sparse_map(sm, t, corr_trange);
        //      const bool good = ta_helpers::allclose_tot(rv, corr, 2);
        //      REQUIRE(good);
        // }

        /* Here we apply a sparse map (independent indices are element indices,
         * dependent indices are tile indices):
         *
         * 0 -> {{1}, {2}}
         * 1 -> {{0}, {2}}
         * 2 -> {{0}, {1}}
         *
         * to a matrix (lines denote tile boundaries)
         *
         *  0  1 |  2 |  3  4  5
         *  6  7 |  8 |  9 10 11
         * ---------------------
         * 12 13 | 14 | 15 16 17
         *
         * the resulting ToT is a vector of vectors with three elements:
         *
         * 1 2
         *
         * 6 8
         *
         * 12 13
         */
        SECTION("Mode 0 is independent") {
            // Make the sparse map
            ei e0{0}, e1{1}, e2{2};
            SparseMap<ei, ei> sm(
              {{e0, {e1, e2}}, {e1, {e0, e2}}, {e2, {e0, e1}}});

            std::map<std::size_t, std::size_t> tot2t{{0, 0}};

            // Make the correct answer
            TA::TiledRange corr_trange{{0, 2, 3}};
            TA::detail::vector_il<inner_tile> corr_il{
              inner_tile{TA::Range{2}, {1, 2}},
              inner_tile{TA::Range{2}, {6, 8}},
              inner_tile{TA::Range{2}, {12, 13}}};
            tot_type corr(world, corr_trange, corr_il);

            auto rv         = from_sparse_map(sm, t, corr_trange, tot2t);
            const bool good = ta_helpers::allclose_tot(rv, corr, 1);
            REQUIRE(good);
        }

        /* Here we apply a sparse map (independent indices are element indices,
         * dependent indices are tile indices):
         *
         * 0 -> {{0}, {1}}
         * 1 -> {{0}}
         * 2 -> {{1}}
         *
         * to a matrix (lines denote tile boundaries)
         *
         *  0  1 |  2 |  3  4  5
         *  6  7 |  8 |  9 10 11
         * ---------------------
         * 12 13 | 14 | 15 16 17
         *
         * the resulting ToT is a vector of vectors with six elements, the
         * non-zero ones are:
         *
         * 0 6
         *
         * 1
         *
         * 8
         */
        SECTION("Mode 1 is independent") {
            // Make the sparse map
            ei e0{0}, e1{1}, e2{2};
            SparseMap<ei, ei> sm({{e0, {e0, e1}}, {e1, {e0}}, {e2, {e1}}});

            std::map<std::size_t, std::size_t> tot2t{{0, 1}};

            // Make the correct answer
            TA::TiledRange corr_trange{{0, 2, 3}};
            TA::detail::vector_il<inner_tile> corr_il{
              inner_tile{TA::Range{2}, {0, 6}}, inner_tile{TA::Range{1}, {1}},
              inner_tile{TA::Range{1}, {8}}};
            tot_type corr(world, corr_trange, corr_il);

            auto rv         = from_sparse_map(sm, t, corr_trange, tot2t);
            const bool good = ta_helpers::allclose_tot(rv, corr, 1);
            REQUIRE(good);
        }

        /* Here we apply a sparse map (independent indices are element indices,
         * dependent indices are tile indices):
         *
         * {0, 0} -> {{1}, {2}}
         * {0, 1} -> {{1}, {2}}
         * {1, 0} -> {{0}, {2}}
         * {1, 1} -> {{0}, {2}}
         * {3, 2} -> {{0}, {1}}
         *
         * to a matrix (lines denote tile boundaries)
         *
         *  0  1 |  2 |  3  4  5
         *  6  7 |  8 |  9 10 11
         * ---------------------
         * 12 13 | 14 | 15 16 17
         *
         * the resulting ToT is a matrix of vectors with five non-zero elements:
         *
         * (0, 0) : 1 2
         * (0, 1) : 7 8
         * (1, 0) : 0 2
         * (1, 1) : 6 8
         * (3, 2) : 12 13
         */
        // SECTION("Mode 0 is independent, but comes from ToT mode 1") {
        //     // Make the sparse map
        //     ei e0{0}, e1{1}, e2{2}, e00{0, 0}, e01{0, 1}, e10{1, 0}, e11{1,
        //     1},
        //       e32{3, 2};
        //     SparseMap<ei, ei> sm({{e00, {e1, e2}},
        //                           {e01, {e1, e2}},
        //                           {e10, {e0, e2}},
        //                           {e11, {e0, e2}},
        //                           {e32, {e0, e1}}});

        //     TA::TiledRange corr_trange{{0, 1, 2, 3, 4}, {0, 2, 3}};
        //     std::map<std::size_t, std::size_t> tot2t{{1, 0}};

        //     // Make the correct answer
        //     inner_tile corr00{TA::Range{2},{1, 2}};
        //     inner_tile corr01{TA::Range{2}, {7, 8}};
        //     inner_tile corr10{TA::Range{2}, {0, 2}};
        //     inner_tile corr11{TA::Range{2},{6, 8}};
        //     inner_tile corr32{TA::Range{2}, {12, 13}};
        //     inner_tile zero;
        //     TA::detail::matrix_il<inner_tile> corr_il{
        //                   {corr00, corr01, zero},
        //                   {corr10, corr11, zero},
        //                   {zero, zero, zero},
        //                   {zero, zero, corr32}};
        //     tot_type corr(world, corr_trange, corr_il);

        //     auto rv = from_sparse_map(sm, t, corr_trange, tot2t);
        //     std::cout << rv << corr << std::endl;
        //     const bool good = ta_helpers::allclose_tot(rv, corr, 1);
        //     REQUIRE(good);
        // }
    }
}
