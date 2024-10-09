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

#include "detail_/molecule_view_pimpl.hpp"

namespace chemist {

#define TPARAMS template<typename MoleculeType>
#define MOLECULE_VIEW MoleculeView<MoleculeType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
MOLECULE_VIEW::MoleculeView() noexcept = default;

TPARAMS
MOLECULE_VIEW::MoleculeView(molecule_reference mol) {
    // Need to ensure mol.nuclei is called first in case it allocates a PIMPL
    auto nuclei_ref = mol.nuclei();
    MoleculeView temp(std::move(nuclei_ref), mol.charge_data(),
                      mol.multiplicity_data());
    temp.swap(*this);
}

TPARAMS
MOLECULE_VIEW::MoleculeView(nuclei_reference nuclei, charge_pointer pcharge,
                            multiplicity_pointer pmultiplicity) :
  m_pimpl_(
    std::make_unique<pimpl_type>(std::move(nuclei), pcharge, pmultiplicity)) {
    // Sanity check that charge and multiplicity make sense
    if(pcharge != nullptr) check_charge_(*pcharge);
    if(pmultiplicity != nullptr) check_multiplicity_(*pmultiplicity);
}

TPARAMS
MOLECULE_VIEW::MoleculeView(const MoleculeView& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS
MOLECULE_VIEW::MoleculeView(MoleculeView&& other) noexcept = default;

TPARAMS
MOLECULE_VIEW& MOLECULE_VIEW::operator=(const MoleculeView& rhs) {
    if(this != &rhs) MoleculeView(rhs).swap(*this);
    return *this;
}

TPARAMS
MOLECULE_VIEW& MOLECULE_VIEW::operator=(MoleculeView&& rhs) noexcept = default;

TPARAMS MOLECULE_VIEW::~MoleculeView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

TPARAMS
typename MOLECULE_VIEW::nuclei_reference MOLECULE_VIEW::nuclei() {
    return has_pimpl_() ? m_pimpl_->nuclei() : nuclei_reference{};
}

TPARAMS
typename MOLECULE_VIEW::const_nuclei_reference MOLECULE_VIEW::nuclei() const {
    return has_pimpl_() ? std::as_const(*m_pimpl_).nuclei() :
                          const_nuclei_reference{};
}

TPARAMS
typename MOLECULE_VIEW::size_type MOLECULE_VIEW::n_electrons() const noexcept {
    if(!has_pimpl_()) return 0;
    auto sum = neutral_n_electrons_();
    if(charge() < 0) return sum + size_type(-1 * charge());
    return sum - static_cast<size_type>(charge());
}

TPARAMS
typename MOLECULE_VIEW::charge_type MOLECULE_VIEW::charge() const noexcept {
    if(has_pimpl_()) {
        auto pcharge = std::as_const(*m_pimpl_).charge_data();
        if(pcharge) return *pcharge;
    }
    return 0;
}

TPARAMS
template<typename, typename>
void MOLECULE_VIEW::set_charge(charge_type charge) {
    check_charge_(charge);
    if(has_pimpl_()) {
        auto pcharge = m_pimpl_->charge_data();
        if(pcharge) *pcharge = charge;
    }
}

TPARAMS
typename MOLECULE_VIEW::multiplicity_type MOLECULE_VIEW::multiplicity()
  const noexcept {
    if(has_pimpl_()) {
        auto pmult = std::as_const(*m_pimpl_).multiplicity_data();
        if(pmult) return *pmult;
    }
    return 1;
}

TPARAMS
template<typename, typename>
void MOLECULE_VIEW::set_multiplicity(multiplicity_type multiplicity) {
    check_multiplicity_(multiplicity);
    if(has_pimpl_()) {
        auto pmult = m_pimpl_->multiplicity_data();
        if(pmult) *pmult = multiplicity;
    }
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
typename MOLECULE_VIEW::molecule_type MOLECULE_VIEW::as_molecule() const {
    return molecule_type(charge(), multiplicity(), nuclei().as_nuclei());
}

TPARAMS
typename MOLECULE_VIEW::const_charge_pointer MOLECULE_VIEW::charge_data()
  const noexcept {
    return has_pimpl_() ? std::as_const(*m_pimpl_).charge_data() : nullptr;
}

TPARAMS
typename MOLECULE_VIEW::const_multiplicity_pointer
MOLECULE_VIEW::multiplicity_data() const noexcept {
    return has_pimpl_() ? std::as_const(*m_pimpl_).multiplicity_data() :
                          nullptr;
}

TPARAMS
void MOLECULE_VIEW::swap(MoleculeView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool MOLECULE_VIEW::operator==(const MoleculeView& rhs) const noexcept {
    if(this->size() != rhs.size()) return false;
    if(this->size() == 0) return true; // Both empty
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool MOLECULE_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
typename MOLECULE_VIEW::size_type MOLECULE_VIEW::neutral_n_electrons_()
  const noexcept {
    size_type sum = 0;
    // N.B. *this is const so nuclei won't allocate a PIMPL
    for(const auto& n0 : this->nuclei()) sum += n0.Z();
    return sum;
}

TPARAMS
void MOLECULE_VIEW::check_charge_(charge_type charge) const {
    if(!has_pimpl_() && charge != 0)
        throw std::runtime_error("Empty Molecule can only have a charge of 0");

    charge_type n_electrons = neutral_n_electrons_();
    if(n_electrons < charge)
        throw std::runtime_error("Molecule does not have enough electrons");
}

TPARAMS
void MOLECULE_VIEW::check_multiplicity_(multiplicity_type multiplicity) const {
    if(!has_pimpl_() && multiplicity != 1)
        throw std::runtime_error("Empty Molecule can only have a multiplicity"
                                 " of 1.");
    if(multiplicity == 1 && n_electrons() % 2)
        throw std::runtime_error(
          "Singlet not possible for odd number of electrons");
}

#undef MOLECULE_VIEW
#undef TPARAMS

template class MoleculeView<Molecule>;
template void MoleculeView<Molecule>::set_charge<Molecule>(
  typename MoleculeView<Molecule>::charge_type);
template void MoleculeView<Molecule>::set_multiplicity<Molecule>(
  typename MoleculeView<Molecule>::multiplicity_type);

template class MoleculeView<const Molecule>;

} // namespace chemist
