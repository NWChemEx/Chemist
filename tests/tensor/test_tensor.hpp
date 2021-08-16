/* Functions, types, and includes common to the unit tests focusing on testing
 * the tensor component of LibChemist.
 */
#pragma once
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/tensor.hpp"
#include "libchemist/types.hpp"
#include <catch2/catch.hpp>
#include <utilities/type_traits/variant/cat.hpp>
namespace testing {

/// Type of a variant with all possible tensor types in it
using all_tensor_variant_t = utilities::type_traits::variant::cat_t<
  libchemist::tensor::type::tensor_variant,
  libchemist::tensor::type::tot_variant>;

/// Function which generates some dummy tensors for a given type
template<typename TensorType>
auto get_tensors() {
    auto& world = TA::get_default_world();
    std::map<std::string, TensorType> rv;
    if constexpr(!libchemist::tensor::TensorTraits<TensorType>::is_tot) {
	using vector_il = TA::detail::vector_il<double>;
        using matrix_il = TA::detail::matrix_il<double>;
        using tensor_il = TA::detail::tensor3_il<double>;

        rv["vector"] = TensorType(world, vector_il{1.0, 2.0, 3.0});
        rv["matrix"] = TensorType(world, matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}});
        rv["tensor"] = TensorType(
          world, tensor_il{matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}}, matrix_il{vector_il{5.0, 6.0}, vector_il{7.0, 8.0}}});
    } else {
        using outer_tile = typename TensorType::value_type;
        using inner_tile = typename outer_tile::value_type;
	using dvector_il = TA::detail::vector_il<double>;
	using vector_il  = TA::detail::vector_il<inner_tile>;
        using matrix_il  = TA::detail::matrix_il<inner_tile>;

        inner_tile v0(TA::Range({2}), {1.0, 2.0});
        inner_tile v1(TA::Range({2}), {3.0, 4.0});
        inner_tile v2(TA::Range({2}), {5.0, 6.0});
        inner_tile v3(TA::Range({2}), {7.0, 8.0});
        inner_tile mat0(TA::Range({2, 2}), dvector_il{1.0, 2.0, 3.0, 4.0});
        inner_tile mat1(TA::Range({2, 2}), dvector_il{5.0, 6.0, 7.0, 8.0});
        rv["vector-of-vectors"]  = TensorType(world, vector_il{v0, v1});
        rv["matrix-of-vectors"]  = TensorType(world, matrix_il{vector_il{v0, v1}, vector_il{v2, v3}});
        rv["vector-of-matrices"] = TensorType(world, vector_il{mat0, mat1});
    }
    return rv;
}

} // namespace testing
