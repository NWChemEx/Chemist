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

template<typename... ParticleTypes>
class Density;

template<typename... ParticleTypes>
class DecomposableDensity;

template<typename... ParticleTypes>
struct ChemistClassTraits<Density<ParticleTypes...>> {
    using value_type      = Density<ParticleTypes...>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<typename... ParticleTypes>
struct ChemistClassTraits<const Density<ParticleTypes...>> {
    using value_type      = Density<ParticleTypes...>;
    using reference       = const value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<typename... ParticleTypes>
struct ChemistClassTraits<DecomposableDensity<ParticleTypes...>> {
    using value_type      = DecomposableDensity<ParticleTypes...>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<typename... ParticleTypes>
struct ChemistClassTraits<const DecomposableDensity<ParticleTypes...>> {
    using value_type      = DecomposableDensity<ParticleTypes...>;
    using reference       = const value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

} // namespace chemist
