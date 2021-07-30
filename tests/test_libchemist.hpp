#include "libchemist/types.hpp"
#include <catch2/catch.hpp>
#include <tiledarray.h>

namespace testing {

inline auto generate_tensor(unsigned int rank, double seed = 1.0) {
    auto& world = TA::get_default_world();

    using ta_tensor_type = TA::TSpArrayD;
    ta_tensor_type t;

    if(rank == 1u) {
        t = ta_tensor_type(world, {seed, seed + 1.0, seed + 2.0});
    } else if(rank == 2u) {
        t = ta_tensor_type(world, {{seed + 0.0, seed + 1.0, seed + 2.0},
                                   {seed + 3.0, seed + 4.0, seed + 5.0},
                                   {seed + 6.0, seed + 7.0, seed + 8.0}});
    } else if(rank == 3ul) {
        t = ta_tensor_type(world, {{{seed + 00.0, seed + 01.0, seed + 02.0},
                                    {seed + 03.0, seed + 04.0, seed + 05.0},
                                    {seed + 06.0, seed + 07.0, seed + 08.0}},
                                   {{seed + 09.0, seed + 10.0, seed + 11.0},
                                    {seed + 12.0, seed + 13.0, seed + 14.0},
                                    {seed + 15.0, seed + 16.0, seed + 17.0}},
                                   {{seed + 18.0, seed + 19.0, seed + 20.0},
                                    {seed + 21.0, seed + 22.0, seed + 23.0},
                                    {seed + 24.0, seed + 25.0, seed + 26.0}}});
    } else {
        throw std::runtime_error("Ranks of 0, or higher than 3, are NYI.");
    }
    return libchemist::type::tensor(t);
}

} // namespace testing
