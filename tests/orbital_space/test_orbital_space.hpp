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
#include "chemist/orbital_space/orbital_space.hpp"

namespace testing {

inline auto get_canonical() {
    using tensor_type = chemist::type::tensor;
    using from_space  = chemist::orbital_space::AOSpaceD;
    using vector_il   = TA::detail::vector_il<double>;
    using matrix_il   = TA::detail::matrix_il<double>;

    tensor_type ei(vector_il{1.0, 2.0, 3.0});
    tensor_type c(matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}});

    return chemist::orbital_space::CanonicalSpaceD(ei, c, from_space{});
}

} // namespace testing
