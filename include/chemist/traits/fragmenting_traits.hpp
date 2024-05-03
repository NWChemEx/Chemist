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
namespace fragmenting {
class CapSet;
template<typename NucleiType>
class FragmentedNuclei;
class FragmentedMolecule;
class FragmentedChemicalSystem;
} // namespace fragmenting

template<>
struct ChemistClassTraits<fragmenting::FragmentedNuclei<Nuclei>> {
    using supersystem_type   = Nuclei;
    using value_type         = fragmenting::FragmentedNuclei<supersystem_type>;
    using reference          = value_type&;
    using const_reference    = const value_type&;
    using supersystem_traits = ChemistClassTraits<supersystem_type>;
    using fragment_type      = typename supersystem_traits::value_type;
    using fragment_reference = typename supersystem_traits::view_type;
    using const_fragment_reference =
      typename supersystem_traits::const_view_type;
    using cap_set_type = fragmenting::CapSet;
};

template<>
struct ChemistClassTraits<fragmenting::FragmentedNuclei<const Nuclei>> {
    using supersystem_type   = const Nuclei;
    using value_type         = fragmenting::FragmentedNuclei<supersystem_type>;
    using reference          = value_type&;
    using const_reference    = const value_type&;
    using supersystem_traits = ChemistClassTraits<supersystem_type>;
    using fragment_type      = typename supersystem_traits::value_type;
    using fragment_reference = typename supersystem_traits::view_type;
    using const_fragment_reference =
      typename supersystem_traits::const_view_type;
    using cap_set_type = fragmenting::CapSet;
};

} // namespace chemist
