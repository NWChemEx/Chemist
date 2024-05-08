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
#include <chemist/traits/point_charge_traits.hpp>
#include <string>

namespace chemist {
class Nucleus;
template<typename T>
class NucleusView;
class Nuclei;
template<typename T>
class NucleiView;

template<>
struct ChemistClassTraits<Nucleus> {
    using value_type                    = Nucleus;
    using reference                     = Nucleus&;
    using const_reference               = const Nucleus&;
    using view_type                     = NucleusView<value_type>;
    using const_view_type               = NucleusView<const value_type>;
    using name_type                     = std::string;
    using name_reference                = name_type&;
    using const_name_reference          = const name_type&;
    using name_pointer                  = name_type*;
    using const_name_pointer            = const name_type*;
    using atomic_number_type            = unsigned int;
    using atomic_number_reference       = atomic_number_type&;
    using const_atomic_number_reference = const atomic_number_type&;
    using atomic_number_pointer         = atomic_number_type*;
    using const_atomic_number_pointer   = const atomic_number_type*;
    using mass_type                     = double;
    using mass_reference                = mass_type&;
    using const_mass_reference          = const mass_type&;
    using mass_pointer                  = mass_type*;
    using const_mass_pointer            = const mass_type*;
    using point_charge_traits = ChemistClassTraits<PointCharge<double>>;
};

template<>
struct ChemistClassTraits<const Nucleus> {
    using value_type                    = Nucleus;
    using reference                     = const Nucleus&;
    using const_reference               = const Nucleus&;
    using view_type                     = NucleusView<const value_type>;
    using const_view_type               = NucleusView<const value_type>;
    using name_type                     = std::string;
    using name_reference                = const name_type&;
    using const_name_reference          = const name_type&;
    using name_pointer                  = const name_type*;
    using const_name_pointer            = const name_type*;
    using atomic_number_type            = unsigned int;
    using atomic_number_reference       = const atomic_number_type&;
    using const_atomic_number_reference = const atomic_number_type&;
    using atomic_number_pointer         = const atomic_number_type*;
    using const_atomic_number_pointer   = const atomic_number_type*;
    using mass_type                     = double;
    using mass_reference                = const mass_type&;
    using const_mass_reference          = const mass_type&;
    using mass_pointer                  = const mass_type*;
    using const_mass_pointer            = const mass_type*;
    using point_charge_traits = ChemistClassTraits<const PointCharge<double>>;
};

template<>
struct ChemistClassTraits<Nuclei> {
    using value_type      = Nuclei;
    using reference       = Nuclei&;
    using const_reference = const Nuclei&;
    using view_type       = NucleiView<value_type>;
    using const_view_type = NucleiView<const value_type>;
    using nucleus_traits  = ChemistClassTraits<Nucleus>;
    using charges_traits  = ChemistClassTraits<Charges<double>>;
};

template<>
struct ChemistClassTraits<const Nuclei> {
    using value_type      = Nuclei;
    using reference       = const Nuclei&;
    using const_reference = const Nuclei&;
    using view_type       = NucleiView<const value_type>;
    using const_view_type = NucleiView<const value_type>;
    using nucleus_traits  = ChemistClassTraits<const Nucleus>;
    using charges_traits  = ChemistClassTraits<const Charges<double>>;
};

} // namespace chemist
