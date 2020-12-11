#pragma once
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include "libchemist/types.hpp"

namespace libchemist::sparse_map {
namespace detail_ {
//TODO: doc
/** @brief
 *
 * @tparam TileType
 * @tparam T
 * @param tile
 * @param sm
 * @param tensor
 * @param ind2mode
 * @return
 */
template<typename TileType, typename T>
auto make_reduced_tile_(TileType tile,
                        const SparseMap<ElementIndex, ElementIndex>& esm,
                        const TA::TiledRange& trange_rv,
                        const T& t_of_t) {

    const auto& range_rv = tile.range();

    //TODO: comment these
    const auto inv_esm = esm.inverse();
    SparseMap<TileIndex, ElementIndex> tesm(t_of_t.trange(),esm);
    const auto inv_tesm = tesm.inverse();
    SparseMap<TileIndex, TileIndex> inv_tsm(trange_rv,inv_tesm);
    SparseMap<TileIndex, ElementIndex> inv_etsm(trange_rv,inv_esm);
    const auto etsm = inv_etsm.inverse();

    const auto tidx_v = ta_helpers::get_block_idx(trange_rv, range_rv);
    const TileIndex tidx_lhs(tidx_v.begin(), tidx_v.end());

    for (const auto& otidx : inv_tsm.at(tidx_lhs)) {
        const auto& outer_tile = t_of_t.find(otidx);
        for (const auto& oeidx_v : outer_tile.range()) {
            const ElementIndex oeidx(oeidx_v.begin(), oeidx_v.end());
            if (etsm.at(oeidx).count(tidx_lhs)) {
                for (const auto& lhs_idx_v : range_rv) {
                    const ElementIndex eidx_lhs(lhs_idx_v.begin(), lhs_idx_v.end());
                    if (esm.at(oeidx).count(eidx_lhs)) {
                        tile[eidx_lhs] += outer_tile[oeidx][esm.at(oeidx).result_index(eidx_lhs)];
                    }
                }
            }
        }
    }

    return tile;
}
} // namespace detail_

//TODO: doc
/** @brief
 *
 * @tparam T The type of the tensor being sparsified, assumed to be a TiledArray
 *           tensor.
 *
 * @param[in] esm The element-to-element sparse map describing how to sparsify
 *                the tensor.
 * @param[in] tensor The tensor we are sparsifying.
 * @param[in] outer_trange The TiledRange for the outer tensor of the
 *            tensor-of-tensor this function is creating.
 * @param[in] ind2mode A map from independent index mode to the mode in
 *                     @p tensor it maps to.  *i.e.*, `ind2mode[i]` is the mode
 *                     of @p tensor that the `i`-th mode of an independent index
 *                     maps to.
 * @return The tensor-of-tensors resulting from applying @p esm to @p tensor.
 */
template<typename T>
auto reduce_tot_sum(const SparseMap<ElementIndex, ElementIndex>& esm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {

    using scalar_type = typename T::scalar_type;
    using tensor_type    = type::tensor<scalar_type>;

    auto inv_esm = esm.inverse();
    SparseMap<TileIndex, ElementIndex> inv_etsm(trange_rv,inv_esm);

    auto l =[=](auto& tile, const auto& range) {
        using tile_type = std::decay_t<decltype(tile)>;
        const auto tidx = ta_helpers::get_block_idx(trange_rv, range);
        if (!inv_etsm.count(TileIndex(tidx.begin(), tidx.end()))) return 0.0;
        tile = detail_::make_reduced_tile_(tile_type(range,0.0), esm, trange_rv, t_of_t);
        return tile.norm();
    };

    return TA::make_array<tensor_type>(t_of_t.world(), trange_rv, l);
}

template<typename T>
auto reduce_tot_sum(const SparseMap<ElementIndex, TileIndex>& etsm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {
    SparseMap<ElementIndex, ElementIndex> esm(etsm);
    return from_sparse_map(esm, t_of_t, trange_rv);
}

template<typename T>
auto reduce_tot_sum(const SparseMap<TileIndex, ElementIndex>& tesm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {
    SparseMap<ElementIndex, ElementIndex> esm(tesm);
    return from_sparse_map(esm, t_of_t, trange_rv);
}

template<typename T>
auto reduce_tot_sum(const SparseMap<TileIndex, TileIndex>& tsm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {
    SparseMap<ElementIndex, ElementIndex> esm(tsm);
    return from_sparse_map(esm, t_of_t, trange_rv);
}

} // namespace libchemist::sparse_map