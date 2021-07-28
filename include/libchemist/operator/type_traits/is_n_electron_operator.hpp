#pragma once
#include "libchemist/operator/type_traits/is_operator.hpp"
#include <type_traits>

namespace libchemist {
namespace detail_ {

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

} // namespace detail_

/// Checks if all passed types are N-electron operator types
template<std::size_t N, typename... Ops>
inline constexpr bool are_n_electron_operators_v =
  std::conjunction_v<detail_::is_n_electron_operator<N, Ops>...>;

/// Alias to is_n_electron_operator<1,OpType>::value
template<typename OpType>
inline constexpr bool is_one_electron_operator_v =
  detail_::is_n_electron_operator<1, OpType>::value;

/// Checks if all passed types are 1-electron operator types
template<typename... Ops>
inline constexpr bool are_one_electron_operators_v =
  are_n_electron_operators_v<1, Ops...>;

/// Alias for enable_if_one_electron_operator<OpType,U>::type
template<typename OpType, typename U = void>
using enable_if_one_electron_operator_t =
  std::enable_if_t<is_one_electron_operator_v<OpType>, U>;

} // namespace libchemist
