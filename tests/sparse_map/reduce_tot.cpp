#include "chemist/sparse_map/reduce_tot.hpp"
#include "chemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::sparse_map;
using namespace chemist::sparse_map::detail_;

using tot_type =
  TA::DistArray<TA::Tensor<TA::Tensor<double>>, TA::SparsePolicy>;
using tot_tile    = typename tot_type::value_type;
using inner_tile  = typename tot_tile::value_type;
using tensor_type = TA::DistArray<TA::Tensor<double>, TA::SparsePolicy>;
using tile_type   = typename tensor_type::value_type;
using ei          = ElementIndex;
using ti          = TileIndex;

TEST_CASE("make_reduced_tile_") {
    auto& world = TA::get_default_world();

    SECTION("Vector-of-vectors to vector") {
        // Make the sparse map
        ei e0{0}, e1{1}, e2{2}, e3{3};
        SparseMap<ei, ei> sm({{e0, {e1, e3}}, {e1, {e1, e2}}, {e2, {e0}}});

        // Make ToT
        TA::TiledRange tot_trange{{0, 3}};
        TA::detail::vector_il<inner_tile> corr_il{
          inner_tile{TA::Range{2}, {1, 3}}, inner_tile{TA::Range{2}, {1, 2}},
          inner_tile{TA::Range{1}, {5}}};
        tot_type t_of_t(world, tot_trange, corr_il);

        TA::TiledRange trange{{0, 2, 4}};

        SECTION("Tile 0") {
            tile_type tile(TA::Range{1}, 0);
            tile_type corr(TA::Range{1}, {5, 2});
            tile = make_reduced_tile_(tile, sm, trange, t_of_t);
            REQUIRE(tile == corr);
        }

        SECTION("Tile 1") {
            TA::Range r{{1, 2}};
            tile_type tile(r, 0);
            tile_type corr(r, {2, 3});
            tile = make_reduced_tile_(tile, sm, trange, t_of_t);
            REQUIRE(tile == corr);
        }

        SECTION("Incompatible ranks") {
            ei e0{0}, e1{1}, e01{0, 1}, e12{1, 2}, e02{0, 2};
            SparseMap<ei, ei> bad_sm({{e0, {e01, e12}}, {e1, {e02, e12}}});
            tile_type tile(TA::Range{1});
            REQUIRE_THROWS_AS(make_reduced_tile_(tile, bad_sm, trange, t_of_t),
                              std::runtime_error);
        }
    }
}

// TODO: This unit test randomly segfaults
// TEST_CASE("reduce_tot_sum") {
//     auto& world = TA::get_default_world();

//     SECTION("Vector-of-vectors to vector") {
//         // Make the sparse map
//         ei e0{0}, e1{1}, e2{2}, e3{3};
//         SparseMap<ei, ei> sm({{e0, {e1, e3}}, {e1, {e1, e2}}, {e2, {e0}}});

//         // Make ToT
//         TA::TiledRange tot_trange{{0, 3}};
//         TA::detail::vector_il<inner_tile> corr_il{
//           inner_tile{TA::Range{2}, {1, 3}}, inner_tile{TA::Range{2}, {1, 2}},
//           inner_tile{TA::Range{1}, {5}}};
//         tot_type t_of_t(world, tot_trange, corr_il);

//         // Make correct reduced tensor
//         TA::TiledRange trange{{0, 2, 4}};
//         TA::detail::vector_il<double> il{5, 2, 2, 3};
//         tensor_type corr(world, trange, il);

//         auto rv = reduce_tot_sum(sm, t_of_t, trange);
//         REQUIRE(ta_helpers::allclose(rv, corr));

//         SECTION("Incompatible ranks") {
//             ei e0{0}, e1{1}, e01{0, 1}, e12{1, 2}, e02{0, 2};

//             SparseMap<ei, ei> bad_ind_sm(
//               {{e01, {e01, e12}}, {e12, {e02, e12}}});
//             REQUIRE_THROWS_AS(reduce_tot_sum(bad_ind_sm, t_of_t, trange),
//                               std::runtime_error);

//             SparseMap<ei, ei> bad_dep_sm({{e0, {e01, e12}}, {e1, {e02,
//             e12}}}); REQUIRE_THROWS_AS(reduce_tot_sum(bad_dep_sm, t_of_t,
//             trange),
//                               std::runtime_error);

//             TA::TiledRange bad_trange{{0, 2, 4}, {0, 2, 4}};
//             REQUIRE_THROWS_AS(reduce_tot_sum(sm, t_of_t, bad_trange),
//                               std::runtime_error);
//         }
//     }
// }
