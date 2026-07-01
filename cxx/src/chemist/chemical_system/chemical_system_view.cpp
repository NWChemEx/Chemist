/*
 * Copyright 2024 NWChemEx-Project
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

#include "detail_/chemical_system_view_pimpl.hpp"

namespace chemist {

#define TPARAMS template<typename ChemicalSystemType>
#define CHEMICAL_SYSTEM_VIEW ChemicalSystemView<ChemicalSystemType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
CHEMICAL_SYSTEM_VIEW::ChemicalSystemView() noexcept = default;

TPARAMS
CHEMICAL_SYSTEM_VIEW::ChemicalSystemView(molecule_reference molecule) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(molecule))) {}

TPARAMS
CHEMICAL_SYSTEM_VIEW::ChemicalSystemView(const ChemicalSystemView& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS
CHEMICAL_SYSTEM_VIEW::ChemicalSystemView(ChemicalSystemView&& other) noexcept =
  default;

TPARAMS
CHEMICAL_SYSTEM_VIEW& CHEMICAL_SYSTEM_VIEW::operator=(
  const ChemicalSystemView& rhs) {
    if(this != &rhs) ChemicalSystemView(rhs).swap(*this);
    return *this;
}

TPARAMS
CHEMICAL_SYSTEM_VIEW& CHEMICAL_SYSTEM_VIEW::operator=(
  ChemicalSystemView&& rhs) noexcept = default;

TPARAMS
CHEMICAL_SYSTEM_VIEW::~ChemicalSystemView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and Setters
// -----------------------------------------------------------------------------

TPARAMS
typename CHEMICAL_SYSTEM_VIEW::molecule_reference
CHEMICAL_SYSTEM_VIEW::molecule() noexcept {
    return has_pimpl_() ? m_pimpl_->molecule() : molecule_reference{};
}

TPARAMS
typename CHEMICAL_SYSTEM_VIEW::const_molecule_reference
CHEMICAL_SYSTEM_VIEW::molecule() const noexcept {
    return has_pimpl_() ? std::as_const(*m_pimpl_).molecule() :
                          const_molecule_reference{};
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
void CHEMICAL_SYSTEM_VIEW::swap(ChemicalSystemView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

namespace {

template<typename LHSType, typename RHSType>
bool operator_equal_guts(LHSType&& lhs, RHSType&& rhs) noexcept {
    return lhs.molecule() == rhs.molecule();
}

} // namespace

TPARAMS
bool CHEMICAL_SYSTEM_VIEW::operator==(
  const ChemicalSystemView& rhs) const noexcept {
    return operator_equal_guts(*this, rhs);
}

TPARAMS
bool CHEMICAL_SYSTEM_VIEW::operator==(
  const_chemical_system_reference rhs) const noexcept {
    return operator_equal_guts(*this, rhs);
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool CHEMICAL_SYSTEM_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

#undef CHEMICAL_SYSTEM_VIEW
#undef TPARAMS

template class ChemicalSystemView<ChemicalSystem>;
template class ChemicalSystemView<const ChemicalSystem>;

} // namespace chemist
