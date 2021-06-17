#include "hamiltonian_pimpl.hpp"

namespace libchemist {

Hamiltonian::Hamiltonian() :
  pimpl_(std::make_unique<detail_::HamiltonianPIMPL>()){};

Hamiltonian::~Hamiltonian() noexcept = default;

Hamiltonian::Hamiltonian( const Hamiltonian& other ) {
  *this = other;
}
Hamiltonian::Hamiltonian( Hamiltonian&& other ) noexcept {
  *this = std::move(other);
}

Hamiltonian& Hamiltonian::operator=( const Hamiltonian& other ) {
  this->pimpl_ = other.pimpl_->clone();
  return *this;
}
Hamiltonian& Hamiltonian::operator=( Hamiltonian&& other ) noexcept {
  this->pimpl_ = std::move(other.pimpl_);
  return *this;
}

#if 0
template <std::size_t N>
const Hamiltonian::nbody_container<N>& Hamiltonian::terms() const {
  return pimpl_->terms<N>();
}
template <std::size_t N>
Hamiltonian::nbody_container<N>& Hamiltonian::terms() {
  return pimpl_->terms<N>();
}
#endif

template <std::size_t N>
void Hamiltonian::add_term_( std::size_t hash, std::shared_ptr<Operator<N>>&& op ) {
  pimpl_->add_term(hash,std::move(op));
}

template <std::size_t N>
Hamiltonian::get_return_type<Operator<N>> Hamiltonian::get_terms_( std::size_t hash ) {
  return pimpl_->get_terms<N>(hash);
}



#if 0
template const Hamiltonian::nbody_container<1>& Hamiltonian::terms<1>() const;
template const Hamiltonian::nbody_container<2>& Hamiltonian::terms<2>() const;
template Hamiltonian::nbody_container<1>& Hamiltonian::terms<1>();
template Hamiltonian::nbody_container<2>& Hamiltonian::terms<2>();
#endif

template void Hamiltonian::add_term_<1>(std::size_t,std::shared_ptr<Operator<1>>&&);
template void Hamiltonian::add_term_<2>(std::size_t,std::shared_ptr<Operator<2>>&&);

template Hamiltonian::get_return_type<Operator<1>> Hamiltonian::get_terms_<1>(std::size_t);
template Hamiltonian::get_return_type<Operator<2>> Hamiltonian::get_terms_<2>(std::size_t);

}
