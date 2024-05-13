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
#include <chemist/chemical_system/chemical_system/chemical_system.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_molecule.hpp>
#include <chemist/traits/fragmenting_traits.hpp>

namespace chemist::fragmenting {
namespace detail_ {
template<typename ChemicalSystemType>
class FragmentedChemicalSystemPIMPL;

}

template<typename ChemicalSystemType>
class FragmentedChemicalSystem
  : public FragmentedBase<FragmentedChemicalSystem<ChemicalSystemType>> {
private:
    /// Type of *this
    using my_type = FragmentedChemicalSystem<ChemicalSystemType>;

    /// Type of the base class
    using base_type = FragmentedBase<my_type>;

public:
    /// Type of the PIMPL implementing *this
    using pimpl_type =
      detail_::FragmentedChemicalSystemPIMPL<ChemicalSystemType>;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Struct defining the types *this uses
    using type_traits = ChemistClassTraits<my_type>;

    /// Struct defining types for the FragmentedMolecule part of *this
    using fragmented_molecule_traits =
      typename type_traits::fragmented_molecule_traits;

    /// Type of a fragment
    using value_type = typename type_traits::fragment_type;

    /// Type of a reference to a fragment
    using reference = typename type_traits::fragment_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename type_traits::const_fragment_reference;

    /// Type of an object managing the FragmentedMolecule piece of *this
    using fragmented_molecule_type =
      typename fragmented_molecule_traits::value_type;

    /// Pull in base class's types
    ///@{
    using typename base_type::const_supersystem_reference;
    using typename base_type::supersystem_reference;
    using typename base_type::supersystem_type;
    ///@}

    /// Type the superysystem uses for indexing and offsets
    using size_type = typename supersystem_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    FragmentedChemicalSystem() noexcept;

    // FragmentedChemicalSystem(fragmented_molecule_type frags);

    /// Defaulted no-throw dtor
    ~FragmentedChemicalSystem() noexcept;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state in *this with that in @p other.
     *
     *  @param[in,out] other The instance to exchange state with. After this
     *                       method @p other will contain the state which was
     *                       previously in *this.
     *
     *  @throw None No throw gurantee.
     */
    void swap(FragmentedChemicalSystem& other) noexcept;

    bool operator==(const FragmentedChemicalSystem& rhs) const noexcept;

    bool operator!=(const FragmentedChemicalSystem& rhs) const noexcept;

protected:
    /// Allows base class to access implementations
    friend utilities::IndexableContainerBase<my_type>;

    /// Implements at() and operator[]
    reference at_(size_type i);

    /// Implements at() const and operator[] const
    const_reference at_(size_type i) const;

    /// Implements size
    size_type size_() const noexcept;

    /// Implements supersystem()
    virtual supersystem_reference supersystem_() override;

    /// Implements supersystem() const
    virtual const_supersystem_reference supersystem_() const override;

private:
    /// Code factorization for determining if *this has a PIMPL
    bool has_pimpl_() const noexcept;

    /// The object which actually implements *this
    pimpl_pointer m_pimpl_;
};

extern template class FragmentedChemicalSystem<ChemicalSystem>;
extern template class FragmentedChemicalSystem<const ChemicalSystem>;

} // namespace chemist::fragmenting
