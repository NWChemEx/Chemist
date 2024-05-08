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
MOLECULE_VIEW::MoleculeView(molecule_reference mol) :
  MoleculeView(mol.nuclei(), mol.charge_data(), mol.multiplicity_data()) {}

TPARAMS
MOLECULE_VIEW::MoleculeView(nuclei_reference nuclei, charge_pointer pcharge,
                            multiplicity_pointer pmultiplicity) :
  m_pimpl_(
    std::make_unique<pimpl_type>(std::move(nuclei), pcharge, pmultiplicity)) {}

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
    return has_pimpl_() ? std::as_const(*m_pimpl_).charge() : 0;
}

TPARAMS
template<typename, typename>
void MOLECULE_VIEW::set_charge(charge_type charge) {
    charge_type q = charge;
    if(!has_pimpl_() && q != 0)
        throw std::runtime_error("Empty Molecule can only have a charge of 0");
    if(neutral_n_electrons_() < q)
        throw std::runtime_error("Molecule does not have enough electrons");
    if(has_pimpl_()) m_pimpl_->charge() = q;
}

TPARAMS
typename MOLECULE_VIEW::multiplicity_type MOLECULE_VIEW::multiplicity()
  const noexcept {
    return has_pimpl_() ? std::as_const(*m_pimpl_).multiplicity() : 1;
}

TPARAMS
template<typename, typename>
void MOLECULE_VIEW::set_multiplicity(multiplicity_type multiplicity) {
    auto mult = static_cast<multiplicity_type>(multiplicity);
    if(!has_pimpl_() && mult != 1)
        throw std::runtime_error("Empty Molecule can only have a multiplicity"
                                 " of 1.");
    if(mult == 1 && n_electrons() % 2)
        throw std::runtime_error(
          "Singlet not possible for odd number of electrons");
    if(has_pimpl_()) m_pimpl_->multiplicity() = mult;
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
typename MOLECULE_VIEW::const_charge_pointer MOLECULE_VIEW::charge_data()
  const noexcept {
    return has_pimpl_() ? &(std::as_const(*m_pimpl_).charge()) : nullptr;
}

TPARAMS
typename MOLECULE_VIEW::const_multiplicity_pointer
MOLECULE_VIEW::multiplicity_data() const noexcept {
    return has_pimpl_() ? &(std::as_const(*m_pimpl_).multiplicity()) : nullptr;
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
    for(const auto& n0 : this->nuclei()) sum += n0.Z();
    return sum;
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
