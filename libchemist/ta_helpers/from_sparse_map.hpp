#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include <tiledarray.h>

namespace libchemist {
namespace detail_ {


} // namespace detail_

///** @brief Main API for forming a tensor-of-tensors from a SparseMap instance.
// *
// *
// *  @note This function assumes that dependent modes of the sparse map appear in
// *        the same order in both the sparse map and the tensor. If this is not
// *        the case it is necessary to permute the tensor in order to make this
// *        the case.
// *
// * @tparam TensorType
// * @param sm
// * @param[in] t The tensor we are applying @p sm to.
// * @param[in] ind_modes
// * @param[in] dep_modes
// * @return
// */
//template<typename TensorType>
//auto from_sparse_map(const SparseMap& sm,
//                     TensorType&& t,
//                     std::map<std::size_t, TA::TiledRange1> free_ind_ranges,
//                     const std::map<std::size_t, std::size_t>& ind_sm_modes2t) {
//    //TODO: Check inputs specifically the following should be true
//    // ind_modes.size() + free_ind_ranges.size() == sm.dep_rank()
//    // ind_modes.size() + dep_modes.size() == t.rank()
//    // dep_modes.size() == sm.dep_rank()
//
//    using clean_tensor  = std::decay_t<TensorType>;
//    using scalar_type   = typename clean_tensor::numeric_type;
//    using tot_tile_type = TA::Tensor<TA::Tensor<scalar_type>>;
//    using policy_type   = typename clean_tensor::policy_type;
//    using rv_type       = TA::DistArray<tot_tile_type, policy_type>;
//
//    //--------------- Step 1: Assemble TiledRange of output --------------------
//
//    auto rv_trange =
//      make_tot_trange(t.trange(), ind_sm_modes2t, free_ind_ranges);
//
//    //-------- Step 2: Assemble a lambda which will make the tile
//    ToTMapper mapper(t.range().rank(), ind_sm_mdoes2t);
//    auto l = [=](tot_tile_type& tile2init, const TA::Range& r){
//        // r is the range of the tile we need to make, but what's its index?
//        auto ind_tile_idx = get_block_idx(rv_trange, r);
//        if(!sm.count(ind_tile_idx)) return 0.0; // Not in SM means it's zero
//
//        tot_tile_type buffer(r); // buffer will hold the tile as we make it
//        auto tiles = get_tiles(ind_tile_idx, sm.at(ind_tile_idx), t, mapper);
//
//        for(const auto& ind_elem_idx : r){
//            buffer(ind_elem_idx) = make_inner_tile_(ind_elem_idx, tiles, mapper);
//        }
//    };
//    return TA::make_tensor<rv_type>(t.world(), rv_trange, l)
//}

} // namespace libchemist
