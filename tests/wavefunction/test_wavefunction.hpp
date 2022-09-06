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
#include "chemist/orbital_space/type_traits/type_traits.hpp"
#include "chemist/types.hpp"
#include <catch2/catch.hpp>
#include <tensorwrapper/tensor/tensor.hpp>

/* This file contains functions which are useful for testing classes in the
 * wavefunction component of Libchemist.
 */

namespace testing {

// Makes dummy tensors of type "TensorType" with elements seeded from "seed"
template<typename TensorType>
auto make_tensor(double seed = 1.0) {
    using field_type = typename TensorType::field_type;

    constexpr bool is_tot =
      std::is_same_v<field_type, tensorwrapper::tensor::field::Tensor>;

    auto& world = TA::get_default_world();
    if constexpr(is_tot) {
        throw std::runtime_error("ToT make_tensor NYI");
    } else {
        using vector_il = TA::detail::vector_il<double>;
        using matrix_il = TA::detail::matrix_il<double>;
        return TensorType(matrix_il{vector_il{seed}, vector_il{seed + 1.0}});
    }
}

// Makes dummy spaces of type "space" with tensors seeded from "seed"
template<typename space>
auto make_space(double seed = 1.0) {
    using transform_type  = typename space::transform_type;
    using from_space_type = typename space::from_space_type;

    if constexpr(chemist::orbital_space::is_canonical_space_v<space>) {
        return space(make_tensor<transform_type>(seed));
    } else if constexpr(chemist::orbital_space::is_independent_space_v<space>) {
        using base_space = typename space::base_space_type;
        return space(base_space(make_tensor<transform_type>(seed)));
    } else {
        return space(make_tensor<transform_type>(seed), from_space_type{});
    }
}
} // namespace testing
