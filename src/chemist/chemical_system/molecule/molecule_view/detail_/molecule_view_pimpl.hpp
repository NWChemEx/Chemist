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

#pragma once
#include <chemist/chemical_system/molecule/molecule_view/molecule_view.hpp>
namespace chemist::detail_ {

template<typename MoleculeType>
class MoleculeViewPIMPL {
private:
    /// Type of *this
    using my_type = MoleculeViewPIMPL<MoleculeType>;

public:
    /// Type *this implements
    using parent_type = MoleculeView<MoleculeType>;

    /// Bring parent type's types into scope
    ///@{
    using pimpl_pointer          = typename parent_type::pimpl_pointer;
    using nuclei_reference       = typename parent_type::nuclei_reference;
    using const_nuclei_reference = typename parent_type::const_nuclei_reference;
    using charge_pointer         = typename parent_type::charge_pointer;
    using multiplicity_pointer   = typename parent_type::multiplicity_pointer;
    ///@}

    MoleculeViewPIMPL(nuclei_reference nuclei, charge_pointer pnet_electrons,
                      multiplicity_pointer pmultiplicity) :
      m_nuclei_(std::move(nuclei)),
      m_pnet_electrons_(pnet_electrons),
      m_pmultiplicity_(pmultiplicity) {}

    MoleculeViewPIMPL(const MoleculeViewPIMPL& other) = default;

    /// Deleted b/c parent_type implements generically
    ///@{
    MoleculeViewPIMPL(MoleculeViewPIMPL&&) noexcept        = delete;
    MoleculeViewPIMPL& operator=(const MoleculeViewPIMPL&) = delete;
    MoleculeViewPIMPL& operator=(MoleculeViewPIMPL&&)      = delete;
    ///@}

    /// Direct access to state (no checks for nullptr; left to parent_type)
    /// @{
    auto& nuclei() { return m_nuclei_; }
    const auto& nuclei() const { return m_nuclei_; }
    auto& charge() { return *m_pnet_electrons_; }
    const auto& charge() const { return *m_pnet_electrons_; }
    auto& multiplicity() { return *m_pmultiplicity_; }
    const auto& multiplicity() const { return *m_pmultiplicity_; }
    ///@}

    /// Returns a deep (maybe eventually polymorphic) copy of *this
    pimpl_pointer clone() const { return std::make_unique<my_type>(*this); }

    /// Determines if *this aliases the same Molecule as @p rhs
    bool operator==(const MoleculeViewPIMPL& rhs) const noexcept;

private:
    /// The nuclear framework of *this
    nuclei_reference m_nuclei_;

    /// Pointer to the net number of electrons in *this
    charge_pointer m_pnet_electrons_;

    /// Pointer to the multiplicity of *this
    multiplicity_pointer m_pmultiplicity_;
};

// -- Inline implementations

template<typename MoleculeType>
bool MoleculeViewPIMPL<MoleculeType>::operator==(
  const MoleculeViewPIMPL& rhs) const noexcept {
    if(m_nuclei_ != rhs.m_nuclei_) return false;
    if(m_nuclei_.size() == 0) return true;

    // Factor out logic to compare the pointers
    auto compare_pointers = [](auto&& plhs, auto&& prhs) {
        if(plhs != prhs) {
            // In theory shouldn't be nullptrs, but we check anyways
            if(plhs == nullptr || prhs == nullptr) return false;
            if(*plhs != *prhs) return false;
            return true;
        }
        return true;
    };
    if(!compare_pointers(m_pnet_electrons_, rhs.m_pnet_electrons_))
        return false;
    return compare_pointers(m_pmultiplicity_, rhs.m_pmultiplicity_);
}

} // namespace chemist::detail_
