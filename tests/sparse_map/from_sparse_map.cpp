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

TEST_CASE("from_sparse_map main"){
    using index_type = ElementIndex;
    using sm_t       = SparseMap<index_type, index_type>;
    auto& world      = TA::get_default_world();

    /* Here we apply a sparse map (independent indices are element indices,
     * dependent indices are tile indices):
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
     * [2 3 6 7]
     * [2 3 4 5]
     * [0 1]
     *
     * Since the independent index does not appear in the tensor, the tiling of
     * the ToT, and lengths of the independent mode, are arbitrary aside from
     * the fact that the independent tiled range must contain at least three
     * elements (if it contains more they are simply zero vectors-of-vectors).
     */
//    SECTION("Vector"){
//        // Make the sparse map
//        index_type e0{0}, e1{1}, e2{2}, e3{3};
//        sm_t sm({{e0, {e1, e3}}, {e1, {e1, e2}}, {e2, {e0}}});
//
//        // Make t
//        TA::TiledRange trange{{0, 2, 4, 6, 8}};
//        TA::detail::vector_il<double> il{0, 1, 2, 3, 4, 5, 6, 7, 8};
//        TA::TSpArrayD t(world, trange, il);
//
//        // Partition the outer vector somewhat arbitrarily into a single tile
//        TA::TiledRange corr_trange{{0, 3}};
//
//        // Make the correct answer
//        TA::detail::vector_il<inner_tile_type> corr_il{
//          inner_tile_type{TA::Range{4}, {2, 3, 6, 7}},
//          inner_tile_type{TA::Range{4}, {2, 3, 4, 5}},
//          inner_tile_type{TA::Range{2}, {0, 1}}
//        };
//        tot_type corr(world, corr_trange, corr_il);
//
//        auto rv = from_sparse_map(sm, t, corr_trange);
//        const bool good = allclose_tot(rv, corr, 1);
//        REQUIRE(good);
//    }

//    SECTION("Matrix"){
//
//        // Make t
//        TA::TiledRange trange{{0, 2, 3}, {0, 2, 3, 6}};
//        TA::detail::matrix_il<double> il{{0, 1, 2, 3, 4, 5},
//                                         {6, 7, 8, 9, 10, 11},
//                                         {12, 13, 14, 15, 15, 17}};
//        TA::TSpArrayD t(world, trange, il);
//
//        /* Here we apply a sparse map (independent indices are element indices,
//         * dependent indices are tile indices):
//         *
//         * 0 -> {{0, 1}, {1, 2}}
//         * 1 -> {{0, 2}, {1, 2}}
//         * 3 -> {{0, 0}, {1, 1}}
//         *
//         * to a matrix (lines denote tile boundaries)
//         *
//         *  0  1 |  2 |  3  4  5
//         *  6  7 |  8 |  9 10 11
//         * ---------------------
//         * 12 13 | 14 | 15 16 17
//         *
//         * the resulting ToT is a vector of matrices, which has at least four
//         * vector elements (element 2 is the zero matrix). Respectively the
//         * three non-zero elements are:
//         *
//         * 2  0  0  0
//         * 8  0  0  0
//         * 0 15 16 17
//         *
//         *  3  4  5
//         *  9 10 11
//         * 15 16 17
//         *
//         * 0 1  0
//         * 6 7  0
//         * 0 0 14
//         */
//        SECTION("All modes dependent") {
//            // Make the sparse map
//            index_type e0{0}, e1{1}, e3{3}, e01{0, 1}, e12{1, 2}, e02{0, 2},
//              e11{1, 1}, e00{0, 0};
//            SparseMap sm(
//              {{e0, {e01, e12}}, {e1, {e02, e12}}, {e3, {e00, e11}}});
//
//            // Partition the outer vector somewhat arbitrarily into a single tile
//            TA::TiledRange corr_trange{{0, 2, 3, 4}};
//            free_range_type idx_map{{0, corr_trange.dim(0)}};
//
//            // Make the correct answer
//            TA::detail::vector_il<inner_tile_type> corr_il{
//              inner_tile_type{TA::Range{{3, 4}},
//                              {2, 0, 0, 0, 8, 0, 0, 0, 0, 15, 16, 17}},
//              inner_tile_type{TA::Range{{3, 3}},
//                              {3, 4, 5, 9, 10, 11, 15, 16, 17}},
//              inner_tile_type{},
//              inner_tile_type{TA::Range{{3, 3}}, {0, 1, 0, 6, 7, 0, 0, 0, 14}}};
//            tot_type corr(world, corr_trange, corr_il);
//
//            auto rv = from_sparse_map(sm, t, std::move(idx_map));
//            const bool good = allclose_tot(rv, corr, 2);
//            REQUIRE(good);
//        }
//
//        /* Here we apply a sparse map (independent indices are element indices,
//         * dependent indices are tile indices):
//         *
//         * 0 -> {{1}, {2}}
//         * 1 -> {{0}, {2}}
//         * 2 -> {{0}, {1}}
//         *
//         * to a matrix (lines denote tile boundaries)
//         *
//         *  0  1 |  2 |  3  4  5
//         *  6  7 |  8 |  9 10 11
//         * ---------------------
//         * 12 13 | 14 | 15 16 17
//         *
//         * the resulting ToT is a vector of vectors with three elements:
//         *
//         * 2 3 4 5
//         *
//         * 6 7 9 10 11
//         *
//         * 12 13 14
//         */
//        SECTION("Mode 0 is independent") {
//            // Make the sparse map
//            index_type e0{0}, e1{1}, e2{2};
//            SparseMap sm({{e0, {e1, e2}}, {e1, {e0, e2}}, {e2, {e0, e1}}});
//
//            std::map<std::size_t, std::size_t> tot2t{{0, 0}};
//
//
//            // Make the correct answer
//            TA::TiledRange corr_trange{{0, 2, 3}};
//            TA::detail::vector_il<inner_tile_type> corr_il{
//              inner_tile_type{TA::Range{{4}},{2, 3, 4, 5}},
//              inner_tile_type{TA::Range{{5}},{6, 7, 9, 10, 11}},
//              inner_tile_type{TA::Range{{3}}, {12, 13, 14}}};
//            tot_type corr(world, corr_trange, corr_il);
//
//            auto rv = from_sparse_map(sm, t, tot2t);
//            const bool good = allclose_tot(rv, corr, 1);
//            REQUIRE(good);
//        }
//
//        /* Here we apply a sparse map (independent indices are element indices,
//         * dependent indices are tile indices):
//         *
//         * 0 -> {{0}, {1}}
//         * 1 -> {{0}}
//         * 2 -> {{1}}
//         *
//         * to a matrix (lines denote tile boundaries)
//         *
//         *  0  1 |  2 |  3  4  5
//         *  6  7 |  8 |  9 10 11
//         * ---------------------
//         * 12 13 | 14 | 15 16 17
//         *
//         * the resulting ToT is a vector of vectors with six elements, the
//         * non-zero ones are:
//         *
//         * 0 6 12
//         *
//         * 1 7
//         *
//         * 14
//         */
//        SECTION("Mode 1 is independent") {
//            // Make the sparse map
//            index_type e0{0}, e1{1}, e2{2};
//            SparseMap sm({{e0, {e0, e1}}, {e1, {e0}}, {e2, {e1}}});
//
//            std::map<std::size_t, std::size_t> tot2t{{0, 1}};
//
//
//            // Make the correct answer
//            TA::TiledRange corr_trange{{0, 2, 3, 6}};
//            TA::detail::vector_il<inner_tile_type> corr_il{
//              inner_tile_type{TA::Range{{3}},{0, 6, 12}},
//              inner_tile_type{TA::Range{{2}},{1, 7}},
//              inner_tile_type{TA::Range{{1}}, {14}},
//              inner_tile_type{},
//              inner_tile_type{},
//              inner_tile_type{}};
//            tot_type corr(world, corr_trange, corr_il);
//
//            auto rv = from_sparse_map(sm, t, tot2t);
//            const bool good = allclose_tot(rv, corr, 1);
//            REQUIRE(good);
//        }
//
//        /* Here we apply a sparse map (independent indices are element indices,
//         * dependent indices are tile indices):
//         *
//         * {0, 0} -> {{1}, {2}}
//         * {0, 1} -> {{1}, {2}}
//         * {1, 0} -> {{0}, {2}}
//         * {1, 1} -> {{0}, {2}}
//         * {3, 2} -> {{0}, {1}}
//         *
//         * to a matrix (lines denote tile boundaries)
//         *
//         *  0  1 |  2 |  3  4  5
//         *  6  7 |  8 |  9 10 11
//         * ---------------------
//         * 12 13 | 14 | 15 16 17
//         *
//         * the resulting ToT is a matrix of vectors with five non-zero elements:
//         *
//         * (0, 0) : 2 3 4 5
//         * (0, 1) : 8 9 10 11
//         * (1, 0) : 0 1 3 4 5
//         * (1, 1) : 6 7 9 10 11
//         * (3, 2) : 12 13 14
//         */
//         SECTION("Mode 0 is independent, but comes from ToT mode 1") {
//            // Make the sparse map
//            index_type e0{0}, e1{1}, e2{2}, e00{0, 0}, e01{0, 1},  e10{1, 0},
//                       e11{1, 1}, e32{3, 2};
//            SparseMap sm({{e00, {e1, e2}},
//                             {e01, {e1, e2}},
//                             {e10, {e0, e2}},
//                             {e11, {e0, e2}},
//                             {e32, {e0, e1}}});
//
//            TA::TiledRange corr_trange{{0, 1, 2, 3, 4}, {0, 2, 3}};
//            free_range_type idx_map{{0, corr_trange.dim(0)}};
//            std::map<std::size_t, std::size_t> tot2t{{1, 0}};
//
//            // Make the correct answer
//            inner_tile_type corr00{TA::Range{{4}},{2, 3, 4, 5}};
//            inner_tile_type corr01{TA::Range{{4}}, {8, 9, 10, 11}};
//            inner_tile_type corr10{TA::Range{5}, {0, 1, 3, 4, 5}};
//            inner_tile_type corr11{TA::Range{{5}},{6, 7, 9, 10, 11}};
//            inner_tile_type corr32{TA::Range{{3}}, {12, 13, 14}};
//            inner_tile_type zero;
//            TA::detail::matrix_il<inner_tile_type> corr_il{
//              {corr00, corr01, zero},
//              {corr10, corr11, zero},
//              {zero, zero, zero},
//              {zero, zero, corr32}};
//            tot_type corr(world, corr_trange, corr_il);
//
//            auto rv = from_sparse_map(sm, t, idx_map, tot2t);
//            const bool good = allclose_tot(rv, corr, 1);
//            REQUIRE(good);
//        }
//    }
}
