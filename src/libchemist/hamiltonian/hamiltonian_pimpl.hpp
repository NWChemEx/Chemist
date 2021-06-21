#pragma once
#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <iostream>

namespace libchemist::detail_ {

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



class HamiltonianPIMPL {

  static constexpr std::size_t max_nbody = Hamiltonian::max_nbody;

  template <typename OpType>
  using term_container_type = 
    std::multimap< std::type_index, std::shared_ptr<OpType> >;

  template <std::size_t NE, std::size_t NN>
  using nbody_container = term_container_type<Operator<NE,NN>>;

public:

  HamiltonianPIMPL();
  HamiltonianPIMPL( const HamiltonianPIMPL& other );
  HamiltonianPIMPL( HamiltonianPIMPL&& other ) noexcept;
  ~HamiltonianPIMPL() noexcept;

  HamiltonianPIMPL& operator=( const HamiltonianPIMPL& other );
  HamiltonianPIMPL& operator=( HamiltonianPIMPL&& other ) noexcept;


  template <std::size_t NE, std::size_t NN>
  void add_term( std::type_index index, std::shared_ptr<Operator<NE,NN>>&& op ) {
    get<NE,NN>(terms_).insert( {index, std::move(op)} );
  }


  template <typename OpType>
  using get_return_type = Hamiltonian::get_return_type<OpType>;

  template <std::size_t NE, std::size_t NN>
  get_return_type<Operator<NE,NN>> get_terms( std::type_index index ) const {
    auto [b,e] = get<NE,NN>(terms_).equal_range( index );
    const std::size_t n_terms = std::distance(b,e);
    get_return_type<Operator<NE,NN>> ret_terms; ret_terms.reserve(n_terms);
    for( auto it = b; it != e; ++it )
      ret_terms.emplace_back(it->second);
    return ret_terms;
  }

  template <std::size_t NE, std::size_t NN>
  bool has_term( std::type_index index ) const noexcept {
    return get<NE,NN>(terms_).count(index);
  }


  std::unique_ptr<HamiltonianPIMPL> clone() const;

private:

  detail_::indexed_tuple_container<nbody_container,max_nbody+1,max_nbody+1> terms_;

};

}
