#pragma once
#include "libchemist/hamiltonian/operator.hpp"
#include <type_traits>
#include <memory>

namespace libchemist {
namespace detail_ {

template <typename OpType>
struct is_operator : 
  public std::is_base_of<Operator,OpType> {};
template <typename OpType>
struct is_density_dependent_operator : 
  public std::is_base_of<DensityDependentOperator,OpType> {};
template <typename OpType>
struct is_density_independent_operator :
  public std::integral_constant<bool,
    is_operator<OpType>::value and 
    not is_density_dependent_operator<OpType>::value > {};

template <typename OpType>
inline constexpr bool is_operator_v = 
  is_operator<OpType>::value;
template <typename OpType>
inline constexpr bool is_density_independent_operator_v = 
  is_density_independent_operator<OpType>::value;
template <typename OpType>
inline constexpr bool is_density_dependent_operator_v = 
  is_density_dependent_operator<OpType>::value;


template <typename... Ops>
inline constexpr bool all_are_operator_v = 
  std::conjunction_v< is_operator<Ops>... >;
template <typename... Ops>
inline constexpr bool all_are_density_dependent_operator_v = 
  std::conjunction_v< is_density_dependent_operator<Ops>... >;
template <typename... Ops>
inline constexpr bool all_are_density_independent_operator_v = 
  std::conjunction_v< is_density_independent_operator<Ops>... >;


template <typename OpType, typename U = void>
struct enable_if_operator {
  using type = std::enable_if_t< is_operator_v<OpType>, U >;
};

template <typename OpType, typename U = void>
using enable_if_operator_t = typename enable_if_operator<OpType,U>::type;

}
}
