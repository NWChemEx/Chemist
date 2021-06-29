#include "libchemist/hamiltonian/hamiltonian.hpp"

namespace libchemist {

Hamiltonian::Hamiltonian() noexcept  = default;
Hamiltonian::~Hamiltonian() noexcept = default;

Hamiltonian::Hamiltonian( const Hamiltonian& other ) :
  DerivedOperator( other ) { }
Hamiltonian::Hamiltonian( Hamiltonian&& other ) noexcept :
  DerivedOperator( std::move(other) ) { }

Hamiltonian& Hamiltonian::operator=( const Hamiltonian& other ) {
    auto& this_as_derived_op = static_cast<DerivedOperator&>(*this);
    this_as_derived_op = DerivedOperator::operator=(other);
    return *this;
}

Hamiltonian& Hamiltonian::operator=( Hamiltonian&& other ) noexcept {
    auto& this_as_derived_op = static_cast<DerivedOperator&>(*this);
    this_as_derived_op = DerivedOperator::operator=(std::move(other));
    return *this;
}

bool Hamiltonian::is_equal_impl( const DerivedOperator& other ) const noexcept {
    auto ptr = dynamic_cast<const Hamiltonian*>(&other);
    if( !ptr ) return false;
    return *this == *ptr;
}

}
