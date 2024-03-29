/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "linear_combination_pimpl.hpp"
#include <algorithm>

namespace chemist::operators::detail_ {

LinearCombinationPIMPL::LinearCombinationPIMPL()           = default;
LinearCombinationPIMPL::~LinearCombinationPIMPL() noexcept = default;

LinearCombinationPIMPL::LinearCombinationPIMPL(
  const LinearCombinationPIMPL& other) {
    *this = other;
}

LinearCombinationPIMPL::LinearCombinationPIMPL(
  LinearCombinationPIMPL&& other) noexcept = default;

LinearCombinationPIMPL& LinearCombinationPIMPL::operator=(
  const LinearCombinationPIMPL& other) {
    if(this == &other) return *this;

    operator_container_type buffer;
    for(const auto& [k, v] : other.terms_) buffer.insert({k, v->clone()});
    this->terms_.swap(buffer);
    return *this;
}

LinearCombinationPIMPL& LinearCombinationPIMPL::operator=(
  LinearCombinationPIMPL&& other) noexcept = default;

bool LinearCombinationPIMPL::operator==(
  const LinearCombinationPIMPL& other) const {
    // Implement comparison with polymorphic checks on operator instances
    if(terms_.size() != other.terms_.size()) return false;
    for(const auto& [key, val] : terms_) {
        auto it = other.terms_.find(key);
        if(it == other.terms_.end()) return false;       // Key not in other
        if(not it->second->is_equal(*val)) return false; // Unequal Operators
    }
    return true;
}

bool LinearCombinationPIMPL::operator!=(
  const LinearCombinationPIMPL& other) const {
    return !(*this == other);
}

void LinearCombinationPIMPL::add_term(rtti_type index, type_erased_pointer op) {
    terms_.insert({index, std::move(op)});
}

LinearCombinationPIMPL::get_return_type LinearCombinationPIMPL::get_terms(
  std::type_index index) {
    auto [b, e]               = terms_.equal_range(index);
    const std::size_t n_terms = std::distance(b, e);
    get_return_type ret_terms;
    ret_terms.reserve(n_terms);
    for(auto it = b; it != e; ++it) ret_terms.emplace_back(it->second);
    return ret_terms;
}

LinearCombinationPIMPL::get_const_return_type LinearCombinationPIMPL::get_terms(
  std::type_index index) const {
    auto [b, e]               = terms_.equal_range(index);
    const std::size_t n_terms = std::distance(b, e);
    get_const_return_type ret_terms;
    ret_terms.reserve(n_terms);
    for(auto it = b; it != e; ++it) ret_terms.emplace_back(it->second);
    return ret_terms;
}

bool LinearCombinationPIMPL::has_term(rtti_type index) const noexcept {
    return terms_.count(index);
}

std::unique_ptr<LinearCombinationPIMPL> LinearCombinationPIMPL::clone() const {
    return std::make_unique<LinearCombinationPIMPL>(*this);
}

} // namespace chemist::operators::detail_
