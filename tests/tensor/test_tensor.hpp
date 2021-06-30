/* Functions, types, and includes common to the unit tests focusing on testing
 * the tensor component of LibChemist.
 */
#pragma once
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/tensor.hpp"
#include <catch2/catch.hpp>

namespace testing {

template<typename TensorType>
auto get_tensors() {
    auto& world = TA::get_default_world();
    std::map<std::string, TensorType> rv;
    if constexpr (!libchemist::tensor::is_tot_v<TensorType>) {
        rv["vector"] = TensorType(world, {1.0, 2.0, 3.0});
        rv["matrix"] = TensorType(world, {{1.0, 2.0}, {3.0, 4.0}});
        rv["tensor"] = TensorType(world, {{{1.0, 2.0}, {3.0, 4.0}},
                                          {{5.0, 6.0}, {7.0, 8.0}}});
    } else{
        using outer_tile = typename TensorType::value_type;
        using inner_tile = typename outer_tile::value_type;
        inner_tile v0(TA::Range({2}), {1.0, 2.0});
        inner_tile v1(TA::Range({2}), {3.0, 4.0});
        inner_tile v2(TA::Range({2}), {5.0, 6.0});
        inner_tile v3(TA::Range({2}), {7.0, 8.0});
        inner_tile mat0(TA::Range({2, 2}), {1.0, 2.0, 3.0, 4.0});
        inner_tile mat1(TA::Range({2, 2}), {5.0, 6.0, 7.0, 8.0});
        rv["vector-of-vectors"] = TensorType(world, {v0, v1});
        rv["matrix-of-vectors"] = TensorType(world, {{v0, v1}, {v2, v3}});
        rv["vector-of-matrices"] = TensorType(world, {mat0, mat1});
    }
    return rv;
}

}
