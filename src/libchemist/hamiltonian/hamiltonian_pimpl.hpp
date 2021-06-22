#pragma once
#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <map>

namespace libchemist::detail_ {

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


  void add_term( std::type_index index, std::shared_ptr<Operator>&& op ); 

  using get_return_type = Hamiltonian::get_return_type<Operator>;
  get_return_type get_terms( std::type_index index ) const; 

  bool has_term( std::type_index index ) const noexcept; 


  std::unique_ptr<HamiltonianPIMPL> clone() const;

private:

  operator_container_type terms_;

};

}
