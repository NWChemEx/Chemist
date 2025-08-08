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
#include "compute_n_aos.hpp"
#include <chemist/basis_set/atomic_basis_set_traits.hpp>
#include <chemist/basis_set/atomic_basis_set_view.hpp>
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
      typename abs_traits::name_reference name,
      typename abs_traits::atomic_number_reference z,
      typename abs_traits::center_reference center,
      std::vector<typename abs_traits::shell_reference> shells);

    typename abs_traits::name_reference basis_set_name() { return m_name_; }

    typename abs_traits::const_name_reference basis_set_name() const {
        return m_name_;
    }

    typename abs_traits::atomic_number_reference atomic_number() {
        return m_z_;
    }

    typename abs_traits::const_atomic_number_reference atomic_number() const {
        return m_z_;
    }

    typename abs_traits::center_reference center() { return m_center_; }

    typename abs_traits::const_center_reference center() const {
        return m_center_;
    }

    typename abs_traits::pure_reference pure(size_type shell_i) {
        return m_shells_[shell_i].pure();
    }

    typename abs_traits::const_pure_reference pure(size_type shell_i) const {
        return m_shells_[shell_i].pure();
    }

    typename abs_traits::angular_momentum_reference l(size_type shell_i) {
        return m_shells_[shell_i].l();
    }

    typename abs_traits::const_angular_momentum_reference l(
      size_type shell_i) const {
        return m_shells_[shell_i].l();
    }

    size_type size() const noexcept { return m_shells_.size(); }

    size_type n_aos() const noexcept {
        size_type counter = 0;
        for(size_type i = 0; i < size(); ++i) counter += m_shells_[i].size();
        return counter;
    }

    size_type n_primitives() const noexcept { return m_cg_offsets_.back(); }

    typename abs_traits::range_type primitive_range(size_type shell) const {
        auto begin = m_cg_offsets_[shell];
        auto end   = m_cg_offsets_[shell + 1];
        return typename abs_traits::range_type(begin, end);
    }

    size_type primitive_to_shell(size_type primitive) const {
        for(size_type shell = 0; shell < size(); ++shell) {
            auto r = primitive_range(shell);
            if(primitive >= r.first && primitive < r.second) return shell;
        }
        // Shouldn't be able to get here b/c parent checked primitive index and
        // it was in bounds.
        throw std::runtime_error("Non-existent primitive requested");
    }

    auto operator[](size_type i) { return m_shells_[i]; }

    auto operator[](size_type i) const { return m_shells_[i]; }

    auto cg(size_type i) { return m_shells_[i].contracted_gaussian(); }

    auto cg(size_type i) const { return m_shells_[i].contracted_gaussian(); }

    auto primitive(size_type i) {
        auto sh_i = primitive_to_shell(i);
        return m_shells_[sh_i].contracted_gaussian()[i - m_cg_offsets_[sh_i]];
    }

    auto primitive(size_type i) const {
        auto sh_i = primitive_to_shell(i);
        return m_shells_[sh_i].contracted_gaussian()[i - m_cg_offsets_[sh_i]];
    }

    bool operator==(const AtomicBasisSetViewPIMPL& rhs) const noexcept {
        if(basis_set_name() != rhs.basis_set_name()) return false;
        if(atomic_number() != rhs.atomic_number()) return false;
        if(center() != rhs.center()) return false;
        if(m_shells_ != rhs.m_shells_) return false;
        return true;
    }

private:
    /// The name of the basis set (if set)
    typename abs_traits::name_reference m_name_;

    /// The atomic number of the basis set (if set)
    typename abs_traits::atomic_number_reference m_z_;

    /// Where *this is centered (if set)
    typename abs_traits::center_reference m_center_;

    /// The offsets for primitives in a shell
    std::vector<size_type> m_cg_offsets_ = {0};

    /// The elements of the atomic basis set this references
    std::vector<typename abs_traits::shell_reference> m_shells_;
};

template<typename AtomicBasisSetType>
AtomicBasisSetViewPIMPL<AtomicBasisSetType>::AtomicBasisSetViewPIMPL(
  typename abs_traits::name_reference name,
  typename abs_traits::atomic_number_reference z,
  typename abs_traits::center_reference center,
  std::vector<typename abs_traits::shell_reference> shells) :
  m_name_(name),
  m_z_(z),
  m_center_(std::move(center)),
  m_shells_(std::move(shells)) {
    for(size_type i = 0; i < m_shells_.size(); ++i) {
        m_cg_offsets_.push_back(m_cg_offsets_[i] +
                                m_shells_[i].contracted_gaussian().size());
    }
}

} // namespace chemist::basis_set::detail_
