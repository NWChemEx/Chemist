#pragma once
#include "libchemist/types.hpp"
#include "libchemist/sparse_map/index.hpp"
#include "libchemist/sparse_map/sparse_map_class.hpp"
#include <algorithm>
#include <iterator>

namespace libchemist::sparse_map {
namespace detail_ {

using mode_map_t = std::vector<std::set<std::size_t>>;

/** @brief Creates an object which can be used to map the modes of the tensor
 *         being sparsified to the
 *
 * @param d
 * @return
 */
inline auto get_mode_map_(const Domain<ElementIndex>& d) {
    const auto rank = d.rank();
    mode_map_t counts(rank);
    for(const auto& idx : d)
        for(std::size_t i = 0; i < rank; ++i) counts[i].insert(idx[i]);
    return counts;
}

inline auto compute_tile_size_(const mode_map_t& mode_map) {
    std::vector<std::size_t> sizes(mode_map.size());
    for(std::size_t i = 0; i < sizes.size(); ++i)
        sizes[i] = mode_map[i].size();
    return sizes;
}

inline auto uninject_index_(const ElementIndex& idx,
                            const std::map<std::size_t, std::size_t>& injections) {
    if(injections.size() == 0) return idx;
    std::vector<std::size_t> uninjected_idx(idx.size() - injections.size());
    for(std::size_t i = 0, counter = 0; i < idx.size(); ++i){
        if(!injections.count(i)) {
            uninjected_idx[counter] = idx[i];
            ++counter;
        }
    }
    return ElementIndex(std::move(uninjected_idx));
}

inline auto map_index_(const ElementIndex& idx, const mode_map_t& mode_map) {
    const auto rank = idx.size();
    std::vector<std::size_t> rv(rank, 0);
    for(std::size_t i = 0; i < rank; ++i){
        const auto& offsets = mode_map[i];
        const auto value    = idx[i];
        auto itr            = std::find(offsets.begin(), offsets.end(), value);
        if(itr == offsets.end())
            throw std::out_of_range("Index is not in domain");
        rv[i] = std::distance(offsets.begin(), itr);
    }
    return rv;
}

/**
 *
 * @tparam TileType
 * @tparam T
 * @param tile
 * @param sm
 * @param tensor
 * @param[in] ind2mode Map from modes of an independent index to mode of
 *                     @p tensor.
 * @return
 */
template<typename TileType, typename T>
auto make_tot_tile_(TileType tile,
                    const SparseMap<ElementIndex, ElementIndex>& sm,
                    const T& tensor,
                    const std::map<std::size_t, std::size_t>& ind2mode) {
    using inner_tile_t  = typename std::decay_t<TileType>::value_type;

    const auto& trange  = tensor.trange();            // Trange of "dense"
    const auto ind_rank = sm.ind_rank();              // Rank of inner tensor
    const auto inj_rank = ind_rank + ind2mode.size(); // Rank of injected index
    const bool do_inj   = !ind2mode.empty();          // Are we injecting?

    // Move allocations out of loops
    std::map<std::size_t, std::size_t> injections;     // Map for injections

    for(const auto& oeidx_v : tile.range()){ //Loop over outer-elemental indices
        const ElementIndex oeidx(oeidx_v.begin(), oeidx_v.end());

        //Handle scenario where independent index has no domain
        if(!sm.count(oeidx)) {
            tile[oeidx] = inner_tile_t{};
            continue;
        }

        const auto& d = sm.at(oeidx);   // The elements in the inner tile
        auto idx_map = get_mode_map_(d); // Maps elements from tensor to tile
        inner_tile_t buffer(TA::Range(compute_tile_size_(idx_map)));

        // Determine tiles to retrieve using injected domain
        // TODO: This is just a copy of d if do_inj == false
        for(const auto& [k, v] : ind2mode) injections[v] = oeidx[k];
        auto injected_d = d.inject(injections);
        Domain<TileIndex> tdomain(trange, injected_d);

        for(const auto& itidx : tdomain) { // Loop over inner-tile indices
            inner_tile_t t = tensor.find(itidx);
            for(const auto& ieidx : injected_d) { // Loop over inner-element
                const auto& ieidx_raw = ieidx.m_index;
                if(t.range().includes(ieidx)){ //Is element in tile?
                    // Remove injected modes
                    auto lhs_idx = uninject_index_(ieidx, injections);
                    // map it to output
                    buffer[map_index_(lhs_idx, idx_map)] = t[ieidx_raw];
                }
            }
        }
        tile[oeidx] = buffer;
    }
    return tile;
}

} // namespace detail_

template<typename T>
auto from_sparse_map(const SparseMap<ElementIndex, ElementIndex>& esm,
                     const T& tensor,
                     const TA::TiledRange outer_trange,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {

    using scalar_type = typename T::scalar_type;
    using tot_type    = type::tensor_of_tensors<scalar_type>;

    if(esm.dep_rank() + ind2mode.size() != tensor.trange().rank())
        throw std::runtime_error("Ranks don't work out.");

    auto l =[=](auto& tile, const auto& range) {
        using tile_type = std::decay_t<decltype(tile)>;
        tile = detail_::make_tot_tile_(tile_type(range), esm, tensor, ind2mode);
        return tile.norm();
    };

    return TA::make_array<tot_type>(tensor.world(), outer_trange, l);
}

template<typename T>
auto from_sparse_map(const SparseMap<ElementIndex, TileIndex>& etsm,
                     const T& tensor,
                     const TA::TiledRange& trange,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    SparseMap<ElementIndex, ElementIndex> esm(etsm);
    return from_sparse_map(esm, tensor, trange, ind2mode);
}

template<typename T>
auto from_sparse_map(const SparseMap<TileIndex, ElementIndex>& tesm,
                     const T& tensor,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    SparseMap<ElementIndex, ElementIndex> esm(tesm);
    return from_sparse_map(esm, tensor, tesm.trange(), ind2mode);
}

template<typename T>
auto from_sparse_map(const SparseMap<TileIndex, TileIndex>& tsm,
                     const T& tensor,
                     const std::map<std::size_t, std::size_t>& ind2mode = {}) {
    SparseMap<ElementIndex, ElementIndex> esm(tsm);
    return from_sparse_map(esm, tsm.trange(), tensor, ind2mode);
}

} // namespace libchemist::sparse_map
