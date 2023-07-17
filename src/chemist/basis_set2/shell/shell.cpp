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

#include "../detail_/compute_n_aos.hpp"
#include "detail_/shell_pimpl.hpp"

namespace chemist::basis_set {

#define SHELL Shell<CGType>

template<typename CGType, typename... Args>
auto make_shell_pimpl(Args&&... args) {
    using pimpl_type = typename Shell<CGType>::pimpl_type;
    return std::make_unique<pimpl_type>(std::forward<Args>(args)...);
}

// -----------------------------------------------------------------------------
// -- Ctors/dtor
// -----------------------------------------------------------------------------

template<typename CGType>
SHELL::Shell() noexcept = default;

template<typename CGType>
SHELL::Shell(const Shell& rhs) :
  Shell(rhs.has_pimpl_() ? make_shell_pimpl<CGType>(*rhs.m_pimpl_) : nullptr) {}

template<typename CGType>
SHELL::Shell(Shell&& rhs) noexcept = default;

template<typename CGType>
SHELL& SHELL::operator=(const Shell& rhs) {
    if(&rhs != this) Shell(rhs).swap(*this);
    return *this;
}

template<typename CGType>
SHELL& SHELL::operator=(Shell&& rhs) noexcept = default;

template<typename CGType>
SHELL::Shell(pure_type pure, angular_momentum_type l, value_type cg) :
  m_pimpl_(make_shell_pimpl<CGType>(pure, l, std::move(cg))) {}

template<typename CGType>
SHELL::~Shell() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters/setters
// -----------------------------------------------------------------------------

template<typename CGType>
typename SHELL::pure_reference SHELL::pure() {
    if(is_null()) m_pimpl_ = make_shell_pimpl<CGType>();
    return m_pimpl_->m_pure;
}

template<typename CGType>
typename SHELL::const_pure_reference SHELL::pure() const {
    assert_pimpl_();
    return m_pimpl_->m_pure;
}

template<typename CGType>
typename SHELL::angular_momentum_reference SHELL::l() {
    if(is_null()) m_pimpl_ = make_shell_pimpl<CGType>();
    return m_pimpl_->m_l;
}

template<typename CGType>
typename SHELL::const_angular_momentum_reference SHELL::l() const {
    assert_pimpl_();
    return m_pimpl_->m_l;
}

template<typename CGType>
typename SHELL::reference SHELL::contracted_gaussian() {
    if(is_null()) m_pimpl_ = make_shell_pimpl<CGType>();
    return m_pimpl_->m_cg;
}

template<typename CGType>
typename SHELL::const_reference SHELL::contracted_gaussian() const {
    assert_pimpl_();
    return m_pimpl_->m_cg;
}

template<typename CGType>
typename SHELL::size_type SHELL::n_primitives() const noexcept {
    if(is_null()) return 0;
    return m_pimpl_->m_cg.size();
}

template<typename CGType>
typename SHELL::cg_traits::primitive_reference SHELL::primitive(size_type i) {
    return contracted_gaussian().at(i);
}

template<typename CGType>
typename SHELL::cg_traits::const_primitive_reference SHELL::primitive(
  size_type i) const {
    if(i < n_primitives()) return contracted_gaussian().at(i);
    throw std::out_of_range("i is not in the range [0, n_primitives()).");
}

template<typename CGType>
typename SHELL::size_type SHELL::size() const noexcept {
    return size_();
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

template<typename CGType>
void SHELL::swap(Shell& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

template<typename CGType>
bool SHELL::is_null() const noexcept {
    return !has_pimpl_();
}

template<typename CGType>
bool SHELL::operator==(const Shell& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are null
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

// -----------------------------------------------------------------------------
// -- Protected/Private functions
// -----------------------------------------------------------------------------

template<typename CGType>
SHELL::Shell(pimpl_ptr my_pimpl) noexcept : m_pimpl_(std::move(my_pimpl)) {}

template<typename CGType>
bool SHELL::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

template<typename CGType>
void SHELL::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("Shell has no PIMPL. Was it default constructed or"
                             " moved from?");
}

template<typename CGType>
typename SHELL::size_type SHELL::size_() const noexcept {
    if(is_null()) return 0;
    return detail_::compute_n_aos(l(), pure());
}

// template<typename CGType>
// typename SHELL::contracted_gaussian_reference SHELL::at_(size_type index) {
//     assert(m_pimpl_ != nullptr);
//     auto ptr1 = m_pimpl_->at(index);
//     auto ptr2 = this->point_alias();
//     ContractedGaussian<T> temp(std::move(ptr1), std::move(ptr2));
//     return reference(std::move(temp));
// }

// template<typename CGType>
// typename SHELL::const_cg_reference SHELL::at_(size_type index) const {
//     assert(m_pimpl_ != nullptr);
//     auto ptr1 = m_pimpl_->at(index);
//     auto ptr2 = this->point_alias();
//     ContractedGaussian<T> temp(std::move(ptr1), std::move(ptr2));
//     return const_reference(std::move(temp));
// }

#undef SHELL

template class Shell<ContractedGaussianD>;
template class Shell<ContractedGaussianF>;

} // namespace chemist::basis_set