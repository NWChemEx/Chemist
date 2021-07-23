#pragma once
#include "libchemist/operator/operator_class.hpp"
#include <memory>
#include <type_traits>

namespace libchemist {
namespace detail_ {

/** Type trait to determine if a type is an operator type at compile-time
 *
 *  @tparam OpType The type to check
 *
 *  Creates a bool_constant which is `true` if `OpType` is derived from
 * Operator, `false`, otherwise.
 */
template<typename OpType>
struct is_operator : public std::is_base_of<Operator, std::decay_t<OpType>> {};

/** Type trait to determine if a type is an density dependent operator type at
 * compile-time
 *
 *  @tparam OpType The type to check
 *
 *  Creates a bool_constant which is `true` if `OpType` is derived from
 * DensityDependentOperator, `false`, otherwise.
 */
template<typename OpType>
struct is_density_dependent_operator
  : public std::is_base_of<DensityDependentOperator, std::decay_t<OpType>> {};

/** Type trait to determine if a type is an density independent operator type at
 * compile-time
 *
 *  @tparam OpType The type to check
 *
 *  Creates a bool_constant which is `true` if `OpType` is derived from Operator
 * but not DensityDependentOperator, `false`, otherwise.
 */
template<typename OpType>
struct is_density_independent_operator
  : public std::integral_constant<
      bool, is_operator<OpType>::value and
              not is_density_dependent_operator<OpType>::value> {};

/// Alias to is_operator<OpType>::value
template<typename OpType>
inline constexpr bool is_operator_v = is_operator<OpType>::value;

/// Alias to is_density_independent_operator<OpType>::value
template<typename OpType>
inline constexpr bool is_density_independent_operator_v =
  is_density_independent_operator<OpType>::value;

/// Alias to is_density_dependent_operator<OpType>::value
template<typename OpType>
inline constexpr bool is_density_dependent_operator_v =
  is_density_dependent_operator<OpType>::value;

/// Checks if all operators in parameter pack are operator types
template<typename... Ops>
inline constexpr bool all_are_operator_v =
  std::conjunction_v<is_operator<Ops>...>;

/// Checks if all operators in parameter pack are density dependent operator
/// types
template<typename... Ops>
inline constexpr bool all_are_density_dependent_operator_v =
  std::conjunction_v<is_density_dependent_operator<Ops>...>;

/// Checks if all operators in parameter pack are density independent operator
/// types
template<typename... Ops>
inline constexpr bool all_are_density_independent_operator_v =
  std::conjunction_v<is_density_independent_operator<Ops>...>;

/// Enable-if alias for operator type checks
template<typename OpType, typename U = void>
struct enable_if_operator {
    using type = std::enable_if_t<is_operator_v<OpType>, U>;
};

/// Alias for enable_if_operator<OpType,U>::type
template<typename OpType, typename U = void>
using enable_if_operator_t = typename enable_if_operator<OpType, U>::type;

/** Type trait to check if a type is an operator type which takes exactly `N`
 * electrons.
 *
 *  @tparam N Number of desired electrons
 *  @tparam OpType Type to check
 *
 *  Default case: std::false_type
 */
template<std::size_t N, typename OpType, typename = void>
struct is_n_electron_operator : public std::false_type {};

/** Type trait to check if a type is an operator type which takes exactly `N`
 * electrons.
 *
 *  @tparam N Number of desired electrons
 *  @tparam OpType Type to check
 *
 *  Creates a bool_constant which is `true` if the passed type is an Operator
 * type and OpType::n_electrons == N. `false` otherwise.
 */
template<std::size_t N, typename OpType>
struct is_n_electron_operator<
  N, OpType, typename std::enable_if<is_operator_v<OpType>>::type>
  : std::integral_constant<bool, std::decay_t<OpType>::n_electrons == N> {};

/// Checks if all passed types are N-electron operator types
template<std::size_t N, typename... Ops>
inline constexpr bool are_n_electron_operators_v =
  std::conjunction_v<is_n_electron_operator<N, Ops>...>;

/// Alias to is_n_electron_operator<1,OpType>::value
template<typename OpType>
inline constexpr bool is_one_electron_operator_v =
  is_n_electron_operator<1, OpType>::value;

/// Checks if all passed types are 1-electron operator types
template<typename... Ops>
inline constexpr bool are_one_electron_operators_v =
  are_n_electron_operators_v<1, Ops...>;

/// Enable-if alias for is_one_electron_operator checks
template<typename OpType, typename U = void>
struct enable_if_one_electron_operator {
    using type = std::enable_if_t<is_one_electron_operator_v<OpType>, U>;
};

/// Alias for enable_if_one_electron_operator<OpType,U>::type
template<typename OpType, typename U = void>
using enable_if_one_electron_operator_t =
  typename enable_if_one_electron_operator<OpType, U>::type;

} // namespace detail_
} // namespace libchemist
