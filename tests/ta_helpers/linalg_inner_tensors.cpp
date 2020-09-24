#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/linalg_inner_tensors.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist;

TEST_CASE("diagonalize_inner_tensors"){
    using inner_tile_type = TA::Tensor<double>;
    using tile_type       = TA::Tensor<inner_tile_type>;
    using tensor_type     = TA::DistArray<tile_type, TA::SparsePolicy>;
    using il_type         = TA::detail::vector_il<inner_tile_type>;

    auto& world = TA::get_default_world();
    TA::Range r22({2, 2}), r33({3, 3});
    inner_tile_type e0(r33, {1, 2, 3, 2, 4, 5, 3, 5, 6});
    inner_tile_type e0_evals(TA::Range(3), {-0.5157294715892564, 0.1709151888271797, 11.3448142827620728});
    inner_tile_type e0_evecs(r33, {0.7369762290995787, 0.5910090485061027, 0.3279852776056817,
                                   0.3279852776056812, -0.7369762290995785, 0.5910090485061033,
                                   -0.5910090485061031, 0.3279852776056821, 0.7369762290995784});

    inner_tile_type e1(r22, {1, 2, 2, 3});
    inner_tile_type e1_evals(TA::Range(2), {-0.2360679774997897, 4.2360679774997898});
    inner_tile_type e1_evecs(r22, {-0.8506508083520399, 0.5257311121191335,
                                   0.5257311121191335, 0.8506508083520399});

    il_type il{e0, e1};
    il_type eval_il{e0_evals, e1_evals};
    il_type evec_il{e0_evecs, e1_evecs};

    tensor_type t(world, il);
    tensor_type corr_evals(world, eval_il);
    tensor_type corr_evecs(world, evec_il);

    auto [evals, evecs] = diagonalize_inner_tensors(t);
    REQUIRE(libchemist::allclose_tot(evals, corr_evals, 1));
    REQUIRE(libchemist::allclose_tot(evecs, corr_evecs, 2, true));
}
