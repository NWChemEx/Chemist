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

namespace chemist {
class Nucleus;
class Nuclei;

template<typename NucleusType>
class NucleusView;

template<typename NucleiType>
class NucleiView;

template<typename ChemicalSystemClass>
struct ChemicalSystemClassTraits;

#define CS_TRAITS(CS_CLASS)                                     \
    template<>                                                  \
    struct ChemicalSystemClassTraits<CS_CLASS> {                \
        using value_type      = CS_CLASS;                       \
        using reference       = CS_CLASS##View<CS_CLASS>;       \
        using const_reference = CS_CLASS##View<const CS_CLASS>; \
    };                                                          \
    template<>                                                  \
    struct ChemicalSystemClassTraits<const CS_CLASS> {          \
        using value_type      = CS_CLASS;                       \
        using reference       = CS_CLASS##View<const CS_CLASS>; \
        using const_reference = CS_CLASS##View<const CS_CLASS>; \
    }

CS_TRAITS(Nucleus);
CS_TRAITS(Nuclei);

#undef CS_TRAITS

} // namespace chemist
