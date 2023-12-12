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

#include "chemist/vector_space/ao_space.hpp"

namespace chemist::vector_space {

#define AOSPACE AOSpace<BasisType>

template<typename BasisType>
bool AOSPACE::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

template class AOSpace<basis_set::AOBasisSetD>;
template class AOSpace<basis_set::AOBasisSetF>;

#undef AOSPACE
} // namespace chemist::vector_space
