#pragma once
#include "chemist/ta_helpers/einsum/einsum.hpp"
#include "chemist/ta_helpers/linalg_inner_tensors.hpp"
#include "chemist/ta_helpers/slice.hpp"
#include "chemist/ta_helpers/ta_helpers.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace chemist::ta_helpers {

template<typename T>
auto remove_redundancy(const T& C, const T& S, double thresh = 1.0E-8) {
    // Diagonalize S, compute diagonal of S^0.5 with redundant elements zeroed
    auto [evals, evecs]   = TA::heig(S);
    std::size_t n_nonzero = 0;
    decltype(evals) non0_evals;
    for(auto& x : evals) {
        if(x < thresh) continue;
        non0_evals.push_back(1.0 / std::sqrt(x));
        ++n_nonzero;
    }

    auto extents = evecs.elements_range().extent();
    std::size_t hi0(extents[0]), hi1(extents[1]);
    sparse_map::ElementIndex lo{0, hi1 - n_nonzero};
    sparse_map::ElementIndex hi{hi0, hi1};
    auto non0_evecs = slice(evecs, lo, hi);

    using tensor_type = std::decay_t<decltype(evecs)>;

    auto l = [=](auto& tile, const auto& range) {
        tile = std::decay_t<decltype(tile)>(range);
        for(auto i : range) tile(i) = non0_evals[i[0]];
        return tile.norm();
    };

    auto new_evals = TA::make_array<tensor_type>(
      evecs.world(), TA::TiledRange{non0_evecs.trange().dim(1)}, l);

    auto evecs_bar = einsum::einsum("i,j", "i,j", "j", non0_evecs, new_evals);
    // Make AO to NRPAO transform and return it
    T NewC;
    NewC("mu,nu") = C("mu,lambda") * evecs_bar("lambda,nu");

    return NewC;
}

template<typename T>
auto sparse_remove_redundancy(const T& C, const T& S, double thresh = 1.0E-8) {
    using tensor_type     = std::decay_t<T>;
    using outer_tile_type = typename tensor_type::value_type;
    using inner_tile_type = typename outer_tile_type::value_type;

    auto [evals, evecs] = diagonalize_inner_tensors(S);

    // Zero-out redundant eigenvalues, compute S**-1/2 for non-redundant
    for(outer_tile_type outer_tile : evals) {
        for(inner_tile_type inner_tile : outer_tile) {
            for(auto& x : inner_tile)
                x = x >= thresh ? 1.0 / std::sqrt(x) : 0.0;
        }
    }
    C.world().gop.fence();
    tensor_type evecs_bar, NewC;
    using TA::expressions::einsum;
    einsum(evecs_bar("i;b,a"), evals("i;a"), evecs("i;b,a"));
    einsum(NewC("i;mu,a"), C("i;mu,b"), evecs_bar("i;b,a"));
    return NewC;
}

} // namespace chemist::ta_helpers
