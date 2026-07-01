/*
 * Copyright 2024 NWChemEx
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
#include <chemist/traits/molecule_traits.hpp>

namespace chemist {

class ChemicalSystem;
template<typename ChemicalSystemType>
class ChemicalSystemView;

template<>
struct ChemistClassTraits<ChemicalSystem> {
    using value_type      = ChemicalSystem;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = ChemicalSystemView<value_type>;
    using const_view_type = ChemicalSystemView<const value_type>;
    using molecule_traits = ChemistClassTraits<Molecule>;
};

template<>
struct ChemistClassTraits<const ChemicalSystem> {
    using value_type      = ChemicalSystem;
    using reference       = const value_type&;
    using const_reference = const value_type&;
    using view_type       = ChemicalSystemView<const value_type>;
    using const_view_type = ChemicalSystemView<const value_type>;
    using molecule_traits = ChemistClassTraits<const Molecule>;
};

} // namespace chemist
