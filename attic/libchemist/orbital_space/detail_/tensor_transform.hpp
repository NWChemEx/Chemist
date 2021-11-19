#pragma once
#include "chemist/ta_helpers/detail_/contraction_dummy_annotation.hpp"
namespace chemist::orbital_space::detail_ {

template<typename SpaceType, typename TensorType>
auto tensor_transform(SpaceType&& orbs, TensorType&& t,
                      const std::vector<std::size_t>& modes) {
    const auto t_rank = t.range().rank();
    for(const auto& i : modes) {
        if(i >= t_rank) {
            throw std::runtime_error(
              "Modes to be transformed incompatible with input tensor");
        }
    }

    std::decay_t<TensorType> rv(t);
    const auto& C_ao2x = orbs.C();
    using ta_helpers::detail_::contraction_dummy_annotations;
    auto n_modes = rv.range().rank();
    for(const auto& i : modes) {
        auto [start, finish, change] = contraction_dummy_annotations(t_rank, i);
        rv(finish)                   = rv(start) * C_ao2x(change);
    }
    return rv;
}

} // namespace chemist::orbital_space::detail_