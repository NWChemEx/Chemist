#pragma once
#include "libchemist/sparse_map/detail_/from_sparse_map_helpers.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include <tiledarray_fwd.h>

namespace libchemist::sparse_map {
/** @brief Main API for forming a tensor-of-tensors from a SparseMap instance.
 *
 *
 *  @note This function assumes that dependent modes of the sparse map appear in
 *        the same order in both the sparse map and the tensor. If this is not
 *        the case it is necessary to permute the tensor in order to make this
 *        the case.
 *
 * @tparam TensorType
 * @param sm
 * @param[in] t The tensor we are applying @p sm to.
 * @param[in] ind_modes
 * @param[in] dep_modes
 * @return
 */
template<typename TensorType>
auto from_sparse_map(
  const SparseMap& sm, TensorType&& t,
  std::map<std::size_t, TA::TiledRange1> free_ind_ranges,
  const std::map<std::size_t, std::size_t>& ind_sm_modes2t = {}) {
    // TODO: Check inputs specifically the following should be true
    // ind_modes.size() + free_ind_ranges.size() == sm.dep_rank()
    // ind_modes.size() + dep_modes.size() == t.rank()
    // dep_modes.size() == sm.dep_rank()

    using clean_tensor  = std::decay_t<TensorType>;
    using scalar_type   = typename clean_tensor::numeric_type;
    using tot_tile_type = TA::Tensor<TA::Tensor<scalar_type>>;
    using policy_type   = typename clean_tensor::policy_type;
    using rv_type       = TA::DistArray<tot_tile_type, policy_type>;

    //--------------- Step 1: Assemble TiledRange of output --------------------

    auto rv_trange =
      detail_::make_tot_trange_(t.trange(), ind_sm_modes2t, free_ind_ranges);

    //-------- Step 2: Assemble a lambda which will make the tile
    detail_::ToTMapper mapper(t.range().rank(), ind_sm_modes2t);
    auto l = [=](tot_tile_type& tile2init, const TA::Range& r) {
        // r is the range of the tile we need to make, but what's its index?
        auto ind_tile_idx = get_block_idx(rv_trange, r);

        tot_tile_type buffer(r); // buffer will hold the tile as we make it
        bool is_zero = true;
        for(const auto& ind_elem_idx : r) {
            const auto has_idx =
              sm.count(ind_elem_idx.begin(), ind_elem_idx.end());
            if(is_zero && !has_idx)
                continue;
            else if(!has_idx)
                throw std::runtime_error(
                  "Must provide domains for every independent index of a "
                  "non-zero tile");
            is_zero       = false;
            const auto& d = sm.at(ind_elem_idx);
            auto tiles    = get_tiles_(ind_tile_idx, d, t, mapper);
            buffer(ind_elem_idx) =
              make_inner_tile_(ind_elem_idx, tiles, mapper);
        }
        if(is_zero) return 0.0;
        tile2init.swap(buffer);
        return tile2init.norm();
    };
    auto rv = TA::make_array<rv_type>(t.world(), rv_trange, l);
    t.world().gop.fence();
    return rv;
}

/** @brief Overload of from_sparse_map for case where all independent modes
 *         appear in @p t.
 *
 *  If all of the independent modes of the ToT appear in the tensor we are
 *  mapping we do not need the user to tell us the tiling for the independent
 *  modes (we'll grab them from @p t). This overload simplifies the API of
 *  the main from_sparse_map call to account for this. Aside from that fact this
 *  overload behaves the same as the main from_sparse_map call.
 *
 * @tparam TensorType Assumed to be TA::DistArray or a class with the same API.
 *
 * @param[in] sm The sparse map we are applying to the tensor.
 * @param[in] t The tensor we are applying the sparse map to.
 * @param[in] ind_sm_modes2t A map such that `ind_sm_modes2t[i]` is the mode of
 *            @p t that the i-th independent mode of the ToT maps to.
 * @return The ToT resulting from applying @p sm to @p t.
 */
template<typename TensorType>
auto from_sparse_map(const SparseMap& sm, TensorType&& t,
                     const std::map<std::size_t, std::size_t>& ind_sm_modes2t) {
    return from_sparse_map(sm, std::forward<TensorType>(t),
                           std::map<std::size_t, TA::TiledRange1>{},
                           ind_sm_modes2t);
}
} // namespace libchemist::sparse_map
