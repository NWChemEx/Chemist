#pragma once
#include "libchemist/hamiltonian/operator.hpp"
#include "libchemist/hamiltonian/type_traits.hpp"
#include <map>
#include <algorithm>
#include <memory>

namespace libchemist {
namespace detail_ {
class HamiltonianPIMPL;
} // namespace detail_


class Hamiltonian {

public:
  /// Max number particle number for supported operators
  static constexpr std::size_t max_nbody = 2;

  /** @brief Creates a new Hamiltonian instance containing no operators.
   *
   *  This ctor can be used to create a new Hamiltonian instance which
   *  contains no operators. Operators may be added by calling `add_term`
   *  or `add_terms`.
   *
   *  @throw No throw gurantee.
   *
   *  Complexity: Constant
   */
  Hamiltonian() noexcept;

  /** @brief Creates a new Hamiltonian instances by deep copying another
   *         instance.
   *
   *  This store can be used to create a new Hamiltonian instance which
   *  contains a deep copy of @p other
   *
   *  @param[in]
   */
  Hamiltonian( const Hamiltonian& other );
  Hamiltonian( Hamiltonian&& other ) noexcept;
  ~Hamiltonian() noexcept;

  Hamiltonian& operator=( const Hamiltonian& other );
  Hamiltonian& operator=( Hamiltonian&& other ) noexcept;

  /** @brief Creates a new Hamiltonian instance from a collection of 
   *  Operator instances.
   *
   *  This ctor can be used to create a Hamiltonian instance from
   *  a collection of Operator instances. Additional terms may be
   *  added by invoking `add_term` or `add_terms`.
   *
   *  @tparam Args Parameter pack of passed Operator types, must all
   *               be derived from Operator.
   *  
   *  @param[in] args Operator instances from which to construct the Hamiltonian
   *                  instance. 
   *
   *  @throw std::bad_alloc if there is insufficient memory either to create the
   *                        PIMPL or to store the internal state of any Operator
   *                        instance. Strong exception gurantee if only a single
   *                        operator is passed, Basic exception gurantee 
   *                        otherwise.
   *
   *  Complexity: Linear in the size of @p args
   */
  template <typename... Args,
    typename = std::enable_if_t< detail_::all_are_operator_v<Args...> > 
  >
  Hamiltonian( Args&&... args ) : Hamiltonian() {
    add_terms( std::forward<Args>(args)... );
  }



  template <typename OpType>
  detail_::enable_if_operator_t< OpType, Hamiltonian& > 
    add_term( const OpType& op ) {
    add_term_<OpType::n_elec,OpType::n_nuc>( typeid(OpType), 
      std::make_shared<OpType>(op) );
    return *this;
  }

  template <typename... Ops>
  std::enable_if_t< detail_::all_are_operator_v<Ops...>, Hamiltonian& >
    add_terms( Ops&&... ops ) {
    (add_term( std::forward<Ops>(ops) ),...);
    return *this;
  }

  template <typename OpType>
  using get_return_type = typename std::vector<std::shared_ptr<OpType>>;

  template <typename OpType>
  detail_::enable_if_operator_t< OpType, get_return_type<OpType> > 
    get_terms() const {
    auto type_erased_terms = 
      get_terms_<OpType::n_elec,OpType::n_nuc>( typeid(OpType) );
    get_return_type<OpType> ret_terms( type_erased_terms.size() );
    std::transform( type_erased_terms.begin(), type_erased_terms.end(),
      ret_terms.begin(), 
      std::dynamic_pointer_cast<OpType,Operator<OpType::n_elec,OpType::n_nuc>> );
    return ret_terms;
  }

  template <typename OpType>
  detail_::enable_if_operator_t< OpType, bool > has_term() const noexcept {
    return has_term_<OpType::n_elec,OpType::n_nuc>( typeid(OpType) );
  }
private:

  template <std::size_t NE, std::size_t NN>
  void add_term_( std::type_index index, std::shared_ptr<Operator<NE,NN>>&& );

  template <std::size_t NE, std::size_t NN>
  get_return_type<Operator<NE,NN>> get_terms_( std::type_index index ) const;

  template <std::size_t NE, std::size_t NN>
  bool has_term_( std::type_index index ) const noexcept;

  std::unique_ptr<detail_::HamiltonianPIMPL> pimpl_;

};


#define EXTERN_DECL_HAM_FUNCTIONS(NMAX,NE,NN)                          \
  extern template void Hamiltonian::add_term_<NE,NN>( std::type_index, \
    std::shared_ptr<Operator<NE,NN>>&& );                              \
  extern template Hamiltonian::get_return_type<Operator<NE,NN>>        \
    Hamiltonian::get_terms_<NE,NN>( std::type_index ) const;           \
  extern template bool Hamiltonian::has_term_<NE,NN>(                  \
    std::type_index ) const noexcept;


#if 0
extern template void Hamiltonian::add_term_<1>(std::type_index,std::shared_ptr<Operator<1>>&&);
extern template void Hamiltonian::add_term_<2>(std::type_index,std::shared_ptr<Operator<2>>&&);

extern template Hamiltonian::get_return_type<Operator<1>> Hamiltonian::get_terms_<1>(std::type_index) const;
extern template Hamiltonian::get_return_type<Operator<2>> Hamiltonian::get_terms_<2>(std::type_index) const;

extern template bool Hamiltonian::has_term_<1>(std::type_index) const noexcept;
extern template bool Hamiltonian::has_term_<2>(std::type_index) const noexcept;
#else

EXTERN_DECL_HAM_FUNCTIONS(2,1,0)
EXTERN_DECL_HAM_FUNCTIONS(2,1,1)
EXTERN_DECL_HAM_FUNCTIONS(2,2,0)
EXTERN_DECL_HAM_FUNCTIONS(2,0,2)

#endif

} // namespace libchemist
