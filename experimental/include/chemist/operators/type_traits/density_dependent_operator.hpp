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

// #pragma once
// #include <type_traits>

// namespace chemist::operators {

// template<typename Particle>
// class Density;

// namespace detail_ {

// /** Type trait to determine if a type is an density dependent operator type
// at
//  * compile-time
//  *
//  *  @tparam OpType The type to check
//  *
//  *  Creates a bool_constant which is `true` if `OpType` is derived from
//  * DensityDependentOperator, `false`, otherwise.
//  */
// template<typename T>
// struct IsDensity : std::false_type {};

// template<typename T>
// struct IsDensity<Density<T>> : std::true_type {};

// } // namespace detail_

// // /// Alias to is_density_dependent_operator<OpType>::value
// // template<typename OpType>
// // inline constexpr bool is_density_dependent_operator_v =
// //   detail_::is_density_dependent_operator<OpType>::value;

// // /// Checks if all operators in parameter pack are density dependent
// operator
// // /// types
// // template<typename... Ops>
// // inline constexpr bool all_are_density_dependent_operator_v =
// //   std::conjunction_v<detail_::is_density_dependent_operator<Ops>...>;

// } // namespace chemist
