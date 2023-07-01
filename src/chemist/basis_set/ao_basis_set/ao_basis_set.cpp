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

#include "ao_basis_set_pimpl.hpp"
#include "chemist/basis_set/ao_basis_set.hpp"
#include <cassert>
#include <utility>

namespace chemist {

#define AO_BS_TPARAMS template<typename AtomicBasisSetType>
#define AO_BS AOBasisSet<AtomicBasisSetType>

// -----------------------------------------------------------------------------
// -- Constructors, Assignment, dtor
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
AO_BS::AOBasisSet() noexcept = default;

AO_BS_TPARAMS
AO_BS::AOBasisSet(const AOBasisSet& rhs) :
  m_pimpl_(rhs.has_pimpl_() ? std::make_unique<pimpl_type>(*(rhs.m_pimpl_)) :
                              nullptr) {}

AO_BS_TPARAMS
AO_BS::AOBasisSet(AOBasisSet&& rhs) noexcept = default;

AO_BS_TPARAMS
AO_BS& AO_BS::operator=(const AOBasisSet& rhs) {
    if(&rhs != this) AOBasisSet(rhs).swap(*this);
    return *this;
}

AO_BS_TPARAMS
AO_BS& AO_BS::operator=(AOBasisSet&& rhs) noexcept = default;

AO_BS_TPARAMS
AO_BS::~AOBasisSet() noexcept = default;

// -----------------------------------------------------------------------------
// -- AtomicBasisSet getters/setters
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
void AO_BS::add_center(value_type center) {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    m_pimpl_->add_center(std::move(center));
}

AO_BS_TPARAMS
typename AO_BS::range_type AO_BS::shell_range(size_type center) const {
    assert_center_index_(center);
    return m_pimpl_->shell_range(center);
}

// -----------------------------------------------------------------------------
// -- Shell getters/setters
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
typename AO_BS::size_type AO_BS::max_l() const {
    if(this->empty())
        throw std::runtime_error("Basis set contains no shells!!!");
    size_type l = 0;
    for(auto&& shell_i : shells()) l = std::max(l, shell_i.l());
    return l;
}

AO_BS_TPARAMS
typename AO_BS::size_type AO_BS::n_shells() const noexcept {
    size_type counter = 0;
    for(auto&& x : *this) counter += x.size();
    return counter;
}

AO_BS_TPARAMS
typename AO_BS::shell_reference AO_BS::shell(size_type i) {
    for(auto&& x : *this) {
        if(i < x.size())
            return x[i];
        else
            i -= x.size();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_shells())");
}

AO_BS_TPARAMS
typename AO_BS::const_shell_reference AO_BS::shell(size_type i) const {
    for(auto&& x : *this) {
        if(i < x.size())
            return x[i];
        else
            i -= x.size();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_shells())");
}

AO_BS_TPARAMS
typename AO_BS::range_type AO_BS::primitive_range(size_type shell) const {
    assert_shell_index_(shell);
    return m_pimpl_->primitive_range(shell);
}

// -----------------------------------------------------------------------------
// -- AO getters/setters
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
typename AO_BS::size_type AO_BS::n_aos() const noexcept {
    size_type counter = 0;
    for(auto&& x : *this) counter += x.n_aos();
    return counter;
}

// -----------------------------------------------------------------------------
// --- Primitive getters/setters
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
typename AO_BS::size_type AO_BS::n_primitives() const noexcept {
    size_type counter = 0;
    for(auto&& x : *this) counter += x.n_unique_primitives();
    return counter;
}

AO_BS_TPARAMS
typename AO_BS::primitive_reference AO_BS::primitive(size_type i) {
    for(auto&& x : *this) {
        if(i < x.n_unique_primitives())
            return x.unique_primitive(i);
        else
            i -= x.n_unique_primitives();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_unique_primitives())");
}

AO_BS_TPARAMS
typename AO_BS::const_primitive_reference AO_BS::primitive(size_type i) const {
    for(auto&& x : *this) {
        if(i < x.n_unique_primitives())
            return x.unique_primitive(i);
        else
            i -= x.n_unique_primitives();
    }
    throw std::out_of_range("Requested i: " + std::to_string(i) +
                            " is not in the range [0, n_unique_primitives())");
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
AO_BS& AO_BS::operator+=(const AOBasisSet& rhs) {
    if(!has_pimpl()) m_pimpl_ = std::make_unique<pimpl_type>();
    for(const auto& atomic_bs : rhs) m_pimpl_.add_center(atomic_bs);
    return *this;
}

AO_BS_TPARAMS
AO_BS AO_BS::operator+(const AOBasisSet& rhs) const {
    return AOBasisSet(*this) += rhs;
}

// -----------------------------------------------------------------------------
// -- Protected and Private Fxns
// -----------------------------------------------------------------------------

AO_BS_TPARAMS
bool AO_BS::has_pimpl_() const noexcept { return static_cast<bool>(m_pimpl_); }

AO_BS_TPARAMS
void assert_center_index_(size_type center) const {
    if(center < size_()) return;
    throw std::out_of_range("Center i = " + std::to_string(center) +
                            "is not in the range [0, size()) with size() = " +
                            std::to_string(size_()));
}

AO_BS_TPARAMS
void assert_shell_index_(size_type shell) const {
    if(shell < n_shells()) return;
    throw std::out_of_range(
      "Shell i = " + std::to_string(shell) +
      "is not in the range [0, n_shells()) with n_shells() = " +
      std::to_string(n_shells()));
}

AO_BS_TPARAMS
void assert_primitive_index_(size_type primitive) const {
    if(primitive < n_primitives()) return;
    throw std::out_of_range(
      "Primitive i = " + std::to_string(primitive) +
      "is not in the range [0, n_primitives()) with n_primitives() = " +
      std::to_string(n_primitives()));
}

AO_BS_TPARAMS
typename AO_BS::size_type AO_BS::size_() const noexcept {
    if(!has_pimpl()) return 0;
    return m_pimpl_->size();
}

AO_BS_TPARAMS
typename AO_BS::reference AO_BS::at_(size_type i) { return m_pimpl_->at(i); }

AO_BS_TPARAMS
typename AO_BS::const_reference AO_BS::at_(size_type i) const {
    return m_pimpl_->at(i);
}

#undef AO_BS
#undef AO_BS_TPARAMS

template class AOBasisSet<AtomicBasisSetD>;
template class AOBasisSet<AtomicBasisSetF>;

} // namespace chemist
