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
#include <chemist/enums.hpp>

namespace chemist::basis_set::detail_ {

template<typename AngularMomentumType>
auto compute_n_aos(AngularMomentumType L, ShellType purity) {
    const bool is_pure = purity == ShellType::pure;
    // The "else" return value is the closed form for 2+L choose 2
    return is_pure ? (2ul * L + 1ul) : ((L * L + 3ul * L + 2ul) / 2ul);
}

} // namespace chemist::basis_set::detail_
