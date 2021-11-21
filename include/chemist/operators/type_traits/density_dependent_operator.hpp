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
