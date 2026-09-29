/*
 * Copyright 2026 NWChemEx-Project
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

#include <chemist/experimental/basis_set/cartesian_ao_class.hpp>
#include <utility>

namespace chemist::experimental {

void CartesianAO::swap(CartesianAO& other) noexcept {
    m_cg_.swap(other.m_cg_);
    std::swap(m_i_, other.m_i_);
    std::swap(m_j_, other.m_j_);
    std::swap(m_k_, other.m_k_);
}

} // namespace chemist::experimental
