/*
 * Copyright 2022 NWChemEx-Project
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
#include <type_traits>

namespace chemist::orbital_space {

// Forward declare CanonicalSpace for TMP purposes
template<typename OrbitalEnergy, typename BaseType>
class CanonicalSpace;

namespace detail_ {

/** @brief Primary template for determining if @p T is an instance of
 *         CanonicalSpace.
 *
 *  The primary template is used whenever @p T is **NOT** an instance of
 *  CanonicalSpace. When instantiated the primary template will contain a
 *  static constexpr member "value" which is set to false.
 *
 *  @tparam T This type trait will deterimine if @p T is an instantiation of a
 *            CanonicalSpace.
 */
template<typename T>
struct IsCanonicalSpace : std::false_type {};

/** @brief Specializes IsCanonicalSpace for when @p T is a CanonicalSpace.
 *
 *  This partial specialization is selected when @p T is a CanonicalSpace
 *  instantiation. When instantiated, this partial specialization will contain a
 *  static constexpr member "value" which is set to true.
 *
 *  @tparam OrbitalEnergy The type used to store the orbital energies.
 *  @tparam BaseType The type that the CanonicalSpace derives from.
 *
 */
template<typename OrbitalEnergy, typename BaseType>
struct IsCanonicalSpace<CanonicalSpace<OrbitalEnergy, BaseType>>
  : std::true_type {};

} // namespace detail_

/** @brief Public API for determining if @p T is an instantiation of a
 *         CanonicalSpace.
 *
 *  This global variable is set to true if @p T is an instantiation of
 *  CanonicalSpace and false otherwise.
 *
 *  @tparam T The type we are inspecting.
 */
template<typename T>
constexpr auto is_canonical_space_v = detail_::IsCanonicalSpace<T>::value;

} // namespace chemist::orbital_space
