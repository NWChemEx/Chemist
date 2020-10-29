#pragma once
#include "libchemist/sparse_map/detail_/tot_mapper.hpp"
#include "libchemist/sparse_map/domain.hpp"
#include <tiledarray.h>

namespace libchemist::sparse_map::detail_ {

/** @brief Extracts the subrange running over dependent modes of the ToT.
 *
 *  Given a tensor @f$T@f$, which is being mapped to a ToT @f$U@f$, this
 *  function will extract from @f$T@f$'s range a subrange which runs over modes
 *  of @f$T@f$ which map to dependent modes in @f$U@f$. The modes are returned
 *  in the same order they appear in @f$T@f$'s range.
 *
 * @param[in] range The full range of @f$T@f$, the tensor being mapped to a ToT.
 * @param[in] mapper A ToTMapper instance containing the index/mode mapping from
 *                   @f$T@f$ to @f$U@f$.
 * @return A TA::Range which runs over the dependent modes of @f$T@f$.
 *
 * @throw std::bad_alloc if there is insufficient memory to allocate a buffer to
 *                       hold the subranges. Strong throw guarantee.
 */
TA::Range extract_dep_range_(const TA::Range& range, const ToTMapper& mapper);

/** @brief Extracts from a list of tiles the subranges which map to dependent
 *         modes of the ToT.
 *
 *  This overload essentially just loops over the single range overload
 *  extract_dep_range_(const TA::Range&, const ToTMapper). See that overload for
 *  more details.
 *
 * @tparam TileType Type of the tensor's tiles. Should satisfy TA's concept of a
 *                  tile.
 * @param[in] tiles The list of tiles from which we are extracting ranges.
 * @param[in] mapper A ToTMapper instance containing the index/mode mapping from
 *                   the tensor to the ToT.
 * @return At the moment this returns an std::vector whose i-th element is an
 *         std::set containing all indices for mode i appearing in one or more
 *         elements of @p tiles.
 */
template<typename TileType>
auto extract_dep_range_(const std::vector<TileType>& tiles,
                        const ToTMapper& mapper) {
    using size_type     = typename ToTMapper::size_type;
    const auto dep_rank = mapper.t_dep_rank();

    // TODO: We know the ranges are contiguous stitch them together instead of
    //       adding all elements to a set
    std::vector<std::set<size_type>> elements(dep_rank);

    for(const auto& tile : tiles) {
        const auto range = extract_dep_range_(tile.range(), mapper);
        for(size_type i = 0; i < dep_rank; ++i) {
            const auto [lo, hi] = range.dim(i);
            for(auto elem = lo; elem < hi; ++elem) elements[i].insert(elem);
        }
    }
    return elements;
}

/** @brief Extracts the tiles needed to form the specified ToT tile.
 *
 *  Assume we are mapping a tensor @f$T@f$ to a ToT @f$U@f$. Given the
 *  coordinate index of a tile in @f$U@f$ this function will retrieve all of the
 *  tiles from @f$T@f$ which will contribute to it.
 *
 * @tparam IndexType Expected to be a random-access container
 * @tparam TensorType Expected to satisfy the same API as TA::DistArray
 *
 * @param[in] ind_tile_idx The coordinate index of a tile in @f$U@f$
 * @param[in] d The domain associated with @p ind_tile_idx
 * @param[in] t The tensor we are extracting tiles out of (@f$T@f$ in the
 *            description above).
 * @param[in] mapper A ToTMapper instance containing the index/mode mapping from
 *                   @f$T@f$ to @f$U@f$.
 * @return An std::vector containing the extracted tiles. The order of the tiles
 *         matches the order of the indices in @p d, that is to say the i-th
 *         tile in the return is associated with the i-th index in @p d.
 */
template<typename IndexType, typename TensorType>
auto get_tiles_(IndexType&& ind_tile_idx, const Domain& d, TensorType&& t,
                const ToTMapper& mapper) {
    using t_tile_type = typename std::decay_t<TensorType>::value_type;
    std::vector<t_tile_type> tiles;

    for(const auto& dep_tile_idx : d) {
        const auto t_tile_idx = mapper.tot2t_index(ind_tile_idx, dep_tile_idx);
        tiles.push_back(t.find(t_tile_idx).get());
    }
    return tiles;
}

/** @brief Makes the TA::Range for the tile formed from the list @p tiles
 *
 *  This function wraps the process of making a TA::Range for the tensor which
 *  will be formed from the list of tiles. Generally speaking this involves
 *  determining how many unique indices appear along each dependent mode. For
 *  example, given two tiles such that for a given dependent mode the first tile
 *  includes elements 1, 2, and 3 and the second tile includes 2, 3, 4 the
 *  unique set of elements is 1, 2, 3, and 4 and the range for that mode is
 *  [0, 4).
 *
 * @tparam TileType The type of tiles in the tensor. Assumed to satisfy TA's
 *                  tile concept.
 * @param[in] tiles An std::vector of the tiles which should be combined to
 *                  form the range.
 * @param[in] mapper A ToTMapper instance containing the index/mode mapping from
 *                   the tensor to the tensor-of-tensors.
 * @return the TA::Range instance spanning the inner tile which will be formed
 *         by the contents of @p tiles.
 */
template<typename TileType>
auto make_inner_tile_range_(const std::vector<TileType>& tiles,
                            const ToTMapper& mapper) {
    using size_type     = typename ToTMapper::size_type;
    const auto elements = extract_dep_range_(tiles, mapper);
    std::vector<size_type> extents;
    extents.reserve(elements.size());
    for(const auto& elems_i : elements) extents.push_back(elems_i.size());
    return TA::Range(extents);
}

/** @brief Wraps the process of creating an inner tile for a tensor-of-tensors.
 *
 *  True the name a tensor-of-tensors is a tensor whose elements are also
 *  tensors. This function wraps the process of creating an instance of the
 *  inner tensor. In general this involves taking slices of the provided tiles
 *  and splicing them together to form a new tile. The new tensor is created by
 *  taking the direct sum of the slices in the order provided.
 *
 * @tparam IndexType Type of container used to hold the independent index. Must
 *                   satisfy random-access container.
 * @tparam TileType  Type of tiles in the tensor. Assumed to satisfy TA's tile
 *                   concept.
 * @tparam InnerTileType Type of tile which should be produced by this function.
 *                       Must satisfy TA's tile concept. By default
 *                       InnerTileType is set to TileType.
 * @param[in] ind_elem_idx The coordinate index of the outer tensor element we
 *                         are making.
 * @param[in] tiles The list of tiles being combined into the inner tile.
 * @param[in] mapper A ToTMapper instance containing the index/mode mapping from
 *                   the tensor to the tensor-of-tensors.
 * @return The tile created by direct summing the provided tiles.
 */
template<typename IndexType, typename TileType,
         typename InnerTileType = TileType>
auto make_inner_tile_(IndexType&& ind_elem_idx,
                      const std::vector<TileType>& tiles,
                      const ToTMapper& mapper) {
    const auto dep_rank = mapper.t_dep_rank();
    const auto range    = make_inner_tile_range_(tiles, mapper);
    InnerTileType rv(range, 0.0);
    const auto elements = extract_dep_range_(tiles, mapper);

    for(const auto& tile : tiles) {
        auto tile_dep_range = extract_dep_range_(tile.range(), mapper);
        for(const auto& dep_idx : tile_dep_range) {
            auto full_idx = mapper.tot2t_index(ind_elem_idx, dep_idx);
            std::vector<std::size_t> result_idx;
            result_idx.reserve(dep_rank);
            for(size_t i = 0; i < dep_rank; ++i) {
                const auto ebegin    = elements[i].begin();
                const auto dep_i_itr = elements[i].find(dep_idx[i]);
                const auto n         = std::distance(ebegin, dep_i_itr);
                result_idx.push_back(n);
            }
            rv(result_idx) = tile(full_idx);
        }
    }
    return rv;
}

/** @brief Makes the TiledRange for the tensor-of-tensors
 *
 * @param[in] t_trange The TiledRange of the tensor being turned into a ToT
 * @param[in] ind_sm_modes2t A map from the independent modes of the ToT to
 *                           the modes of the tensor. For example if
 *                           `ind_sm_modes2t[0] == 1` then the 0-th mode of the
 *                           ToT is the same as the 1-st mode of the tensor.
 * @param[in] free_ind_ranges A map of user-provided ToT tilings. These are
 *                            needed for when an independent mode does not
 *                            appear in the tensor being mapped.
 * @return The TiledRange for the tensor-of-tensors.
 */
TA::TiledRange make_tot_trange_(
  const TA::TiledRange& t_trange,
  const std::map<std::size_t, std::size_t>& ind_sm_modes2t,
  const std::map<std::size_t, TA::TiledRange1>& free_ind_ranges);

} // namespace libchemist::sparse_map::detail_
