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

#include <chemist/experimental/basis_set/cca_shell_class.hpp>
#include <chemist/experimental/basis_set/spherical_ao_class.hpp>
#include <memory>
#include <utility>

namespace chemist::experimental {

// The ctors which build a shell live here, rather than in the header, because
// the shell headers include spherical_ao_class.hpp; see the note on
// SphericalAO.

SphericalAO::SphericalAO() :
  m_shell_(std::make_unique<CCAShell<CartesianAO>>()) {}

SphericalAO::SphericalAO(contracted_gaussian_type cg, magnetic_index_type m) :
  m_shell_((check_m(cg.get_l(), m),
            std::make_unique<CCAShell<CartesianAO>>(std::move(cg)))),
  m_m_(m) {}

SphericalAO::SphericalAO(const SphericalAO& other) :
  m_shell_(other.m_shell_->clone()), m_m_(other.m_m_) {}

SphericalAO& SphericalAO::operator=(const SphericalAO& other) {
    if(this != &other) SphericalAO(other).swap(*this);
    return *this;
}

void SphericalAO::swap(SphericalAO& other) noexcept {
    m_shell_.swap(other.m_shell_);
    std::swap(m_m_, other.m_m_);
}

} // namespace chemist::experimental
