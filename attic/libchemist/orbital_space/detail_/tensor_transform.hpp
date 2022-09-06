/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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