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

namespace chemist {

class Electron;
class ManyElectrons;

template<>
struct ChemistClassTraits<Electron> {
    using value_type      = Electron;
    using reference       = value_type;
    using const_reference = value_type;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<>
struct ChemistClassTraits<const Electron> {
    using value_type      = Electron;
    using reference       = value_type;
    using const_reference = value_type;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<>
struct ChemistClassTraits<ManyElectrons> {
    using value_type      = ManyElectrons;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type&;
    using const_view_type = const value_type&;
};

template<>
struct ChemistClassTraits<const ManyElectrons> {
    using value_type      = ManyElectrons;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type&;
    using const_view_type = const value_type&;
};

} // namespace chemist