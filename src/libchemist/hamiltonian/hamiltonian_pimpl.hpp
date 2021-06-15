#pragma once
#include "libchemist/hamiltonian/hamiltonian.hpp"

namespace libchemist::detail_ {

template <template <std::size_t> typename T, std::size_t... Ns>
auto make_indexed_tuple_container( std::index_sequence<Ns...> ) {
  return std::tuple< T<Ns+1>...>();
}

template <template <std::size_t> typename T, size_t N>
using indexed_tuple_container = 
  decltype(
    make_indexed_tuple_container<T>(std::declval<std::make_index_sequence<N>>())
  );




class HamiltonianPIMPL {

  static constexpr std::size_t max_nbody = Hamiltonian::max_nbody;

  template <typename OpType>
  using term_container_type = 
    std::multimap< std::size_t, std::shared_ptr<OpType> >;

  template <std::size_t N>
  using nbody_container = term_container_type<Operator<N>>;

public:

  HamiltonianPIMPL();
  HamiltonianPIMPL( const HamiltonianPIMPL& other );
  HamiltonianPIMPL( HamiltonianPIMPL&& other ) noexcept;
  ~HamiltonianPIMPL() noexcept;

  HamiltonianPIMPL& operator=( const HamiltonianPIMPL& other );
  HamiltonianPIMPL& operator=( HamiltonianPIMPL&& other ) noexcept;


  template <std::size_t N>
  void add_term( std::size_t hash, std::shared_ptr<Operator<N>>&& op ) {
    std::get<N-1>(terms_).insert( {hash, std::move(op)} );
  }

  template <std::size_t N>
  inline const nbody_container<N>& terms() const {
    return std::get<N-1>(terms_);
  }
  template <std::size_t N>
  inline nbody_container<N>& terms() {
    return std::get<N-1>(terms_);
  }


  std::unique_ptr<HamiltonianPIMPL> clone() const;

private:

  detail_::indexed_tuple_container<nbody_container,max_nbody> terms_;

};

}
