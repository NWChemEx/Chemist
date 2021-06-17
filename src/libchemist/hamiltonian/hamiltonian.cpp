#include "hamiltonian_pimpl.hpp"

namespace libchemist {

Hamiltonian::Hamiltonian() noexcept : pimpl_(nullptr){};

Hamiltonian::~Hamiltonian() noexcept = default;

Hamiltonian::Hamiltonian( const Hamiltonian& other ) {
  *this = other;
}
Hamiltonian::Hamiltonian( Hamiltonian&& other ) noexcept {
  *this = std::move(other);
}

Hamiltonian& Hamiltonian::operator=( const Hamiltonian& other ) {
  if(other.pimpl_) this->pimpl_ = other.pimpl_->clone();
  else { pimpl_.reset(); pimpl_ = nullptr; }
  return *this;
}
Hamiltonian& Hamiltonian::operator=( Hamiltonian&& other ) noexcept {
  this->pimpl_ = std::move(other.pimpl_);
  return *this;
}

template <std::size_t N>
void Hamiltonian::add_term_( std::type_index index, std::shared_ptr<Operator<N>>&& op ) {
  // Create PIMPL on addition of first term
  if(not pimpl_) pimpl_ = std::make_unique<detail_::HamiltonianPIMPL>();
  pimpl_->add_term(index,std::move(op));
}

template <std::size_t N>
Hamiltonian::get_return_type<Operator<N>> Hamiltonian::get_terms_( std::type_index index ) const {
  if(pimpl_) return pimpl_->get_terms<N>(index);
  else return get_return_type<Operator<N>>();
}

template <std::size_t N>
bool Hamiltonian::has_term_( std::type_index index ) const noexcept {
  if( pimpl_ ) return pimpl_->has_term<N>(index);
  return false;
}



template void Hamiltonian::add_term_<1>(std::type_index,std::shared_ptr<Operator<1>>&&);
template void Hamiltonian::add_term_<2>(std::type_index,std::shared_ptr<Operator<2>>&&);

template Hamiltonian::get_return_type<Operator<1>> Hamiltonian::get_terms_<1>(std::type_index) const;
template Hamiltonian::get_return_type<Operator<2>> Hamiltonian::get_terms_<2>(std::type_index) const;

template bool Hamiltonian::has_term_<1>(std::type_index) const noexcept;
template bool Hamiltonian::has_term_<2>(std::type_index) const noexcept;

}
