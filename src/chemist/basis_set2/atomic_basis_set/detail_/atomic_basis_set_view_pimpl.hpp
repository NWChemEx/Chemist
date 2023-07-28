/*
 * Copyright 2023 NWChemEx-Project
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

#pragma once
#include "../../detail_/compute_n_aos.hpp"
#include <chemist/basis_set2/atomic_basis_set/atomic_basis_set_traits.hpp>
#include <chemist/basis_set2/atomic_basis_set/atomic_basis_set_view.hpp>
#include <optional>
#include <utility>

namespace chemist::basis_set::detail_ {

template<typename AtomicBasisSetType>
class AtomicBasisSetViewPIMPL {
public:
    /// The type *this is implementing
    using parent_type = AtomicBasisSetView<AtomicBasisSetType>;

    using abs_traits = AtomicBasisSetTraits<parent_type>;

    using size_type = typename parent_type::size_type;

    AtomicBasisSetViewPIMPL() = delete;

    AtomicBasisSetViewPIMPL(
      size_type nshells, typename abs_traits::pure_reference pure_per_shell,
      typename abs_traits::angular_momentum_reference l_per_shell,
      typename parent_type::const_size_pointer prims_per_shell,
      typename abs_traits::coefficient_reference coefs,
      typename abs_traits::exponent_pointer exps,
      typename abs_traits::center_reference center);

    AtomicBasisSetViewPIMPL(
      typename abs_traits::name_reference name,
      typename abs_traits::atomic_number_reference z, size_type nshells,
      typename abs_traits::pure_reference pure_per_shell,
      typename abs_traits::angular_momentum_reference l_per_shell,
      typename parent_type::const_size_pointer prims_per_shell,
      typename abs_traits::coefficient_reference coefs,
      typename abs_traits::exponent_pointer exps,
      typename abs_traits::center_reference center);

    bool has_name() const noexcept { return m_name_.has_value(); }

    typename abs_traits::name_reference name() { return m_name_.value().get(); }

    typename abs_traits::const_name_reference name() const {
        return m_name_.value().get();
    }

    bool has_atomic_number() const noexcept { return m_z_.has_value(); }

    typename abs_traits::atomic_number_reference atomic_number() {
        return m_z_.value().get();
    }

    typename abs_traits::const_atomic_number_reference atomic_number() const {
        return m_z_.value().get();
    }

    bool has_center() const noexcept { return m_center_.has_value(); }

    typename abs_traits::center_reference center() {
        return m_center_.value().get();
    }

    typename abs_traits::const_center_reference center() const {
        return m_center_.value().get();
    }

    typename abs_traits::pure_reference pure(size_type shell_i) {
        return (&m_pure_)[shell_i];
    }

    typename abs_traits::const_pure_reference pure(size_type shell_i) const {
        return (&m_pure_)[shell_i];
    }

    typename abs_traits::angular_momentum_reference l(size_type shell_i) {
        return (&m_l_)[shell_i];
    }

    typename abs_traits::const_angular_momentum_reference l(
      size_type shell_i) const {
        return (&m_l_)[shell_i];
    }

    typename abs_traits::coefficient_reference coef(size_type i) {
        return (&m_coefs_)[i];
    }

    typename abs_traits::const_coefficient_reference coef(size_type i) const {
        return (&m_coefs_)[i];
    }

    typename abs_traits::exponent_reference exponent(size_type i) {
        return (&m_exps_)[i];
    }

    typename abs_traits::const_exponent_reference exponent(size_type i) const {
        return (&m_exps_)[i];
    }

    size_type size() const noexcept { return m_n_shells_; }

    size_type n_aos() const noexcept {
        size_type counter = 0;
        for(size_type i = 0; i < size(); ++i)
            counter += detail_::compute_n_aos(l(i), pure(i));
        return counter;
    }

    size_type n_primitives() const noexcept {
        size_type counter = 0;
        for(size_type i = 0; i < size(); ++i) counter += m_prims_per_shell_[i];
        return counter;
    }

    typename abs_traits::range_type primitive_range(size_type shell) const {
        auto begin = m_cg_offsets_[shell];
        auto end   = (shell + 1 < size()) ? m_cg_offsets_[shell + 1] : size();
        return range_type(begin, end);
    }

    size_type primitive_to_shell(size_type primitive) const {
        for(size_type i = 0; i < size(); ++i) {
            auto r = primitive_range(primitive);
            if(primitive >= r.first && primitive < r.second) return i;
        }
        // Shouldn't be able to get here b/c parent checked primitive index and
        // it was in bounds.
    }

    auto operator[](size_type i) {
        return typename parent_type::reference(pure(i), l(i), cg(i));
    }

    auto operator[](size_type i) const {
        return typename parent_type::const_reference(pure(i), l(i), cg(i));
    }

    auto cg(size_type i) {
        return typename abs_traits::contracted_gaussian_reference(
          m_prims_per_shell_[i], coef(m_cg_offsets_[i]),
          exponent(m_cg_offsets_[i]), center());
    }

    auto cg(size_type i) const {
        return typename abs_traits::const_cg_reference(
          m_prims_per_shell_[i], coef(m_cg_offsets_[i]),
          exponent(m_cg_offsets_[i]), center());
    }

    auto primitive(size_type i) {
        return abs_traits::primitive_reference(coef(i), exponent(i), center());
    }

    auto primitive(size_type i) const {
        return abs_traits::const_primitive_reference(coef(i), exponent(i),
                                                     center());
    }

    bool operator==(const AtomicBasisSetViewPIMPL& rhs) const noexcept {
        if(has_name() != rhs.has_name()) return false;
        if(has_name() && name() != rhs.name()) return false;
        if(has_atomic_number() != rhs.has_atomic_number()) return false;
        if(has_atomic_number() && atomic_number() != rhs.atomic_number())
            return false;
        if(m_n_shells_ != rhs.m_n_shells_) return false;
        for(size_type i = 0; i < m_n_shells_; ++i) {
            if((*this)[i] != rhs[i]) return false;
        }
        return true;
    }

private:
    using wrapped_name_type =
      std::reference_wrapper<typename abs_traits::name_reference>;

    using wrapped_z_type =
      std::reference_wrapper<typename abs_traits::atomic_number_reference>;

    using wrapped_center_type =
      std::reference_wrapper<typename abs_traits::center_reference>;

    /// The name of the basis set (if set)
    std::optional<wrapped_name_type> m_name_ = nullptr;

    /// The atomic number of the basis set (if set)
    std::optional<wrapped_z_type> m_z_ = nullptr;

    /// Where *this is centered (if set)
    std::optional<wrapped_center_type> m_center_ = nullptr;

    /// The number of shells in *this
    size_type m_n_shells_;

    /// The number of primitives per shell
    typename parent_type::const_size_pointer m_prims_per_shell_;

    /// The offsets for primitives in a shell
    std::vector<size_type> m_cg_offsets_;

    /// The coefficients of the primitives
    typename abs_traits::coefficient_reference m_coefs_;

    /// The exponents of the primitives
    typename abs_traits::exponent_reference m_exps_;

    /// The purity of the shells
    typename abs_traits::pure_reference m_pure_;

    /// The angular momentum of the shells
    typename abs_traits::angular_momentum_reference m_l_;
};

template<typename AtomicBasisSetType>
AtomicBasisSetViewPIMPL<AtomicBasisSetType>::AtomicBasisSetViewPIMPL(
  typename abs_traits::name_reference name,
  typename abs_traits::atomic_number_reference z, size_type nshells,
  typename abs_traits::pure_reference pure_per_shell,
  typename abs_traits::angular_momentum_reference l_per_shell,
  typename parent_type::const_size_pointer prims_per_shell,
  typename abs_traits::coefficient_reference coefs,
  typename abs_traits::exponent_pointer exps,
  typename abs_traits::center_reference center) :
  m_name_(name),
  m_z_(z),
  m_n_shells_(nshells),
  m_prims_per_shell_(prims_per_shell),
  m_cg_offsets_(nshells),
  m_coefs_(coefs),
  m_exps_(exps),
  m_center_(std::move(center)) {
    for(size_type i = 1; i < nshells; ++i) {
        m_cg_offsets_[i] = m_cg_offsets_[i - 1] + m_prims_per_shell_[i];
    }
}

template<typename AtomicBasisSetType>
AtomicBasisSetViewPIMPL<AtomicBasisSetType>::AtomicBasisSetViewPIMPL(
  size_type nshells, typename abs_traits::pure_reference pure_per_shell,
  typename abs_traits::angular_momentum_reference l_per_shell,
  typename parent_type::const_size_pointer prims_per_shell,
  typename abs_traits::coefficient_reference coefs,
  typename abs_traits::exponent_pointer exps,
  typename abs_traits::center_reference center) :
  m_n_shells_(nshells),
  m_prims_per_shell_(prims_per_shell),
  m_cg_offsets_(nshells),
  m_coefs_(coefs),
  m_exps_(exps),
  m_center_(std::move(center)) {
    for(size_type i = 1; i < nshells; ++i) {
        m_cg_offsets_[i] = m_cg_offsets_[i - 1] + m_prims_per_shell_[i];
    }
}

} // namespace chemist::basis_set::detail_