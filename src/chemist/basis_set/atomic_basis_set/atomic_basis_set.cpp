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
#include "atomic_basis_set_pimpl.hpp"
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set.hpp>

namespace chemist {

#define ATOMIC_BASIS_SET AtomicBasisSet<ShellType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet() noexcept = default;

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(const AtomicBasisSet& rhs) :
  AtomicBasisSet(
    rhs.has_pimpl_() ? std::make_unique<pipml_type>(*rhs.m_pimpl_) : nullptr) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(AtomicBasisSet&& rhs) noexcept = default;

template<typename ShellType>
ATOMIC_BASIS_SET& ATOMIC_BASIS_SET::operator=(const AtomicBasisSet& rhs) {
    if(&rhs != this) AtomicBasisSet(rhs).swap(*this);
    return *this;
}

template<typename ShellType>
ATOMIC_BASIS_SET& ATOMIC_BASIS_SET::operator=(AtomicBasisSet&& rhs) noexcept =
  default;

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(const_name_reference name,
                                 atomic_number_type atomic_n, coord_type x,
                                 coord_type y, coord_type z) :
  AtomicBasisSet(std::make_unique<pimpl_pointer>(center_type(x, y, z))) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(coord_type x, coord_type y, coord_type z) :
  AtomicBasisSet(std::make_unique<pimpl_pointer>(center_type(x, y, z))) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(const_name_reference name,
                                 atomic_number_type atomic_n) :
  AtomicBasisSet(name, atomic_n, 0.0, 0.0, 0.0) {}

template<typename ShellType>
ATOMIC_BASIS_SET::~AtomicBasisSet() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

template<typename ShellType>
bool ATOMIC_BASIS_SET::has_name() const noexcept {
    if(is_null()) return false;
    return m_pimpl_->m_name.has_value();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::name_reference ATOMIC_BASIS_SET::basis_set_name() {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>(0.0, 0.0, 0.0);
    if(!m_pimpl_->m_name.has_value()) m_pimpl_->m_name.emplace("");
    return m_pimpl_->m_name.value();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_name_reference
ATOMIC_BASIS_SET::basis_set_name() const {
    if(has_name()) return m_pimpl_->m_name.value();
    throw std::runtime_error("The current AtomicBasisSet does not have a name");
}

template<typename ShellType>
bool ATOMIC_BASIS_SET::has_atomic_number() const noexcept {
    if(is_null()) return false;
    return m_pimpl_->m_z.has_value();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::atomic_number_reference
ATOMIC_BASIS_SET::atomic_number() {
    if(is_null()) return m_pimpl_ = std::make_unique<pimpl_type>(0.0, 0.0, 0.0);
    if(!m_pimpl_->m_z.has_value()) m_pimpl_->m_z.emplace(0);
    return m_pimpl_->m_z.value();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_atomic_number_reference
ATOMIC_BASIS_SET::atomic_number() const {
    if(has_atomic_number()) return m_pimpl_->m_z.value());
    throw std::runtime_error("The current AtomicBasisSet does not have an "
                             "atomic number");
}

template<typename ShellType>
void ATOMIC_BASIS_SET::add_shell(pure_type pure, angular_momentum_type l,
                                 coefficient_vector cs, exponent_vector es) {
    assert(m_pimpl_ != nullptr);
    m_pimpl_->add_shell(pure, l, std::move(cs), std::move(es));
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::size_type ATOMIC_BASIS_SET::n_aos() const noexcept {
    size_type counter = 0;
    for(auto&& shell_i : *this) counter += shell_i.size();
    return counter;
}

// Disabled until AO class is written
// template<typename ShellType>
// typename ATOMIC_BASIS_SET::ao_reference ATOMIC_BASIS_SET::ao(size_type i) {
//     for(ShellView<T> shell_i : *this) {
//         if(i < shell_i.size())
//             return shell_i[i];
//         else
//             i -= shell_i.size();
//     }
//     throw std::out_of_range("Requested i: " + std::to_string(i) +
//                             " is not in the range [0, naos())");
// }

// template<typename ShellType>
// typename ATOMIC_BASIS_SET::const_ao_reference ATOMIC_BASIS_SET::ao(
//   size_type i) const {
//     for(auto&& shell_i : *this) {
//         if(i < shell_i.size())
//             return shell_i[i];
//         else
//             i -= shell_i.size();
//     }
//     throw std::out_of_range("Requested i: " + std::to_string(i) +
//                             " is not in the range [0, naos())");
// }

template<typename ShellType>
typename ATOMIC_BASIS_SET::size_type ATOMIC_BASIS_SET::n_primitives()
  const noexcept {
    size_type counter = 0;
    for(auto&& shell_i : *this) counter += shell_i.n_primitives();
    return counter;
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::primitive_reference ATOMIC_BASIS_SET::primitive(
  size_type i) {
    if(i >= n_primitives()) {
        throw std::out_of_range("Requested i: " + std::to_string(i) +
                                " is not in the range [0, n_primitives())");
    }
    return m_pimpl_->primitive(i);
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_primitive_reference
ATOMIC_BASIS_SET::primitive(size_type i) const {
    if(i >= n_primitives()) {
        throw std::out_of_range("Requested i: " + std::to_string(i) +
                                " is not in the range [0, n_primitives())");
    }
    return m_pimpl_->primitive(i);
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

template<typename ShellType>
void ATOMIC_BASIS_SET::swap(AtomicBasisSet& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

template<typename ShellType>
bool ATOMIC_BASIS_SET::is_null() const noexcept {
    return !has_pimpl_();
}

template<typename ShellType>
bool ATOMIC_BASIS_SET::operator==(const AtomicBasisSet& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are null
    return *m_pimpl_ == *rhs.m_pimpl_;
}

// -----------------------------------------------------------------------------
// -- Protected and private members
// -----------------------------------------------------------------------------
template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

template<typename ShellType>
typename ATOMIC_BASIS_SET::size_type ATOMIC_BASIS_SET::size_() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->size();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::reference ATOMIC_BASIS_SET::at_(size_type i) {
    assert(m_pimpl_ != nullptr);
    auto ptr1 = m_pimpl_->at(i);
    auto ptr2 = this->point_alias();
    Shell<T> shell(std::move(ptr1), std::move(ptr2));
    return reference(std::move(shell));
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_reference ATOMIC_BASIS_SET::at_(
  size_type i) const {
    assert(m_pimpl_ != nullptr);
    auto ptr1 = m_pimpl_->at(i);
    auto ptr2 = this->point_alias();
    Shell<T> shell(std::move(ptr1), std::move(ptr2));
    return const_reference(std::move(shell));
}

template class AtomicBasisSet<double>;
template class AtomicBasisSet<float>;

} // namespace chemist
