#include "libchemist/hamiltonian/fock_operator.hpp"

namespace libchemist {

FockOperator::FockOperator() noexcept  = default;
FockOperator::~FockOperator() noexcept = default;

FockOperator::FockOperator( const FockOperator& other ) :
  DerivedOperator( other ) { }
FockOperator::FockOperator( FockOperator&& other ) noexcept :
  DerivedOperator( std::move(other) ) { }

FockOperator& FockOperator::operator=( const FockOperator& other ) {
    auto& this_as_derived_op = static_cast<DerivedOperator&>(*this);
    this_as_derived_op = DerivedOperator::operator=(other);
    return *this;
}

FockOperator& FockOperator::operator=( FockOperator&& other ) noexcept {
    auto& this_as_derived_op = static_cast<DerivedOperator&>(*this);
    this_as_derived_op = DerivedOperator::operator=(std::move(other));
    return *this;
}

}
