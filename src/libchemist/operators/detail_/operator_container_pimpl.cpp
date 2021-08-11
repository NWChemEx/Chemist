#include "operator_container_pimpl.hpp"
#include <algorithm>

namespace libchemist::operators::detail_ {

OperatorContainerPIMPL::OperatorContainerPIMPL()           = default;
OperatorContainerPIMPL::~OperatorContainerPIMPL() noexcept = default;

OperatorContainerPIMPL::OperatorContainerPIMPL(
  const OperatorContainerPIMPL& other) {
    *this = other;
}

OperatorContainerPIMPL::OperatorContainerPIMPL(
  OperatorContainerPIMPL&& other) noexcept = default;

OperatorContainerPIMPL& OperatorContainerPIMPL::operator=(
  const OperatorContainerPIMPL& other) {
    if(this == &other) return *this;

    operator_container_type buffer;
    for(const auto& [k, v] : other.terms_) buffer.insert({k, v->clone()});
    this->terms_.swap(buffer);
    return *this;
}

OperatorContainerPIMPL& OperatorContainerPIMPL::operator=(
  OperatorContainerPIMPL&& other) noexcept = default;

bool OperatorContainerPIMPL::operator==(
  const OperatorContainerPIMPL& other) const {
    // Implement comparison with polymorphic checks on operator instances
    if(terms_.size() != other.terms_.size()) return false;
    for(const auto& [key, val] : terms_) {
        auto it = other.terms_.find(key);
        if(it == other.terms_.end()) return false;       // Key not in other
        if(not it->second->is_equal(*val)) return false; // Unequal Operators
    }
    return true;
}

bool OperatorContainerPIMPL::operator!=(
  const OperatorContainerPIMPL& other) const {
    return !(*this == other);
}

void OperatorContainerPIMPL::add_term(rtti_type index, type_erased_pointer op) {
    terms_.insert({index, std::move(op)});
}

OperatorContainerPIMPL::get_return_type OperatorContainerPIMPL::get_terms(
  std::type_index index) {
    auto [b, e]               = terms_.equal_range(index);
    const std::size_t n_terms = std::distance(b, e);
    get_return_type ret_terms;
    ret_terms.reserve(n_terms);
    for(auto it = b; it != e; ++it) ret_terms.emplace_back(it->second);
    return ret_terms;
}

OperatorContainerPIMPL::get_const_return_type OperatorContainerPIMPL::get_terms(
  std::type_index index) const {
    auto [b, e]               = terms_.equal_range(index);
    const std::size_t n_terms = std::distance(b, e);
    get_const_return_type ret_terms;
    ret_terms.reserve(n_terms);
    for(auto it = b; it != e; ++it) ret_terms.emplace_back(it->second);
    return ret_terms;
}

bool OperatorContainerPIMPL::has_term(rtti_type index) const noexcept {
    return terms_.count(index);
}

std::unique_ptr<OperatorContainerPIMPL> OperatorContainerPIMPL::clone() const {
    return std::make_unique<OperatorContainerPIMPL>(*this);
}

void OperatorContainerPIMPL::hash(pluginplay::Hasher& h) const {
    for(const auto& [key, val] : terms_) { h(val); }
}

} // namespace libchemist::operators::detail_
