#pragma once
#include <type_traits>

namespace chemist::operators {

class OperatorBase;

namespace detail_ {

/** Type trait to determine if a type is an operator type at compile-time
 *
 *  @tparam OpType The type to check
 *
 *  Creates a bool_constant which is `true` if `OpType` is derived from
 *  OperatorBase, `false`, otherwise.
 *
 *  @note this trait only works if the operator inherits from OperatorBase.
 */
template<typename OpType>
struct is_operator
  : public std::is_base_of<OperatorBase, std::decay_t<OpType>> {};
} // namespace detail_

/// Alias to is_operator<OpType>::value
template<typename OpType>
inline constexpr bool is_operator_v = detail_::is_operator<OpType>::value;

/// Checks if all operators in parameter pack are operator types
template<typename... Ops>
inline constexpr bool all_are_operator_v =
  std::conjunction_v<detail_::is_operator<Ops>...>;

/// Alias for enable_if_operator<OpType,U>::type
template<typename OpType, typename U = void>
using enable_if_operator_t = std::enable_if_t<is_operator_v<OpType>, U>;

} // namespace chemist::operators
