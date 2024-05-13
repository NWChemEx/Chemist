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
#include <chemist/traits/chemical_system_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <chemist/traits/molecule_traits.hpp>
#include <chemist/traits/nucleus_traits.hpp>

namespace chemist {
namespace fragmenting {
class CapSet;
template<typename DerivedType>
class FragmentedBase;
template<typename NucleiType>
class FragmentedNuclei;
template<typename MoleculeType>
class FragmentedMolecule;
template<typename ChemicalSystemType>
class FragmentedChemicalSystem;
} // namespace fragmenting

template<template<typename> class DerivedType, typename SupersystemType>
struct ChemistClassTraits<
  fragmenting::FragmentedBase<DerivedType<SupersystemType>>> {
    using supersystem_type   = SupersystemType;
    using value_type         = DerivedType<supersystem_type>;
    using reference          = value_type&;
    using const_reference    = const value_type&;
    using supersystem_traits = ChemistClassTraits<supersystem_type>;
    using fragment_type      = typename supersystem_traits::value_type;
    using fragment_reference = typename supersystem_traits::view_type;
    using const_fragment_reference =
      typename supersystem_traits::const_view_type;
};

#define FRAGMENTED_NUCLEI fragmenting::FragmentedNuclei<Nuclei>
template<>
struct ChemistClassTraits<FRAGMENTED_NUCLEI>
  : ChemistClassTraits<fragmenting::FragmentedBase<FRAGMENTED_NUCLEI>> {
    using cap_set_type            = fragmenting::CapSet;
    using cap_set_reference       = cap_set_type&;
    using const_cap_set_reference = const cap_set_type&;
};
#undef FRAGMENTED_NUCLEI

#define CONST_FRAGMENTED_NUCLEI fragmenting::FragmentedNuclei<const Nuclei>
template<>
struct ChemistClassTraits<CONST_FRAGMENTED_NUCLEI>
  : ChemistClassTraits<fragmenting::FragmentedBase<CONST_FRAGMENTED_NUCLEI>> {
    using cap_set_type            = fragmenting::CapSet;
    using cap_set_reference       = cap_set_type&;
    using const_cap_set_reference = const cap_set_type&;
};
#undef CONST_FRAGMENTED_NUCLEI

#define FRAG_MOLECULE fragmenting::FragmentedMolecule<Molecule>
template<>
struct ChemistClassTraits<FRAG_MOLECULE>
  : ChemistClassTraits<fragmenting::FragmentedBase<FRAG_MOLECULE>> {
    using fragmented_nuclei_traits =
      ChemistClassTraits<fragmenting::FragmentedNuclei<Nuclei>>;
};
#undef FRAG_MOLECULE

#define CONST_FRAG_MOLECULE fragmenting::FragmentedMolecule<const Molecule>
template<>
struct ChemistClassTraits<CONST_FRAG_MOLECULE>
  : ChemistClassTraits<fragmenting::FragmentedBase<CONST_FRAG_MOLECULE>> {
    using fragmented_nuclei_traits =
      ChemistClassTraits<fragmenting::FragmentedNuclei<const Nuclei>>;
};
#undef CONST_FRAG_MOLECULE

#define FRAG_CHEM_SYS fragmenting::FragmentedChemicalSystem<ChemicalSystem>
template<>
struct ChemistClassTraits<FRAG_CHEM_SYS>
  : public ChemistClassTraits<fragmenting::FragmentedBase<FRAG_CHEM_SYS>> {
    using fragmented_molecule_traits =
      ChemistClassTraits<fragmenting::FragmentedMolecule<Molecule>>;
};
#undef FRAG_CHEM_SYS

#define CONST_FRAG_CHEM_SYS \
    fragmenting::FragmentedChemicalSystem<const ChemicalSystem>
template<>
struct ChemistClassTraits<CONST_FRAG_CHEM_SYS>
  : public ChemistClassTraits<
      fragmenting::FragmentedBase<CONST_FRAG_CHEM_SYS>> {
    using fragmented_molecule_traits =
      ChemistClassTraits<fragmenting::FragmentedMolecule<const Molecule>>;
};
#undef CONST_FRAG_CHEM_SYS

} // namespace chemist
