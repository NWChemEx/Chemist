#pragma once
#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <iostream>

namespace libchemist::detail_ {

#if 0
template <template <std::size_t...> typename T, std::size_t M, std::size_t... Ns>
auto make_indexed_tuple_container_row( std::index_sequence<Ns...> ) {
  return std::tuple< T<M,Ns> ... >();
}

template <template <std::size_t...> typename T, std::size_t M, std::size_t... Ms,
          std::size_t... Ns>
auto make_indexed_tuple_container( std::index_sequence<M,Ms...>,
                                   std::index_sequence<Ns...> ns ) {
  if constexpr (sizeof...(Ms))
    return std::tuple_cat( make_indexed_tuple_container_row<T,M>(ns),
                           make_indexed_tuple_container<T>( 
                             std::index_sequence<Ms...>{}, ns
                           ) );
  else
    return make_indexed_tuple_container_row<T,M>(ns);
}

template <template <std::size_t...> typename T, size_t M, size_t N>
using indexed_tuple_container_type =
  decltype(
    make_indexed_tuple_container<T>( std::declval<std::make_index_sequence<M>>(), 
                                     std::declval<std::make_index_sequence<N>>() )
  );

template <template <std::size_t...> typename T, size_t M, size_t N>
struct indexed_tuple_container {
  indexed_tuple_container_type<T,N,M> data;
};


template <size_t I, size_t J, template <std::size_t...> typename T, size_t M, size_t N>
auto& get( indexed_tuple_container<T,M,N>& t ){
  return std::get<I*N + J>( t.data );
}

template <size_t I, size_t J, template <std::size_t...> typename T, size_t M, size_t N>
const auto& get( const indexed_tuple_container<T,M,N>& t ){
  return std::get<I*N + J>( t.data );
}
#endif



class HamiltonianPIMPL {

  using operator_container_type = 
    std::multimap< std::type_index, std::shared_ptr<Operator> >;

public:

  HamiltonianPIMPL();
  HamiltonianPIMPL( const HamiltonianPIMPL& other );
  HamiltonianPIMPL( HamiltonianPIMPL&& other ) noexcept;
  ~HamiltonianPIMPL() noexcept;

  HamiltonianPIMPL& operator=( const HamiltonianPIMPL& other );
  HamiltonianPIMPL& operator=( HamiltonianPIMPL&& other ) noexcept;


  inline void add_term( std::type_index index, std::shared_ptr<Operator>&& op ) {
    terms_.insert( {index, std::move(op)} );
  }


  using get_return_type = Hamiltonian::get_return_type<Operator>;

  inline get_return_type get_terms( std::type_index index ) const {
    auto [b,e] = terms_.equal_range( index );
    const std::size_t n_terms = std::distance(b,e);
    get_return_type ret_terms; ret_terms.reserve(n_terms);
    for( auto it = b; it != e; ++it )
      ret_terms.emplace_back(it->second);
    return ret_terms;
  }

  inline bool has_term( std::type_index index ) const noexcept {
    return terms_.count(index);
  }


  std::unique_ptr<HamiltonianPIMPL> clone() const;

private:

  operator_container_type terms_;

};

}
