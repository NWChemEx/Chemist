#pragma once
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include <tiledarray.h>

namespace libchemist::ta_helpers {

template<typename TileType>
auto tensor_from_tile(TA::World& world, TileType&& tile) {
    std::vector<TA::TiledRange1> trange1s;
    const auto& r = tile.range();
    for(auto i = 0; i < r.rank(); ++i) {
        trange1s.push_back(TA::TiledRange1{0, r.extent(i)});
    }
    TA::TiledRange trange(trange1s.begin(), trange1s.end());
    auto l = [=](auto& out_tile, const auto& range) {
        out_tile = tile;
        return out_tile.norm();
    };
    using tile_type   = std::decay_t<TileType>;
    using tensor_type = TA::DistArray<tile_type, TA::SparsePolicy>;
    return TA::make_array<tensor_type>(world, trange, l);
}

/** @brief Creates two tensors-of-tensors such that the outer tensors have ths
 *         same shape as the provided tensor, but the inner tensors are
 *         respectively the eigenvalues and eigenvectors of the the input
 *         inner tensors.
 *
 * @tparam TensorType The type of the tensor to diagonalize.
 * @tparam SType The type of the tensor holding the overlap matrix.
 * @param t the input tensor-of-tensor to be diagonalized.
 * @param s the overlap of the inner tensors for a generalized eigensolve.
 * @return [evals, evecs] the eigenvalues and eigenvectors, respectively, of the
 * inner tensors.
 */
template<typename TensorType, typename SType = std::decay_t<TensorType>>
auto diagonalize_inner_tensors(TensorType&& t, SType&& s = {}) {
    using tensor_type       = std::decay_t<TensorType>;
    using tile_type         = typename tensor_type::value_type;
    using inner_tensor_type = typename tile_type::value_type;

    const bool is_general = s.is_initialized();
    auto& world           = t.world();
    tensor_type evals(world, t.trange(), t.shape());
    tensor_type evecs(world, t.trange(), t.shape());
    world.gop.fence();
    for(const tile_type& tile : t) {
        auto tile_idx     = get_block_idx(t, tile);
        const auto& range = tile.range();
        tile_type eval_tile(range), evec_tile(range);
        for(const auto& elem_idx : range) {
            const auto& inner_tile = tensor_from_tile(world, tile(elem_idx));
            auto [tile_evals, tile_evecs] = TA::heig(inner_tile);

            TA::Range eval_range(tile_evals.size());
            eval_tile(elem_idx) =
              inner_tensor_type(std::move(eval_range), tile_evals.data());
            evec_tile(elem_idx) = tile_evecs.find({0, 0}).get();
            world.gop.fence();
        }
        evals.set(tile_idx, eval_tile);
        evecs.set(tile_idx, evec_tile);
    }
    t.world().gop.fence();
    return std::make_pair(evals, evecs);
}

/** @brief Creates a tensor-of-tensors such that the outer tensors have ths
 *         same shape as the provided tensor, but the inner tensors are
 *         the cholesky decomposed and inverted input inner tensors.
 *
 * @tparam TensorType The type of the tensor to diagonalize.
 * @tparam SType The type of the tensor holding the overlap matrix.
 * @param t the tensor-of-tensors to Cholesky decompose and invert.
 * @return linv the Cholesky decomposed and inverted inner tensors.
 */
template<typename TensorType>
auto cholesky_linv_inner_tensors(TensorType&& t) {
    using tensor_type       = std::decay_t<TensorType>;
    using tile_type         = typename tensor_type::value_type;
    using inner_tensor_type = typename tile_type::value_type;
    auto& world             = t.world();
    tensor_type linv(world, t.trange(), t.shape());
    world.gop.fence();
    for(const tile_type& tile : t) {
        auto tile_idx     = get_block_idx(t, tile);
        const auto& range = tile.range();
        tile_type linv_tile(range);
        for(const auto& elem_idx : range) {
            const auto& inner_tile = tensor_from_tile(world, tile(elem_idx));
            auto tile_linv         = TA::cholesky_linv(inner_tile);
            TA::Range linv_range(tile_linv.size());
            linv_tile(elem_idx) = tile_linv.find({0, 0}).get();
            world.gop.fence();
        }
        linv.set(tile_idx, linv_tile);
    }
    t.world().gop.fence();
    return linv;
}

} // namespace libchemist::ta_helpers
