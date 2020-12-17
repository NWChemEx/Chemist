#pragma once
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/get_block_idx.hpp"
#include "libchemist/types.hpp"
#include <algorithm>
#include <iterator>

namespace libchemist::sparse_map {
namespace detail_ {

/** @brief Removes injected mode offsets from an index.
 *
 *  When dealing with tensors with one or more modes spanned by an independent
 *  index we need to take slices with the independent mode offsets pinned to
 *  the index in the outer tensor. This function removes those pinned modes.
 *
 *  @param[in] idx The index which contains injected mode offsets.
 *  @param[in] injections The map from mode number to injected index, *i.e.*,
 *                        `injections[i]` is the offset to inject as mode `i`.
 *
 * @return A copy of @p idx with injected modes removed.
 *
 * @throw std::runtime_eror if the modes in @p injections are not consistent
 *                          with @p idx. Strong throw guarantee.
 * @throw std::bad_alloc if there is insufficient memory to create the
 *                       uninjected index. Strong throw guarantee.
 */
inline auto uninject_index_(
  const ElementIndex& idx,
  const std::map<std::size_t, std::size_t>& injections) {
    const auto r = idx.size();
    if(injections.empty()) return idx;
    for(const auto& [k, v] : injections)
        if(k >= r)
            throw std::runtime_error("Mode: " + std::to_string(k) +
                                     " is not in range [0, " +
                                     std::to_string(r) + ")");
    std::vector<std::size_t> uninjected_idx(r - injections.size());
    for(std::size_t i = 0, counter = 0; i < r; ++i) {
        if(!injections.count(i)) {
            uninjected_idx[counter] = idx[i];
            ++counter;
        }
    }
    return ElementIndex(std::move(uninjected_idx));
}

/** @brief Fills in the provided ToT tile.
 *
 * @tparam TileType The type of a ToT tile. Assumed to satisfy TA's tile
 *                  concept.
 * @tparam T The type of the tensor we are taking elements from. Assumed to be
 *           some variation of TA::SpArray
 *
 * @param[in] tile The initialized tile we are filling in.
 * @param[in] sm The SparseMap guiding how to fill @p tile from @p tensor.
 * @param[in] tensor Where the elements for @p tile will be taken from.
 * @param[in] ind2mode Map from modes of an independent index to mode of
 *                     @p tensor such that `ind2mode[i]` is the mode of
 *                     @p tensor that the `i`-th independent mode maps to.
 * @return The ToT tile after filling it in.
 */
template<typename TileType, typename T>
auto make_tot_tile_(TileType tile,
                    const SparseMap<ElementIndex, ElementIndex>& sm,
                    const T& tensor,
                    const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    using inner_tile_t = typename std::decay_t<TileType>::value_type;

    const auto& trange  = tensor.trange();            // Trange of "dense"
    const auto ind_rank = sm.ind_rank();              // Rank of inner tensor
    const auto inj_rank = ind_rank + ind2mode.size(); // Rank of injected index
    const bool do_inj   = !ind2mode.empty();          // Are we injecting?

    // Move allocations out of loops
    std::map<std::size_t, std::size_t> injections; // Map for injections

    for(const auto& oeidx_v :
        tile.range()) { // Loop over outer-elemental indices
        const ElementIndex oeidx(oeidx_v.begin(), oeidx_v.end());

        // Handle scenario where independent index has no domain
        if(!sm.count(oeidx)) {
            tile[oeidx] = inner_tile_t{};
            continue;
        }

        const auto& d = sm.at(oeidx); // The elements in the inner tile
        inner_tile_t buffer(TA::Range(d.result_extents()),0.0);

        // Determine tiles to retrieve using injected domain
        // TODO: This is just a copy of d if do_inj == false
        for(const auto& [k, v] : ind2mode) injections[v] = oeidx[k];
        auto injected_d = d.inject(injections);
        Domain<TileIndex> tdomain(trange, injected_d);

        for(const auto& itidx : tdomain) { // Loop over inner-tile indices

            if (!tensor.is_zero(itidx)) {
                inner_tile_t t = tensor.find(itidx);

                // It's not clear to me whether the injection alters the order. If
                // the indices in injected_d are ordered such that the i-th index
                // of injected_d is the i-th index of d (with the former containing
                // the injected modes) then we can loop over injected_d, d zipped
                // together and avoid the uninjection
                for (const auto &ieidx : injected_d) { // Loop over inner-element
                    if (t.range().includes(ieidx)) {   // Is element in tile?
                        // Remove injected modes
                        auto lhs_idx = uninject_index_(ieidx, injections);
                        // map it to output
                        buffer[d.result_index(lhs_idx)] = t[ieidx];
                    }
                }
            }
        }
        tile[oeidx] = buffer;
    }
    return tile;
}

} // namespace detail_

/** @brief Sparsifies a tensor according to the provided SparseMap.
 *
 *  The most general SparseMap is an element-to-element map. Given such a map,
 *  and the tilings of the tensors, we can make any other sparse map. This
 *  overload of from_sparse_map is at the moment the workhorse for all of the
 *  overloads.
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
auto from_sparse_map(const SparseMap<ElementIndex, ElementIndex>& esm,
                     const T& tensor, const TA::TiledRange outer_trange,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    using scalar_type = typename T::scalar_type;
    using tot_type    = type::tensor_of_tensors<scalar_type>;

    if(esm.dep_rank() + ind2mode.size() != tensor.trange().rank())
        throw std::runtime_error("Ranks don't work out.");

    SparseMap<TileIndex, ElementIndex> tesm(outer_trange, esm);
    auto l = [=](auto& tile, const auto& range) {
        using tile_type = std::decay_t<decltype(tile)>;
        auto otidx      = ta_helpers::get_block_idx(outer_trange, range);
        if(!tesm.count(TileIndex(otidx.begin(), otidx.end()))) return 0.0;
        tile = detail_::make_tot_tile_(tile_type(range), esm, tensor, ind2mode);
        return tile.norm();
    };

    return TA::make_array<tot_type>(tensor.world(), outer_trange, l);
}

/** @brief Sparsifies a tensor according to the provided SparseMap.
 *
 *  This overload creates an element-to-element sparse map from an
 *  element-to-tile sparse map and then calls the element-to-element version.
 *
 * @tparam T The type of the tensor being sparsified, assumed to be a TiledArray
 *           tensor.
 *
 * @param[in] etsm The element-to-tile sparse map describing how to sparsify
 *                the tensor.
 * @param[in] tensor The tensor we are sparsifying.
 * @param[in] outer_trange The TiledRange for the outer tensor of the
 *            tensor-of-tensor this function is creating.
 * @param[in] ind2mode A map from independent index mode to the mode in
 *                     @p tensor it maps to.  *i.e.*, `ind2mode[i]` is the mode
 *                     of @p tensor that the `i`-th mode of an independent index
 *                     maps to.
 * @return The tensor-of-tensors resulting from applying @p etsm to @p tensor.
 */
template<typename T>
auto from_sparse_map(const SparseMap<ElementIndex, TileIndex>& etsm,
                     const T& tensor, const TA::TiledRange& trange,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    SparseMap<ElementIndex, ElementIndex> esm(etsm);
    return from_sparse_map(esm, tensor, trange, ind2mode);
}

/** @brief Sparsifies a tensor according to the provided SparseMap.
 *
 *  This overload creates an element-to-element sparse map from an
 *  tile-to-element sparse map and then calls the element-to-element version.
 *
 * @tparam T The type of the tensor being sparsified, assumed to be a TiledArray
 *           tensor.
 *
 * @param[in] tesm The tile-to-element sparse map describing how to sparsify
 *                 the tensor.
 * @param[in] tensor The tensor we are sparsifying.
 * @param[in] ind2mode A map from independent index mode to the mode in
 *                     @p tensor it maps to.  *i.e.*, `ind2mode[i]` is the mode
 *                     of @p tensor that the `i`-th mode of an independent index
 *                     maps to.
 * @return The tensor-of-tensors resulting from applying @p tesm to @p tensor.
 */
template<typename T>
auto from_sparse_map(const SparseMap<TileIndex, ElementIndex>& tesm,
                     const T& tensor,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    SparseMap<ElementIndex, ElementIndex> esm(tesm);
    return from_sparse_map(esm, tensor, tesm.trange(), ind2mode);
}

/** @brief Sparsifies a tensor according to the provided SparseMap.
 *
 *  This overload creates an element-to-element sparse map from an
 *  tile-to-tile sparse map and then calls the element-to-element version.
 *
 * @tparam T The type of the tensor being sparsified, assumed to be a TiledArray
 *           tensor.
 *
 * @param[in] tsm The tile-to-tile sparse map describing how to sparsify
 *                the tensor.
 * @param[in] tensor The tensor we are sparsifying.
 * @param[in] ind2mode A map from independent index mode to the mode in
 *                     @p tensor it maps to.  *i.e.*, `ind2mode[i]` is the mode
 *                     of @p tensor that the `i`-th mode of an independent index
 *                     maps to.
 * @return The tensor-of-tensors resulting from applying @p tsm to @p tensor.
 */
template<typename T>
auto from_sparse_map(const SparseMap<TileIndex, TileIndex>& tsm,
                     const T& tensor,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    SparseMap<ElementIndex, ElementIndex> esm(tsm);
    return from_sparse_map(esm, tensor, tsm.trange(), ind2mode);
}

} // namespace libchemist::sparse_map
