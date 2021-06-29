#include "derived_operator_pimpl.hpp"

namespace libchemist {

DerivedOperator::DerivedOperator() noexcept : pimpl_(nullptr){};

DerivedOperator::~DerivedOperator() noexcept = default;

DerivedOperator::DerivedOperator( const DerivedOperator& other ) {
    *this = other;
}
DerivedOperator::DerivedOperator( DerivedOperator&& other ) noexcept {
   *this = std::move(other);
}

DerivedOperator& DerivedOperator::operator=( const DerivedOperator& other ) {
    if(other.pimpl_) this->pimpl_ = other.pimpl_->clone();
    else { pimpl_.reset(); pimpl_ = nullptr; }
    return *this;
}
DerivedOperator& DerivedOperator::operator=( DerivedOperator&& other ) noexcept {
    this->pimpl_ = std::move(other.pimpl_);
    return *this;
}

void DerivedOperator::add_term_( std::type_index index, 
  std::shared_ptr<Operator>&& op ) {
    // Create PIMPL on addition of first term
    if(not pimpl_) pimpl_ = std::make_unique<detail_::DerivedOperatorPIMPL>();
    pimpl_->add_term(index,std::move(op));
}

DerivedOperator::get_return_type<Operator> DerivedOperator::get_terms_( 
  std::type_index index ) const {
    if(pimpl_) return pimpl_->get_terms(index);
    else return get_return_type<Operator>();
}

bool DerivedOperator::has_term_( std::type_index index ) const noexcept {
    if( pimpl_ ) return pimpl_->has_term(index);
    return false;
}

}
