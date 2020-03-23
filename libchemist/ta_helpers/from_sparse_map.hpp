#pragma once
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include <tiledarray.h>

namespace libchemist {
namespace detail_ {

template<typename TileType, typename PolicyType>
auto make_inner_ranges_(const sparse_map::SparseMap& sm,
                        const TA::DistArray<TileType, PolicyType>& t,
                        std::size_t mode) {

    // If t's rank is not the same as the rank of the dependent indices then we
    // also need the independent indices to retrieve a block
    const bool need_ind = t.trange().rank() == sm.dep_rank();

    const auto& trange = t.trange();

    for(const auto& [ind, domain] : sm){
        std::vector<std::set<std::pair<size_type, size_type>>> ranges;


    }
}

template<typename TileType, typename PolicyType>
auto from_sparse_map_(const sparse_map::SparseMap& sm,
                      const TA::DistArray<TileType, PolicyType>& t,
                      std::size_t mode) {
    // Type of the tiles in the resulting tensor
    using tile_type = TA::Tensor<TileType>;

    // Type of the resulting tensors
    using tensor_type  = typename TA::DistArray<tile_type, PolicyType>;

    using size_type    = typename sparse_map::SparseMap::size_type;
    using element_type = typename TileType::value_type;
    using index_type   = std::vector<size_type>;

    // If t's rank is not the same as the rank of the dependent indices then we
    // also need the independent indices to retrieve a block
    const bool need_ind = t.trange().rank() == sm.dep_rank();




    return TA::make_array<tensor_type>(
      t.world(), trange, [=](tile_type& tile, const TA::Range& r) {
          auto idx = get_block_idx(trange, r);
          const index_type ind_idx{idx[mode]};

          // Make sure independent index is in sparse map
          if(!sm.count(ind_idx)) return element_type{0.0};

          // Make the dependent index by dropping the independent index
          index_type dep_idx;
          idx.reserve(idx.size() - 1);
          for(size_type i = 0; i < idx.size(); ++i)
              if(i != mode) dep_idx.push_back(idx[i]);

          // Make sure dependent index is in sparse map
          const auto& domain = sm.at(ind_idx);
          if(!domain.count(dep_idx)) return element_type{0.0};

          // It's a good index get the tile from t and put it in the result
          const auto& tidx = need_ind ? idx : dep_idx;
          auto old_tile    = t.find(tidx).get();
          tile             = tile_type(r, old_tile.begin());
          return tile.norm();
      });
}
} // namespace detail_

template<typename TileType, typename PolicyType>
auto from_sparse_map(const sparse_map::SparseMap& sm,
                     const TA::DistArray<TileType, PolicyType>& t,
                     const TA::TiledRange1& tr1, std::size_t mode = 0) {
    if(sm.dep_rank() != t.trange().rank())
        throw std::runtime_error("SparseMap dep_rank() != t.rank()");

    auto trange = add_tiled_dimension(t.trange(), tr1);
    return from_sparse_map_(sm, t, trange, mode);
}

template<typename TileType, typename PolicyType>
auto from_sparse_map(const sparse_map::SparseMap& sm,
                     const TA::DistArray<TileType, PolicyType>& t,
                     std::size_t mode) {
    if(sm.dep_rank() != t.trange().rank() - 1)
        throw std::runtime_error("SparseMap dep_rank() != t.rank()");

    return from_sparse_map_(sm, t, t.trange(), mode);
}

} // namespace libchemist
