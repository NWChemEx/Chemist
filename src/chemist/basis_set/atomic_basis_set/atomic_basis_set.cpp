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
#include "detail_/atomic_basis_set_pimpl.hpp"
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set.hpp>
#include <utility>

namespace chemist::basis_set {

#define ATOMIC_BASIS_SET AtomicBasisSet<ShellType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet() noexcept = default;

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(const AtomicBasisSet& rhs) :
  AtomicBasisSet(
    rhs.has_pimpl_() ? std::make_unique<pimpl_type>(*rhs.m_pimpl_) : nullptr) {}

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
ATOMIC_BASIS_SET::AtomicBasisSet(name_type name, atomic_number_type atomic_n,
                                 typename shell_traits::coord_type x,
                                 typename shell_traits::coord_type y,
                                 typename shell_traits::coord_type z) :
  AtomicBasisSet(std::move(name), std::move(atomic_n),
                 std::move(typename shell_traits::center_type(x, y, z))) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(name_type name, atomic_number_type atomic_n,
                                 typename shell_traits::center_type center) :
  AtomicBasisSet(std::make_unique<pimpl_type>(
    std::move(name), std::move(atomic_n), std::move(center))) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(typename shell_traits::coord_type x,
                                 typename shell_traits::coord_type y,
                                 typename shell_traits::coord_type z) :
  AtomicBasisSet(std::move(typename shell_traits::center_type(x, y, z))) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(typename shell_traits::center_type center) :
  AtomicBasisSet(std::make_unique<pimpl_type>(std::move(center))) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(name_type name, atomic_number_type atomic_n) :
  AtomicBasisSet(std::move(name), std::move(atomic_n), 0.0, 0.0, 0.0) {}

template<typename ShellType>
ATOMIC_BASIS_SET::~AtomicBasisSet() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

template<typename ShellType>
typename ATOMIC_BASIS_SET::name_reference ATOMIC_BASIS_SET::basis_set_name() {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    return m_pimpl_->basis_set_name();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_name_reference
ATOMIC_BASIS_SET::basis_set_name() const {
    if(!is_null()) return m_pimpl_->basis_set_name();
    throw std::runtime_error("The current AtomicBasisSet does not have a name");
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::atomic_number_reference
ATOMIC_BASIS_SET::atomic_number() {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    return m_pimpl_->atomic_number();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_atomic_number_reference
ATOMIC_BASIS_SET::atomic_number() const {
    if(!is_null()) return m_pimpl_->atomic_number();
    throw std::runtime_error("The current AtomicBasisSet does not have an "
                             "atomic number");
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::shell_traits::center_reference
ATOMIC_BASIS_SET::center() {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    return m_pimpl_->center();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::shell_traits::const_center_reference
ATOMIC_BASIS_SET::center() const {
    if(!is_null()) return m_pimpl_->center();
    throw std::runtime_error("The current AtomicBasisSet does not have a "
                             "center set.");
}

template<typename ShellType>
void ATOMIC_BASIS_SET::add_shell(typename shell_traits::pure_type pure,
                                 typename shell_traits::angular_momentum_type l,
                                 typename shell_traits::cg_reference cg) {
    if(is_null()) m_pimpl_ = std::make_unique<pimpl_type>();
    m_pimpl_->add_shell(pure, l, std::move(cg));
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::size_type ATOMIC_BASIS_SET::n_aos() const noexcept {
    size_type counter = 0;
    for(size_type i = 0; i < size_(); ++i)
        counter += detail_::compute_n_aos(m_pimpl_->l(i), m_pimpl_->pure(i));
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
typename ATOMIC_BASIS_SET::range_type ATOMIC_BASIS_SET::primitive_range(
  size_type shell) const {
    if(shell >= size_()) {
        throw std::out_of_range("Requested i: " + std::to_string(shell) +
                                " is not in the range [0, size())");
    }
    return m_pimpl_->primitive_range(shell);
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::size_type ATOMIC_BASIS_SET::primitive_to_shell(
  size_type primitive) const {
    if(primitive >= n_primitives()) {
        throw std::out_of_range("Requested i: " + std::to_string(primitive) +
                                " is not in the range [0, n_primitives())");
    }
    return m_pimpl_->prim2shell(primitive);
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::shell_traits::primitive_reference
ATOMIC_BASIS_SET::primitive(size_type i) {
    if(i >= n_primitives()) {
        throw std::out_of_range("Requested i: " + std::to_string(i) +
                                " is not in the range [0, n_primitives())");
    }
    return m_pimpl_->primitive(i);
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::shell_traits::const_primitive_reference
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
ATOMIC_BASIS_SET::AtomicBasisSet(typename shell_traits::center_type center,
                                 std::vector<const_reference> shell_views) :
  AtomicBasisSet(std::make_unique<pimpl_type>(center, shell_views)) {}

template<typename ShellType>
ATOMIC_BASIS_SET::AtomicBasisSet(const_name_reference name,
                                 atomic_number_type atomic_n,
                                 typename shell_traits::center_type center,
                                 std::vector<const_reference> shell_views) :
  AtomicBasisSet(
    std::make_unique<pimpl_type>(name, atomic_n, center, shell_views)) {}

template<typename ShellType>
bool ATOMIC_BASIS_SET::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::size_type ATOMIC_BASIS_SET::size_() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->size();
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::reference ATOMIC_BASIS_SET::at_(size_type i) {
    return (*m_pimpl_)[i];
}

template<typename ShellType>
typename ATOMIC_BASIS_SET::const_reference ATOMIC_BASIS_SET::at_(
  size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

#undef ATOMIC_BASIS_SET

template class AtomicBasisSet<ShellD>;
template class AtomicBasisSet<ShellF>;

} // namespace chemist::basis_set