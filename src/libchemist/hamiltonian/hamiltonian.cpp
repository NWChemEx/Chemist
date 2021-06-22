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

void Hamiltonian::add_term_( std::type_index index, 
  std::shared_ptr<Operator>&& op ) {
    // Create PIMPL on addition of first term
    if(not pimpl_) pimpl_ = std::make_unique<detail_::HamiltonianPIMPL>();
    pimpl_->add_term(index,std::move(op));
}

Hamiltonian::get_return_type<Operator> Hamiltonian::get_terms_( 
  std::type_index index ) const {
    if(pimpl_) return pimpl_->get_terms(index);
    else return get_return_type<Operator>();
}

bool Hamiltonian::has_term_( std::type_index index ) const noexcept {
    if( pimpl_ ) return pimpl_->has_term(index);
    return false;
}

}
