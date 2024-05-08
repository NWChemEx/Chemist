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
#include <chemist/chemical_system/molecule/molecule.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>
#include <chemist/traits/fragmenting_traits.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename MoleculeType>
class FragmentedMoleculePIMPL;

}

template<typename MoleculeType>
class FragmentedMolecule
  : public FragmentedBase<FragmentedMolecule<MoleculeType>> {
private:
    /// Type of this class
    using my_type = FragmentedMolecule<MoleculeType>;

    /// Type of the base class
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the object implementing *this
    using pimpl_type = detail_::FragmentedMoleculePIMPL<MoleculeType>;

    /// Type of a pointer to the implementation
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Struct defining the types *this uses
    using type_traits = ChemistClassTraits<my_type>;

    /// Struct defining the types used to fragment the Nuclei part of *this
    using fragmented_nuclei_traits =
      typename type_traits::fragmented_nuclei_traits;

    /// Type of the fragments
    using value_type = typename type_traits::fragment_type;

    /// Type of a mutable reference to a fragment
    using reference = typename type_traits::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename type_traits::const_fragment_reference;

    /// Type of the piece of *this representing the fragmented Nuclei object
    using fragmented_nuclei_type =
      typename fragmented_nuclei_traits::value_type;

    /// Type of each fragment's charge
    using charge_type = typename value_type::charge_type;

    /// Type of each fragment's multiplicity
    using multiplicity_type = typename value_type::multiplicity_type;

    /// Type supersystem uses for indexing and offsets
    using size_type = typename supersystem::size_type;

    FragmentedMolecule() noexcept;

    FragmentedMolecule(fragmented_nuclei_type nuclear_frags);

    /// No throw defaulted dtor
    ~FragmentedMolecule() noexcept;

protected:
    friend utilities::IndexableContainerBase<my_type>;

    /// Implements at() and operator[]
    reference at_(size_type i);

    /// Implements at() const and operator[] const
    const_reference at_(size_type i) const;

    /// Implements size
    size_type size_() const noexcept;

private:
    /// Code factorization for determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// The object which actually implements *this
    pimpl_pointer m_pimpl_;
};

extern template class FragmentedMolecule<Molecule>;
extern template class FragmentedMolecule<const Molecule>;

} // namespace chemist::fragmenting
