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

#include <chemist/traits/chemist_class_traits.hpp>
#include <chemist/traits/nucleus_traits.hpp>

namespace chemist {
class Atom;
template<typename AtomType>
AtomView;
class Molecule;
template<typename MoleculeType>
MoleculeView;

template<>
struct ChemistClassTraits<Atom> {
    using value_type                   = Atom;
    using reference                    = value_type&;
    using const_reference              = const value_type&;
    using view_type                    = AtomView<value_type>;
    using const_view_type              = AtomView<const value_type>;
    using nelectron_type               = unsigned short;
    using nelectron_reference          = nelectron_type&;
    using const_nelectron_reference    = const nelectron_type&;
    using nelectron_pointer            = nelectron_type*;
    using const_nelectron_pointer      = const nelectron_type*;
    using multiplicity_type            = unsigned short;
    using multiplicity_reference       = multipliticy_type&;
    using const_multiplicity_reference = const multiplicity_type&;
    using multiplicity_pointer         = multipliticty_type*;
    using const_multiplicity_pointer   = const multiplicity_type*;
    using nucleus_traits               = ChemistClassTraits<Nucleus>;
};

template<>
struct ChemistClassTraits<const Atom> {
    using value_type                   = Atom;
    using reference                    = const value_type&;
    using const_reference              = const value_type&;
    using view_type                    = AtomView<const value_type>;
    using const_view_type              = AtomView<const value_type>;
    using nelectron_type               = unsigned short;
    using nelectron_reference          = const nelectron_type&;
    using const_nelectron_reference    = const nelectron_type&;
    using nelectron_pointer            = const nelectron_type*;
    using const_nelectron_pointer      = const nelectron_type*;
    using multiplicity_type            = unsigned short;
    using multiplicity_reference       = const multipliticy_type&;
    using const_multiplicity_reference = const multiplicity_type&;
    using multiplicity_pointer         = const multipliticty_type*;
    using const_multiplicity_pointer   = const multiplicity_type*;
    using nucleus_traits               = ChemistClassTraits<const Nucleus>;
};

template<>
struct ChemistClassTraits<Molecule> {
    using value_type      = Molecule;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using atom_traits   = ChemistClassTraits<Atom>;
    using nuclei_traits = ChemistClassTraits<Nuclei>;
};

template<>
struct ChemistClassTraits<const Molecule> {
    using value_type        = Molecule;
    using reference         = const value_type&;
    using const_reference   = const value_type&;
    using multiplicity_type = unsigned short;
    using atom_traits       = ChemistClassTraits<const Atom>;
    using nuclei_traits     = ChemistClassTraits<const Nuclei>;
};

} // namespace chemist
