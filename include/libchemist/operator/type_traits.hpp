#pragma once
#include "libchemist/operator/operator.hpp"
#include <type_traits>
#include <memory>

namespace libchemist {
namespace detail_ {

template <typename OpType>
struct is_operator : 
  public std::is_base_of<Operator,std::decay_t<OpType>> {};
template <typename OpType>
struct is_density_dependent_operator : 
  public std::is_base_of<DensityDependentOperator,std::decay_t<OpType>> {};
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
struct enable_if_not_operator {
  using type = std::enable_if_t< not is_operator_v<OpType>, U >;
};

template <typename OpType, typename U = void>
using enable_if_operator_t = typename enable_if_operator<OpType,U>::type;

template <typename OpType, typename U = void>
using enable_if_not_operator_t = typename enable_if_not_operator<OpType,U>::type;





template <std::size_t N, typename OpType, typename = void>
struct is_n_electron_operator;

template <std::size_t N, typename OpType>
struct is_n_electron_operator<N,OpType,enable_if_not_operator_t<OpType>> : 
  public std::false_type {};

template <std::size_t N, typename OpType>
struct is_n_electron_operator<N,OpType,enable_if_operator_t<OpType>> {
    static constexpr bool value = std::decay_t<OpType>::n_electrons == N;
};

template <std::size_t N, typename... Ops>
inline constexpr bool are_n_electron_operators_v =
  std::conjunction_v< is_n_electron_operator<N,Ops>... >;

template <typename OpType>
inline constexpr bool is_one_electron_operator_v =
  is_n_electron_operator<1,OpType>::value;

template <typename... Ops>
inline constexpr bool are_one_electron_operators_v =
  are_n_electron_operators_v<1,Ops...>;


template <typename OpType, typename U = void>
struct enable_if_one_electron_operator {
  using type = std::enable_if_t<is_one_electron_operator_v<OpType>,U>;
};

template <typename OpType, typename U = void>
using enable_if_one_electron_operator_t =
  typename enable_if_one_electron_operator<OpType,U>::type;



}
}
