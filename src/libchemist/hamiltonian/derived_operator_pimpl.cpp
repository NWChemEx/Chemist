#include "derived_operator_pimpl.hpp"
#include <algorithm>

namespace libchemist::detail_ {


DerivedOperatorPIMPL::DerivedOperatorPIMPL() = default;
DerivedOperatorPIMPL::~DerivedOperatorPIMPL() noexcept = default;

DerivedOperatorPIMPL::DerivedOperatorPIMPL( const DerivedOperatorPIMPL& other ) {
    *this = other;
}
DerivedOperatorPIMPL::DerivedOperatorPIMPL( DerivedOperatorPIMPL&& other ) noexcept {
    *this = std::move(other);
}

DerivedOperatorPIMPL& DerivedOperatorPIMPL::operator=( const DerivedOperatorPIMPL& other ) {
    this->terms_ = other.terms_;
    return *this;
}
DerivedOperatorPIMPL& DerivedOperatorPIMPL::operator=( DerivedOperatorPIMPL&& other ) noexcept {
    this->terms_ = std::move(other.terms_);
    return *this;
}


bool DerivedOperatorPIMPL::operator==( const DerivedOperatorPIMPL& other) const {
    return terms_ == other.terms_;
}

bool DerivedOperatorPIMPL::operator!=( const DerivedOperatorPIMPL& other) const {
    return !(*this == other);
}

void DerivedOperatorPIMPL::add_term( std::type_index index, 
  std::shared_ptr<Operator>&& op ) {
    terms_.insert( {index, std::move(op)} );
}

DerivedOperatorPIMPL::get_return_type DerivedOperatorPIMPL::get_terms( 
  std::type_index index ) const {
    auto [b,e] = terms_.equal_range( index );
    const std::size_t n_terms = std::distance(b,e);
    get_return_type ret_terms; ret_terms.reserve(n_terms);
    for( auto it = b; it != e; ++it )
      ret_terms.emplace_back(it->second);
    return ret_terms;
}

bool DerivedOperatorPIMPL::has_term( std::type_index index ) const noexcept {
    return terms_.count(index);
}

std::unique_ptr<DerivedOperatorPIMPL> DerivedOperatorPIMPL::clone() const {
    return std::make_unique<DerivedOperatorPIMPL>(*this);
}

}
