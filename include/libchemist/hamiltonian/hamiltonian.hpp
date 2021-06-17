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
  detail_::enable_if_operator_t< OpType, Hamiltonian& > 
    add_term( const OpType& op ) {
    static_assert( detail_::is_density_independent_operator_v<OpType>,
      "Hamiltonian Only Takes Density Independent Operators, For Density "
      "Dependent Operators, See Fock");

    constexpr auto NBODY = OpType::n_body;
    add_term_<NBODY>( typeid(OpType).hash_code(), 
      std::make_shared<Operator<NBODY>>(op) );

    return *this;
  }

  template <typename OpType>
  using get_return_type = typename std::vector<std::shared_ptr<OpType>>;

  template <typename OpType>
  detail_::enable_if_operator_t< OpType, get_return_type<OpType> > 
    get_terms() const {
    auto type_erased_terms = 
      get_terms_<OpType::n_body>( typeid(OpType).hash_code() );
    get_return_type<OpType> ret_terms( type_erased_terms.size() );
    std::transform( type_erased_terms.begin(), type_erased_terms.end(),
      ret_terms.begin(), 
      std::dynamic_pointer_cast<OpType,Operator<OpType::n_body>> );
    return ret_terms;
  }

  template <typename OpType>
  detail_::enable_if_operator_t< OpType, bool > has_term() const noexcept {
    return has_term_<OpType::n_body>( typeid(OpType).hash_code() );
  }
private:

  template <std::size_t N>
  void add_term_( std::size_t hash, std::shared_ptr<Operator<N>>&& );

  template <std::size_t N>
  get_return_type<Operator<N>> get_terms_( std::size_t hash ) const;

  template <std::size_t N>
  bool has_term_( std::size_t hash ) const noexcept;

  std::unique_ptr<detail_::HamiltonianPIMPL> pimpl_;

};


extern template void Hamiltonian::add_term_<1>(std::size_t,std::shared_ptr<Operator<1>>&&);
extern template void Hamiltonian::add_term_<2>(std::size_t,std::shared_ptr<Operator<2>>&&);

extern template Hamiltonian::get_return_type<Operator<1>> Hamiltonian::get_terms_<1>(std::size_t) const;
extern template Hamiltonian::get_return_type<Operator<2>> Hamiltonian::get_terms_<2>(std::size_t) const;

extern template bool Hamiltonian::has_term_<1>(std::size_t) const noexcept;
extern template bool Hamiltonian::has_term_<2>(std::size_t) const noexcept;

}
