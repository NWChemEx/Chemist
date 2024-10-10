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

#pragma once
#include <chemist/basis_set/ao_basis_set.hpp>
#include <optional>

namespace chemist::basis_set::detail_ {

template<typename AtomicBasisSetType>
class AOBasisSetPIMPL {
private:
    /// The type of the AOBasisSet implemented by this PIMPL
    using bs_type = AOBasisSet<AtomicBasisSetType>;

public:
    /// Unsigned integer type used for indexing/offsets
    using size_type = typename bs_type::size_type;

    // -------------------------------------------------------------------------
    // --  AtomicBasisSet Types
    // -------------------------------------------------------------------------

    /// The objects comprising the Basis set
    using value_type = typename bs_type::value_type;

    /// Read-/write-able references to elements in the basis
    using reference = typename bs_type::reference;

    /// Read-only references to elements in the basis
    using const_reference = typename bs_type::const_reference;

    using abs_traits = AtomicBasisSetTraits<reference>;

    /** @brief Adds a AtomicBasisSet to the current basis set.
     *
     *  @param[in] c The center to add to this basis set.
     *
     *  @throw std::bad_alloc if the underlying vector needs to reallocate and
     *                        there is insufficient memory. Strong throw
     *                        guarantee.
     */
    void add_atomic_basis_set(const_reference c) {
        m_shells_per_center_.push_back(c.size());
        m_shell_offsets_.push_back(n_shells());

        m_names_.push_back(c.basis_set_name());
        m_atomic_numbers_.push_back(c.atomic_number());

        m_x_.push_back(c.center().x());
        m_y_.push_back(c.center().y());
        m_z_.push_back(c.center().z());

        for(const auto& shell_i : c) add_shell(shell_i);
    }

    /** @brief Returns the number of centers in this basis set.
     *
     *  @return The number of centers in this basis set.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_shells_per_center_.size(); }

    size_type n_shells() const noexcept { return m_pure_.size(); }

    size_type n_primitives() const noexcept { return m_coefs_.size(); }

    auto shell_range(size_type center) const {
        size_type begin = m_shell_offsets_[center];
        size_type end   = begin + m_shells_per_center_[center];
        return typename abs_traits::range_type{begin, end};
    }

    auto primitive_range(size_type shell) const {
        size_type begin = m_primitive_offset_[shell];
        size_type end   = begin + m_primitives_per_shell_[shell];
        return typename abs_traits::range_type{begin, end};
    }

    size_type shell_to_center(size_type shell) const {
        for(size_type i = 0; i < size(); ++i) {
            auto [shell_begin, shell_end] = shell_range(i);
            if(shell >= shell_begin && shell < shell_end) return i;
        }
        throw std::runtime_error("Non-existent shell requested.");
    }

    size_type primitive_to_center(size_type primitive) const {
        return shell_to_center(primitive_to_shell(primitive));
    }

    size_type primitive_to_shell(size_type primitive) const {
        for(size_type i = 0; i < n_shells(); ++i) {
            auto [p_begin, p_end] = primitive_range(i);
            if(primitive >= p_begin && primitive < p_end) return i;
        }
        throw std::runtime_error("Non-existent primitive requested.");
    }

    /** @brief Returns the @p i-th center in the basis set.
     *
     *  At the moment this function performs no bounds checks because it is
     *  assumed that it will be used to implement IndexableContainerBase's
     *  operator[] (which will perform the bounds check).
     *
     *  @param[in] i The index of the requested center. @p i must be in the
     *               range [0, size()).
     *  @return A read-/write-able reference to the requested center.
     *
     *  @throw None No throw guarantee.
     */
    auto at(size_type i) {
        using reference_type = typename abs_traits::shell_reference;
        std::vector<reference_type> shells;
        shells.reserve(m_shells_per_center_[i]);

        auto [shell_begin, shell_end] = shell_range(i);
        for(auto shell_i = shell_begin; shell_i < shell_end; ++shell_i) {
            shells.emplace_back(std::move(shell(shell_i)));
        }

        return reference(m_names_[i], m_atomic_numbers_[i], center(i),
                         std::move(shells));
    }

    /** @brief Returns the @p i-th center in the basis set.
     *
     *  At the moment this function performs no bounds checks because it is
     *  assumed that it will be used to implement IndexableContainerBase's
     *  operator[] (which will perform the bounds check).
     *
     *  @param[in] i The index of the requested center. @p i must be in the
     *               range [0, size()).
     *  @return A read-only reference to the requested center.
     *
     *  @throw None No throw guarantee.
     */
    auto at(size_type i) const {
        using reference_type = typename abs_traits::const_shell_reference;
        std::vector<reference_type> shells;
        shells.reserve(m_shells_per_center_[i]);

        auto [shell_begin, shell_end] = shell_range(i);
        for(auto shell_i = shell_begin; shell_i < shell_end; ++shell_i) {
            shells.emplace_back(std::move(shell(shell_i)));
        }

        return const_reference(m_names_[i], m_atomic_numbers_[i], center(i),
                               std::move(shells));
    }

    auto shell(size_type i) {
        using shell_reference = typename abs_traits::shell_reference;
        return shell_reference(m_pure_[i], m_l_[i], cg(i));
    }

    auto shell(size_type i) const {
        using shell_reference = typename abs_traits::const_shell_reference;
        return shell_reference(m_pure_[i], m_l_[i], cg(i));
    }

    auto cg(size_type i) {
        using cg_reference = typename abs_traits::cg_reference;
        auto center_i      = center(shell_to_center(i));
        auto p_off         = m_primitive_offset_[i];
        return cg_reference(m_primitives_per_shell_[i], m_coefs_[p_off],
                            m_exps_[p_off], center_i);
    }

    auto cg(size_type i) const {
        using cg_reference = typename abs_traits::const_cg_reference;
        auto center_i      = center(shell_to_center(i));
        auto p_off         = m_primitive_offset_[i];
        return cg_reference(m_primitives_per_shell_[i], m_coefs_[p_off],
                            m_exps_[p_off], center_i);
    }

    auto primitive(size_type i) {
        using primitive_reference = typename abs_traits::primitive_reference;
        auto center_i             = center(primitive_to_center(i));
        return primitive_reference(m_coefs_[i], m_exps_[i], center_i);
    }

    auto primitive(size_type i) const {
        using primitive_reference =
          typename abs_traits::const_primitive_reference;
        auto center_i = center(primitive_to_center(i));
        return primitive_reference(m_coefs_[i], m_exps_[i], center_i);
    }

    auto center(size_type i) {
        using center_reference = typename abs_traits::center_reference;
        return center_reference(m_x_[i], m_y_[i], m_z_[i]);
    }

    auto center(size_type i) const {
        using const_center_reference =
          typename abs_traits::const_center_reference;
        return const_center_reference(m_x_[i], m_y_[i], m_z_[i]);
    }

    auto max_l() const {
        typename abs_traits::angular_momentum_type max = 0;
        for(auto l : m_l_) max = std::max(max, l);
        return max;
    }

private:
    /** @brief Adds shells to the last center*/
    void add_shell(typename abs_traits::const_shell_reference s) {
        m_pure_.push_back(s.pure());
        m_l_.push_back(s.l());

        const auto prims_in_shell = s.n_primitives();

        m_primitives_per_shell_.push_back(prims_in_shell);
        m_primitive_offset_.push_back(n_primitives());
        for(size_type i = 0; i < prims_in_shell; ++i) {
            add_primitive(s.primitive(i));
        }
    }

    /** @brief Adds primitives to the last shell. */
    void add_primitive(typename abs_traits::const_primitive_reference p) {
        m_coefs_.push_back(p.coefficient());
        m_exps_.push_back(p.exponent());
    }

    // -- Unpacked AtomicBasisSet State

    std::vector<size_type> m_shells_per_center_;

    std::vector<size_type> m_shell_offsets_;

    std::vector<typename abs_traits::name_type> m_names_;

    std::vector<typename abs_traits::atomic_number_type> m_atomic_numbers_;

    std::vector<typename abs_traits::coord_type> m_x_;

    std::vector<typename abs_traits::coord_type> m_y_;

    std::vector<typename abs_traits::coord_type> m_z_;

    // -- Unpacked Shell state

    std::vector<typename abs_traits::pure_type> m_pure_;

    std::vector<typename abs_traits::angular_momentum_type> m_l_;

    std::vector<size_type> m_primitives_per_shell_;

    std::vector<size_type> m_primitive_offset_;

    //-- Unpacked primitive state

    std::vector<typename abs_traits::coefficient_type> m_coefs_;

    std::vector<typename abs_traits::exponent_type> m_exps_;
}; // class AOBasisSetPIMPL

} // namespace chemist::basis_set::detail_