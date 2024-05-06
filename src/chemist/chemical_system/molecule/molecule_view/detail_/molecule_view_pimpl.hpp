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
    using nuclei_reference       = typename parent_type::nuclei_reference;
    using const_nuclei_reference = typename parent_type::const_nuclei_reference;
    using nelectron_pointer      = typename parent_type::nelectron_pointer;
    using multiplicity_pointer   = typename parent_type::multiplicity_pointer;
    ///@}

    MoleculeViewPIMPL(nuclei_reference nuclei, nelectron_pointer pn_electrons,
                      multiplicity_pointer pmultiplicity) :
      m_nuclei_(std::move(nuclei)),
      m_pn_electrons_(pn_electrons),
      m_pmultiplicity_(pmultiplicity) {}

    nuclei_reference nuclei() { return m_nuclei_; }

    const_nuclei_reference nuclei() const { return m_nuclei_; }

    /// Direct access to state (no checks for nullptr; left to parent_type)
    /// @{
    auto& n_electrons() { return *m_pn_electrons_; }
    const auto& n_electrons() const { return *m_pn_electrons_; }
    auto& multiplicity() { return *m_pmultiplicity_; }
    const auto& multiplicity() const { return *m_pmultiplicity_; }
    ///@}

private:
    /// The nuclear framework of *this
    nuclei_reference m_nuclei_;

    /// Pointer to the number of electrons in *this
    nelectron_pointer m_pn_electrons_;

    multiplicity_pointer m_pmultiplicity_;
};

} // namespace chemist::detail_
