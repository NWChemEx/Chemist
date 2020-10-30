#pragma once
#include <TiledArray/expressions/contraction_helpers.h>
#include <libchemist/ta_helpers/einsum/einsum.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

namespace libchemist::ta_helpers {

template<typename T>
auto remove_redundancy(const T& C, const T& S, double thresh = 1.0E-8) {
    // Diagonalize S, compute diagonal of S^0.5 with redundant elements zeroed
    auto [evals, evecs] = TA::heig(S);
    for(auto& x : evals) x = x >= thresh ? 1.0 / std::sqrt(x) : 0.0;
    using tensor_type = std::decay_t<decltype(evecs)>;

    auto l = [=](auto& tile, const auto& range) {
        tile = std::decay_t<decltype(tile)>(range);
        for(auto i : range) tile(i) = evals[i[0]];
        return tile.norm();
    };
    auto new_evals = TA::make_array<tensor_type>(
      evecs.world(), TA::TiledRange{evecs.trange().dim(0)}, l);

    // TODO: use einsum from TA
    //    T evecs_bar;
    //    TA::expressions::einsum(evecs_bar("i,j"), evecs("i,j"),
    //    new_evals("j"));

    auto evecs_bar = einsum::einsum("i,j", "i,j", "j", evecs, new_evals);
    // Make AO to NRPAO transform and return it
    T NewC;
    NewC("mu,nu") = C("mu,lambda") * evecs_bar("lambda,nu");

    return NewC;
}

} // namespace libchemist::ta_helpers
