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

#include "chemist/types.hpp"
#include <catch2/catch.hpp>

namespace testing {

inline auto generate_tensor(unsigned int rank, double seed = 1.0) {
    using vector_il = TA::detail::vector_il<double>;
    using matrix_il = TA::detail::matrix_il<double>;
    using tensor3_il = TA::detail::tensor3_il<double>;
    using tensor4_il = TA::detail::tensor4_il<double>;

    if(rank == 1u) {
        return chemist::type::tensor(vector_il{seed, seed + 1.0, seed + 2.0});
    } else if(rank == 2u) {
        return chemist::type::tensor(
          matrix_il{vector_il{seed + 0.0, seed + 1.0, seed + 2.0},
                    vector_il{seed + 3.0, seed + 4.0, seed + 5.0},
                    vector_il{seed + 6.0, seed + 7.0, seed + 8.0}});
    } else if(rank == 3ul) {
        return chemist::type::tensor(tensor3_il{
          matrix_il{vector_il{seed + 00.0, seed + 01.0, seed + 02.0},
                    vector_il{seed + 03.0, seed + 04.0, seed + 05.0},
                    vector_il{seed + 06.0, seed + 07.0, seed + 08.0}},
          matrix_il{vector_il{seed + 09.0, seed + 10.0, seed + 11.0},
                    vector_il{seed + 12.0, seed + 13.0, seed + 14.0},
                    vector_il{seed + 15.0, seed + 16.0, seed + 17.0}},
          matrix_il{vector_il{seed + 18.0, seed + 19.0, seed + 20.0},
                    vector_il{seed + 21.0, seed + 22.0, seed + 23.0},
                    vector_il{seed + 24.0, seed + 25.0, seed + 26.0}}});
    } else if(rank == 4ul) {
        return chemist::type::tensor(tensor4_il{
          tensor3_il{
          matrix_il{vector_il{seed + 00.0, seed + 01.0, seed + 02.0},
                    vector_il{seed + 03.0, seed + 04.0, seed + 05.0},
                    vector_il{seed + 06.0, seed + 07.0, seed + 08.0}},
          matrix_il{vector_il{seed + 09.0, seed + 10.0, seed + 11.0},
                    vector_il{seed + 12.0, seed + 13.0, seed + 14.0},
                    vector_il{seed + 15.0, seed + 16.0, seed + 17.0}},
          matrix_il{vector_il{seed + 18.0, seed + 19.0, seed + 20.0},
                    vector_il{seed + 21.0, seed + 22.0, seed + 23.0},
                    vector_il{seed + 24.0, seed + 25.0, seed + 26.0}}},
          tensor3_il{
          matrix_il{vector_il{seed + 25.0, seed + 26.0, seed + 27.0},
                    vector_il{seed + 28.0, seed + 29.0, seed + 30.0},
                    vector_il{seed + 31.0, seed + 32.0, seed + 31.0}},
          matrix_il{vector_il{seed + 34.0, seed + 35.0, seed + 36.0},
                    vector_il{seed + 37.0, seed + 38.0, seed + 39.0},
                    vector_il{seed + 40.0, seed + 41.0, seed + 42.0}},
          matrix_il{vector_il{seed + 43.0, seed + 44.0, seed + 45.0},
                    vector_il{seed + 46.0, seed + 47.0, seed + 48.0},
                    vector_il{seed + 49.0, seed + 50.0, seed + 51.0}}},
          tensor3_il{
          matrix_il{vector_il{seed + 52.0, seed + 53.0, seed + 54.0},
                    vector_il{seed + 55.0, seed + 56.0, seed + 57.0},
                    vector_il{seed + 58.0, seed + 59.0, seed + 60.0}},
          matrix_il{vector_il{seed + 61.0, seed + 62.0, seed + 63.0},
                    vector_il{seed + 64.0, seed + 65.0, seed + 66.0},
                    vector_il{seed + 67.0, seed + 68.0, seed + 69.0}},
          matrix_il{vector_il{seed + 70.0, seed + 71.0, seed + 72.0},
                    vector_il{seed + 73.0, seed + 74.0, seed + 77.0},
                    vector_il{seed + 76.0, seed + 79.0, seed + 80.0}}}});
    } else {
        throw std::runtime_error("Ranks of 0, or higher than 4, are NYI.");
    }
    return chemist::type::tensor{};
}

} // namespace testing
