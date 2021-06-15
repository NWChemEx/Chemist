#include "hamiltonian_pimpl.hpp"

namespace libchemist::detail_ {


HamiltonianPIMPL::HamiltonianPIMPL() = default;
HamiltonianPIMPL::~HamiltonianPIMPL() noexcept = default;

HamiltonianPIMPL::HamiltonianPIMPL( const HamiltonianPIMPL& other ) {
  *this = other;
}
HamiltonianPIMPL::HamiltonianPIMPL( HamiltonianPIMPL&& other ) noexcept {
  *this = std::move(other);
}

HamiltonianPIMPL& HamiltonianPIMPL::operator=( const HamiltonianPIMPL& other ) {
  this->terms_ = other.terms_;
  return *this;
}
HamiltonianPIMPL& HamiltonianPIMPL::operator=( HamiltonianPIMPL&& other ) noexcept {
  this->terms_ = std::move(other.terms_);
  return *this;
}


std::unique_ptr<HamiltonianPIMPL> HamiltonianPIMPL::clone() const {
  return std::make_unique<HamiltonianPIMPL>(*this);
}

}
