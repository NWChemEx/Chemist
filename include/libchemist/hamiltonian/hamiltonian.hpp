#pragma once
#include "libchemist/hamiltonian/operator.hpp"
#include "libchemist/hamiltonian/type_traits.hpp"
#include <map>
#include <algorithm>
#include <memory>

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

#if 0
  template <std::size_t N> const nbody_container<N>& terms() const; 
  template <std::size_t N> nbody_container<N>& terms(); 
#else
  template <typename OpType>
  using get_return_type = typename std::vector<std::shared_ptr<OpType>>;

  template <typename OpType>
  std::enable_if_t< detail_::is_operator_v<OpType>, get_return_type<OpType> > 
    get_terms() {
    auto type_erased_terms = get_terms_<OpType::n_body>( typeid(OpType).hash_code() );
    get_return_type<OpType> ret_terms( type_erased_terms.size() );
    std::transform( type_erased_terms.begin(), type_erased_terms.end(),
      ret_terms.begin(), [](auto ptr){ return std::dynamic_pointer_cast<OpType>(ptr); } );
    return ret_terms;
  }
#endif

private:

  template <std::size_t N>
  void add_term_( std::size_t hash, std::shared_ptr<Operator<N>>&& );

  template <std::size_t N>
  get_return_type<Operator<N>> get_terms_( std::size_t hash );

  std::unique_ptr<detail_::HamiltonianPIMPL> pimpl_;

};

#if 0
extern template const Hamiltonian::nbody_container<1>& Hamiltonian::terms<1>() const;
extern template const Hamiltonian::nbody_container<2>& Hamiltonian::terms<2>() const;
extern template Hamiltonian::nbody_container<1>& Hamiltonian::terms<1>();
extern template Hamiltonian::nbody_container<2>& Hamiltonian::terms<2>();
#endif

extern template void Hamiltonian::add_term_<1>(std::size_t,std::shared_ptr<Operator<1>>&&);
extern template void Hamiltonian::add_term_<2>(std::size_t,std::shared_ptr<Operator<2>>&&);

extern template Hamiltonian::get_return_type<Operator<1>> Hamiltonian::get_terms_<1>(std::size_t);
extern template Hamiltonian::get_return_type<Operator<2>> Hamiltonian::get_terms_<2>(std::size_t);

}
