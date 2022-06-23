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
        /// Uncomment with removal of TW pin
        // return TensorType(matrix_il{vector_il{seed}, vector_il{seed + 1.0}});

        /// Remove with removal of TW pin
        using value_type = TA::DistArray<TA::Tensor<double>, TA::SparsePolicy>;
        auto& world      = TA::get_default_world();
        value_type t(world, matrix_il{vector_il{seed}, vector_il{seed + 1.0}});
        return TensorType(t);
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
