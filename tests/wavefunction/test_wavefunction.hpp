#pragma once
#include "libchemist/orbital_space/type_traits.hpp"
#include <catch2/catch.hpp>

/* This file contains functions which are useful for testing classes in the
 * wavefunction component of Libchemist.
 */

namespace testing {

// Makes dummy tensors of type "TensorType" with elements seeded from "seed"
template<typename TensorType>
auto make_tensor(double seed = 1.0) {
    using value_type = typename TensorType::value_type;

    auto& world = TA::get_default_world();
    if constexpr(TA::detail::is_tensor_of_tensor_v<value_type>) {
        using inner_type = typename value_type::value_type;
        inner_type inner(TA::Range{2, 1}, {seed, seed + 1.0});
        return TensorType(world, {inner, inner});
    } else {
        return TensorType(world, {seed, seed + 1.0});
    }
}

// Makes dummy spaces of type "space" with tensors seeded from "seed"
template<typename space>
auto make_space(double seed = 1.0) {
    using transform_type  = typename space::transform_type;
    using from_space_type = typename space::from_space_type;

    if constexpr(libchemist::orbital_space::is_canonical_space_v<space>) {
        return space(make_tensor<transform_type>(seed));
    } else {
        return space(make_tensor<transform_type>(seed), from_space_type{});
    }
}
} // namespace testing