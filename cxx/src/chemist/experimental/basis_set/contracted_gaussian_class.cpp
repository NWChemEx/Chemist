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

#include <chemist/experimental/basis_set/contracted_gaussian_class.hpp>
#include <utility>

namespace chemist::experimental {

void ContractedGaussian::swap(ContractedGaussian& other) noexcept {
    std::swap(m_coefficients_, other.m_coefficients_);
    std::swap(m_exponents_, other.m_exponents_);
    std::swap(m_l_, other.m_l_);
    m_center_.swap(other.m_center_);
}

} // namespace chemist::experimental
