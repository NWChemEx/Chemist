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

    explicit AtomicBasisSetViewPIMPL(
      std::vector<typename abs_traits::const_shell_reference> shells);

    explicit AtomicBasisSetViewPIMPL(
      typename abs_traits::name_reference name,
      typename abs_traits::atomic_number_reference z,
      typename abs_traits::center_reference center,
      std::vector<typename abs_traits::const_shell_reference> shells);

    bool has_name() const noexcept { return m_name_.has_value(); }

    typename abs_traits::name_reference name() { return m_name_.value(); }

    typename abs_traits::const_name_reference name() const {
        return *m_name_.value();
    }

    bool has_atomic_number() const noexcept { return m_z_.has_value(); }

    typename abs_traits::atomic_number_reference atomic_number() {
        return *m_z_.value();
    }

    typename abs_traits::const_atomic_number_reference atomic_number() const {
        return *m_z_.value();
    }

    bool has_center() const noexcept { return m_center_.has_value(); }

    typename abs_traits::center_reference center() {
        return *m_center_.value();
    }

    typename abs_traits::const_center_reference center() const {
        return *m_center_.value();
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
        for(size_type i = 0; i < size(); ++i) counter += m_shells_.size();
        return counter;
    }

    size_type n_primitives() const noexcept {
        size_type counter = 0;
        for(size_type i = 0; i < size(); ++i)
            counter += m_shells_.n_primitives();
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

    auto operator[](size_type i) { return m_shells_[i]; }

    auto operator[](size_type i) const { return m_shells_[i]; }

    auto cg(size_type i) { return m_shells_[i].contracted_gaussian(); }

    auto cg(size_type i) const { return m_shells_[i].contracted_gaussian(); }

    auto primitive(size_type i) {
        auto sh_i = primitive_to_shell(i);
        return m_shells_[sh_i].contracted_gaussian[i - m_cg_offsets_[sh_i]];
    }

    auto primitive(size_type i) const {
        auto sh_i = primitive_to_shell(i);
        return m_shells_[sh_i].contracted_gaussian[i - m_cg_offsets_[sh_i]];
    }

    bool operator==(const AtomicBasisSetViewPIMPL& rhs) const noexcept {
        if(has_name() != rhs.has_name()) return false;
        if(has_name() && name() != rhs.name()) return false;
        if(has_atomic_number() != rhs.has_atomic_number()) return false;
        if(has_atomic_number() && atomic_number() != rhs.atomic_number())
            return false;
        if(has_center() != rhs.has_center()) return false;
        if(has_center() && center() != rhs.center()) return false;
        if(m_shells_ != rhs.m_shells_) return false;
        return true;
    }

private:
    /// The name of the basis set (if set)
    std::optional<typename abs_traits::name_type*> m_name_;

    /// The atomic number of the basis set (if set)
    std::optional<typename abs_traits::atomic_number_type*> m_z_;

    /// Where *this is centered (if set)
    std::optional<typename abs_traits::center_reference*> m_center_;

    /// The offsets for primitives in a shell
    std::vector<size_type> m_cg_offsets_;

    /// The elements of the atomic basis set this references
    std::vector<typename abs_traits::const_shell_reference> m_shells_;
};

template<typename AtomicBasisSetType>
AtomicBasisSetViewPIMPL<AtomicBasisSetType>::AtomicBasisSetViewPIMPL(
  std::vector<typename abs_traits::const_shell_reference> shells) :
  m_cg_offsets_(shells.size(), 0), m_shells_(std::move(shells)) {
    for(size_type i = 1; i < m_shells_.size(); ++i) {
        m_cg_offsets_[i] =
          m_cg_offsets_[i - 1] + m_shells_[i].contracted_gaussian().size();
    }
}

template<typename AtomicBasisSetType>
AtomicBasisSetViewPIMPL<AtomicBasisSetType>::AtomicBasisSetViewPIMPL(
  typename abs_traits::name_reference name,
  typename abs_traits::atomic_number_reference z,
  typename abs_traits::center_reference center,
  std::vector<typename abs_traits::const_shell_reference> shells) :
  m_name_(&name),
  m_z_(&z),
  m_center_(&center),
  m_cg_offsets_(shells.size(), 0),
  m_shells_(std::move(shells)) {
    for(size_type i = 1; i < m_shells_.size(); ++i) {
        m_cg_offsets_[i] =
          m_cg_offsets_[i - 1] + m_shells_[i].contracted_gaussian().size();
    }
}

} // namespace chemist::basis_set::detail_