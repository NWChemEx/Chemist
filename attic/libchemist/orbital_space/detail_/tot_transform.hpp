#pragma once
#include "chemist/ta_helpers/detail_/contraction_dummy_annotation.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace chemist::orbital_space::detail_ {

template<typename SpaceType, typename TensorType>
auto tot_transform(SpaceType&& orbs, TensorType&& t,
                   const std::vector<std::size_t>& modes) {
    using tile_type = typename std::decay_t<TensorType>::value_type;
    using ta_helpers::detail_::contraction_dummy_annotations;

    if(modes.empty()) return t;

    const auto& C   = orbs.C();
    auto ind_rank   = t.range().rank();
    auto C_ind_rank = C.range().rank();
    if(C_ind_rank != ind_rank)
        throw std::runtime_error("Must have same independent ranks");

    // Get a tile of each tensor to determine the inner rank
    auto C_tile      = C.find(0).get();
    auto C_dep_rank  = C_tile(std::vector<int>(ind_rank, 0)).range().rank();
    tile_type t_tile = t.find(0).get();
    auto t_dep_rank  = t_tile(std::vector<int>(ind_rank, 0)).range().rank();

    // We have a transformation to the independent space if there is only one
    // dependent index
    bool is_independent = C_dep_rank == 1;

    // Make sure the modes the user picked are in bounds
    for(const auto& i : modes) {
        if(i >= t_dep_rank)
            throw std::runtime_error("Mode to transform can't be >= number of "
                                     "dependent modes");
    }

    // Return type would be a normal tensor if we're transforming all modes to
    // the independent space. We don't know the ranks at compile time, so we
    // can't use template metaprogramming to switch the return types, and thus
    // you can't use this function to do said transformation...
    if(is_independent && (t_dep_rank == modes.size())) {
        throw std::runtime_error("tot_transform can't be used to transform all "
                                 "dependent modes to independent modes.");
    }

    // Make the independent part of the index
    std::string prefix;
    for(std::size_t i = 0; i < ind_rank; ++i) {
        if(i > 0) prefix += ",";
        prefix += "outer_idx_" + std::to_string(i);
    }
    prefix += ";";

    std::decay_t<TensorType> rv(t);

    if(!is_independent) {
        // We have something like C("i,j;mu,a"), are contracting against
        // something like t("i,j;mu,nu,d"), and getting something like
        // rv("i,j;a,nu,d")
        for(const auto& i : modes) {
            auto [start, finish, change] =
              contraction_dummy_annotations(t_dep_rank, i);
            const auto rv_idx = prefix + finish;
            const auto t_idx  = prefix + start;
            const auto c_idx  = prefix + change;
            TA::expressions::einsum(rv(rv_idx), rv(t_idx), C(c_idx));
        }
    } else {
        // We have something like C("i,j;mu"), are contracting against
        // something like t("i,j;mu,nu,d"), and getting something like
        // rv("i,j;nu,d")

        // N.B. T's dependent rank decreases everytime we do a transform which
        //      complicates the bookkeeping. For now we error if the user wants
        //      more than one mode transformed.
        if(modes.size() > 1)
            throw std::runtime_error("Can only transform 1 independent mode per"
                                     " call");
        auto mode        = modes[0];
        auto rv_idx      = prefix;
        auto t_idx       = prefix;
        const auto c_idx = prefix + "X";

        // For t_idx, with the exception of the 0-th index, we print a comma
        // before prepending the new index. For rv_idx we print a comma if i > 0
        // and i!= mode (if it equals the mode we're not appending an index). We
        // also need to avoid printing a comma if i == 1 and mode == 0
        int tm1      = int(t_dep_rank) - 1;
        int last_idx = mode != tm1 ? tm1 : tm1 - 1;

        for(std::size_t i = 0; i < t_dep_rank; ++i) {
            if(i > 0) t_idx += ",";

            if(mode == 0 && i > 1)
                rv_idx += ",";
            else if(mode != 0 && i > 0 && i != mode)
                rv_idx += ",";

            if(i == mode)
                t_idx += "X";
            else {
                t_idx += "inner_idx_" + std::to_string(i);
                rv_idx += "inner_idx_" + std::to_string(i);
            }
        }
        TA::expressions::einsum(rv(rv_idx), rv(t_idx), C(c_idx));
    }
    return rv;
}
} // namespace chemist::orbital_space::detail_