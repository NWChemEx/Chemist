#pragma once
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include "libchemist/types.hpp"

namespace libchemist::sparse_map {
namespace detail_ {

/** @brief Fills in the provided tile with the appropriate sum of elements from the tensor-of-tensor
 *
 *  The sparse map is used to obtain the correct "corresponding" elements from the inner indices
 *  of the tensor-of-tensor.
 *
 * @tparam TileType The type of the tile to be filled.
 * @tparam T The type of the tensor from which elements are being pulled and summed, assumed to be
 *           a tensor-of-tensor.
 * @param[in] tile The initialized tile to be filled in.
 * @param[in] esm  The sparse map which was used to create the tensor-of-tensor.
 * @param[in] trange_rv The TiledRange which @p tile will match.
 * @param[in] t_of_t The tensor-of-tensor providing the elements to sum.
 * @return  The filled in tile.
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
        const TA::Tensor<TileType> outer_tile = t_of_t.find(otidx);
        for (const auto& oeidx_v : outer_tile.range()) {
            const ElementIndex oeidx(oeidx_v.begin(), oeidx_v.end());
            if (outer_tile[oeidx].range().rank() != esm.dep_rank()) {
                throw std::runtime_error("SparseMap and tensor-of-tensor dependent ranks do not match");
            }
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

/** @brief Reduces a tensor-of-tensors to a regular tensor by summing over "corresponding" elements
 *
 *  Reduces the outer indices of a tensor-of-tensor by summing the elements.
 *  In einsum notation:
 *  A(j,k;) = B(i;j,k)
 *  The inner indices of B are now tiled indices in A.
 *
 * @tparam T The type of the tensor being reduced, assumed to be a TiledArray
 *           tensor-of-tensors.
 *
 * @param[in] esm The element-to-element sparse map which was used to sparsify
 *                the tensor.
 * @param[in] t_of_t The tensor-of-tensors we are reducing.
 * @param[in] trange_rv The TiledRange for the output tensor (tiles the inner indices of @p t_of_t).
 * @return The tensor resulting from applying @p esm to @p tensor.
 */
template<typename T>
auto reduce_tot_sum(const SparseMap<ElementIndex, ElementIndex>& esm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {

    if (trange_rv.rank() != esm.dep_rank()) {
        throw std::runtime_error("Input TiledRange rank and SparseMap dependent rank do not match");
    }

    if (t_of_t.trange().rank() != esm.ind_rank()) {
        throw std::runtime_error("SparseMap and tensor-of-tensor independent ranks do not match");
    }

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

/** @brief Reduces a tensor-of-tensors to a regular tensor by summing over "corresponding" elements
 *
 *  An overload which takes an element-to-tile sparse map.
 *
 * @tparam T The type of the tensor being reduced, assumed to be a TiledArray
 *           tensor-of-tensors.
 *
 * @param[in] etsm The element-to-tile sparse map which was used to sparsify
 *                the tensor.
 * @param[in] t_of_t The tensor-of-tensors we are reducing.
 * @param[in] trange_rv The TiledRange for the output tensor (tiles the inner indices of @p t_of_t).
 * @return The tensor resulting from applying @p esm to @p tensor.
 */
template<typename T>
auto reduce_tot_sum(const SparseMap<ElementIndex, TileIndex>& etsm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {
    SparseMap<ElementIndex, ElementIndex> esm(etsm);
    return reduce_tot_sum(esm, t_of_t, trange_rv);
}

/** @brief Reduces a tensor-of-tensors to a regular tensor by summing over "corresponding" elements
 *
 *  An overload which takes a tile-to-element sparse map.
 *
 * @tparam T The type of the tensor being reduced, assumed to be a TiledArray
 *           tensor-of-tensors.
 *
 * @param[in] tesm The tile-to-element sparse map which was used to sparsify
 *                the tensor.
 * @param[in] t_of_t The tensor-of-tensors we are reducing.
 * @param[in] trange_rv The TiledRange for the output tensor (tiles the inner indices of @p t_of_t).
 * @return The tensor resulting from applying @p esm to @p tensor.
 */
template<typename T>
auto reduce_tot_sum(const SparseMap<TileIndex, ElementIndex>& tesm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {
    SparseMap<ElementIndex, ElementIndex> esm(tesm);
    return reduce_tot_sum(esm, t_of_t, trange_rv);
}

/** @brief Reduces a tensor-of-tensors to a regular tensor by summing over "corresponding" elements
 *
 *  An overload which takes a tile-to-tile sparse map.
 *
 * @tparam T The type of the tensor being reduced, assumed to be a TiledArray
 *           tensor-of-tensors.
 *
 * @param[in] tsm The tile-to-tile sparse map which was used to sparsify
 *                the tensor.
 * @param[in] t_of_t The tensor-of-tensors we are reducing.
 * @param[in] trange_rv The TiledRange for the output tensor (tiles the inner indices of @p t_of_t).
 * @return The tensor resulting from applying @p esm to @p tensor.
 */
template<typename T>
auto reduce_tot_sum(const SparseMap<TileIndex, TileIndex>& tsm,
                    const T& t_of_t,
                    const TA::TiledRange& trange_rv) {
    SparseMap<ElementIndex, ElementIndex> esm(tsm);
    return reduce_tot_sum(esm, t_of_t, trange_rv);
}

} // namespace libchemist::sparse_map