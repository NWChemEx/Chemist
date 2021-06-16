#pragma once
#include "libchemist/hamiltonian/operator.hpp"
#include "libchemist/hamiltonian/type_traits.hpp"
#include <map>

namespace libchemist {

namespace detail_ {
class HamiltonianPIMPL;
}


class Hamiltonian {

public:

  static constexpr std::size_t max_nbody = 2;

private:


  template <typename OpType>
  using term_container_type = 
    std::multimap< std::size_t, std::shared_ptr<OpType> >;

  template <std::size_t N>
  using nbody_container = term_container_type<Operator<N>>;

public:


  template <typename... Args,
    typename = std::enable_if_t< detail_::all_are_operator_v<Args...> > 
  >
  Hamiltonian( Args&&... args ) : Hamiltonian() {
    (add_term(args),...);
  }

  Hamiltonian();
  Hamiltonian( const Hamiltonian& other );
  Hamiltonian( Hamiltonian&& other ) noexcept;
  ~Hamiltonian() noexcept;

  Hamiltonian& operator=( const Hamiltonian& other );
  Hamiltonian& operator=( Hamiltonian&& other ) noexcept;



  template <typename OpType>
  std::enable_if_t<detail_::is_operator_v<OpType>,Hamiltonian&> 
    add_term( const OpType& op ) {
    static_assert( detail_::is_density_independent_operator_v<OpType>,
      "Hamiltonian Only Takes Density Independent Operators, For Density "
      "Dependent Operators, See Fock");

    constexpr auto NBODY = OpType::n_body;
    add_term_<NBODY>( typeid(OpType).hash_code(), 
      std::make_shared<Operator<NBODY>>(op) );

    return *this;
  }

  template <std::size_t N> const nbody_container<N>& terms() const; 
  template <std::size_t N> nbody_container<N>& terms(); 

private:

  template <std::size_t N>
  void add_term_( std::size_t hash, std::shared_ptr<Operator<N>>&& );

  std::unique_ptr<detail_::HamiltonianPIMPL> pimpl_;

};

extern template const Hamiltonian::nbody_container<1>& Hamiltonian::terms<1>() const;
extern template const Hamiltonian::nbody_container<2>& Hamiltonian::terms<2>() const;
extern template Hamiltonian::nbody_container<1>& Hamiltonian::terms<1>();
extern template Hamiltonian::nbody_container<2>& Hamiltonian::terms<2>();

extern template void Hamiltonian::add_term_<1>(std::size_t,std::shared_ptr<Operator<1>>&&);
extern template void Hamiltonian::add_term_<2>(std::size_t,std::shared_ptr<Operator<2>>&&);

}
