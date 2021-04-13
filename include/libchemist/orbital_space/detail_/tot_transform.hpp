#pragma once
#include "libchemist/ta_helpers/detail_/contraction_dummy_annotation.hpp"
#include <TiledArray/expressions/contraction_helpers.h>

namespace libchemist::orbital_space::detail_ {

template<typename SpaceType, typename TensorType>
auto tot_transform(SpaceType&& orbs, TensorType&& t,
                   const std::vector<std::size_t>& modes) {
    using tile_type = typename std::decay_t<TensorType>::value_type;
    using ta_helpers::detail_::contraction_dummy_annotations;

    const auto& C   = orbs.C();
    auto ind_rank   = t.range().rank();
    auto C_ind_rank = C.range().rank();
    TA_ASSERT(C_ind_rank == ind_rank && "Must have same independent ranks");

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
        TA_ASSERT(i < t_dep_rank &&
                  "Mode to transform can't be >= number of dependent modes");
    }

    // Return type would be a normal tensor if we're transforming all modes to
    // the independent space. We don't know the ranks at compile time, so we
    // can't use template metaprogramming to switch the return types, and thus
    // you can't use this function to do said transformation...
    if(is_independent) {
        TA_ASSERT(t_dep_rank > modes.size() &&
                  "tot_transform can't be used to transform all dependent"
                  "modes to independent modes.");
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

        throw std::runtime_error("Can't use transform() for ToT case");
    }
    return rv;
}
} // namespace libchemist::orbital_space::detail_