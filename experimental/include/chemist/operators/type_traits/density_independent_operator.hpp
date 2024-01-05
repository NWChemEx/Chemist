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
// #include "chemist/operators/type_traits/density_dependent_operator.hpp"
// #include "chemist/operators/type_traits/is_operator.hpp"
// #include <type_traits>

// namespace chemist::operators {
// namespace detail_ {

// /** Type trait to determine if a type is an density independent operator type
// at
//  * compile-time
//  *
//  *  @tparam OpType The type to check
//  *
//  *  Creates a bool_constant which is `true` if `OpType` is derived from
//  Operator
//  * but not DensityDependentOperator, `false`, otherwise.
//  */
// template<typename OpType>
// struct is_density_independent_operator
//   : public std::integral_constant<
//       bool, is_operator<OpType>::value and
//               not is_density_dependent_operator<OpType>::value> {};

// } // namespace detail_

// /// Alias to is_density_independent_operator<OpType>::value
// template<typename OpType>
// inline constexpr bool is_density_independent_operator_v =
//   detail_::is_density_independent_operator<OpType>::value;

// /// Checks if all operators in parameter pack are density independent
// operator
// /// types
// template<typename... Ops>
// inline constexpr bool all_are_density_independent_operator_v =
//   std::conjunction_v<detail_::is_density_independent_operator<Ops>...>;

// } // namespace chemist
