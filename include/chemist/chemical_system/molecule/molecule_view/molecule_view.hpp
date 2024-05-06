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
#include <chemist/traits/molecule_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename MoleculeType>
class MoleculeViewPIMPL;

}

template<typename MoleculeType>
class MoleculeView
  : public utilities::IndexableContainerBase<MoleculeView<MoleculeType>> {
private:
    /// Type of *this
    using my_type = MoleculeView<MoleculeType>;

    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    /// Type of the object implementing *this
    using pimpl_type = detail_::MoleculeViewPIMPL<MoleculeType>;

    /// Type of a pointer to the object implementing *this
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of the struct which defines the types for *this
    using traits_type = ChemistClassTraits<MoleculeType>;

    /// Type of the struct which defines the types for Atom objects in *this
    using atom_traits = typename traits_type::atom_traits;

    /// Type of the struct defining types for the Nuclei piece of *this
    using nuclei_traits = typename traits_type::nuclei_traits;

    /// Type of a reference to a Molecule object
    using molecule_reference = typename traits_type::reference;

    /// Type of a mutable reference to a Nuclei object
    using nuclei_reference = typename nuclei_traits::view_type;

    /// Type of a read-only reference to a Nuclei object
    using const_nuclei_reference = typename nuclei_traits::const_view_type;

    /// Type used to count the number of electrons
    using nelectron_type = typename atom_traits::nelectron_type;

    /// Type of a pointer to the number of electrons
    using nelectron_pointer = typename atom_traits::nelectron_pointer;

    /// Type used to hold the multiplicity
    using multiplicity_type = typename atom_traits::multiplciity_pointer;

    /// Type of a pointer to the multiplicity of a Molecule
    using multiplicity_pointer = typename atom_traits::multiplicity_pointer;

    /// Type of the molecule's charge
    using charge_type =
      typename atom_traits::nucleus_traits::point_charge_traits::charge_type;

    // -- Ctors, assignment, and dtor -----------------------------------------

    MoleculeView() noexcept;

    MoleculeView(molecule_reference mol);

    MoleculeView(nuclei_reference nuclei, nelectron_pointer nelectrons,
                 multiplicity_pointer multiplicity);

    MoleculeView(const MoleculeView& other);

    MoleculeView(MoleculeView&& other) noexcept;

    MoleculeView& operator=(const MoleculeView& rhs);

    MoleculeView& operator=(MoleculeView&& rhs) noexcept;

    ~MoleculeView() noexcept;

    // -- Getters and setters --------------------------------------------------

    nuclei_reference nuclei();

    const_nuclei_reference nuclei() const;

    nelectrons_type n_electrons() const noexcept;

    charge_type charge() const noexcept;

    void set_charge(charge_type charge);

    multiplicity_type multiplicity() const noexcept;

    void set_multiplicity(multiplicity_type multiplicity);

private:
    /// Type of the object implementing *this
    pimpl_pointer m_pimpl_;
};

extern template class MoleculeView<Molecule>;
extern template class MoleculeView<const Molecule>;

} // namespace chemist
