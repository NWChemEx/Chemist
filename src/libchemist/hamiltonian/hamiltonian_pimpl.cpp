#include "hamiltonian_pimpl.hpp"
#include <algorithm>

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

void HamiltonianPIMPL::add_term( std::type_index index, 
  std::shared_ptr<Operator>&& op ) {
    terms_.insert( {index, std::move(op)} );
}

HamiltonianPIMPL::get_return_type HamiltonianPIMPL::get_terms( 
  std::type_index index ) const {
    auto [b,e] = terms_.equal_range( index );
    const std::size_t n_terms = std::distance(b,e);
    get_return_type ret_terms; ret_terms.reserve(n_terms);
    for( auto it = b; it != e; ++it )
      ret_terms.emplace_back(it->second);
    return ret_terms;
}

bool HamiltonianPIMPL::has_term( std::type_index index ) const noexcept {
    return terms_.count(index);
}

std::unique_ptr<HamiltonianPIMPL> HamiltonianPIMPL::clone() const {
    return std::make_unique<HamiltonianPIMPL>(*this);
}

}
