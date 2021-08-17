#pragma once
#include "libchemist/orbital_space/type_traits/type_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include <catch2/catch.hpp>

/* This file contains functions which are useful for testing classes in the
 * wavefunction component of Libchemist.
 */

namespace testing {

// Makes dummy tensors of type "TensorType" with elements seeded from "seed"
template<typename TensorType>
auto make_tensor(double seed = 1.0) {
    constexpr bool is_tot =
      std::is_same_v<TensorType, libchemist::type::tensor_of_tensors>;

    auto& world = TA::get_default_world();
    if constexpr(is_tot) {
        using ta_tot =
          libchemist::tensor::type::detail_::tensor_of_tensors<double>;
        using value_type = typename ta_tot::value_type;
        using inner_type = typename value_type::value_type;
        using vector_il  = TA::detail::vector_il<inner_type>;
        inner_type inner(TA::Range{2, 1}, {seed, seed + 1.0});
        return TensorType(ta_tot(world, vector_il{inner, inner}));
    } else {
        using value_type = TA::DistArray<TA::Tensor<double>, TA::SparsePolicy>;
        using vector_il  = TA::detail::vector_il<double>;
        using matrix_il  = TA::detail::matrix_il<double>;
        value_type t(world, matrix_il{vector_il{seed}, vector_il{seed + 1.0}});
        return TensorType(t);
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
