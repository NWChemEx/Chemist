/*
 * Copyright 2026 NWChemEx-Project
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
#include <chemist/experimental/basis_set/ao_shell_view.hpp>
#include <chemist/experimental/basis_set/detail_/ao_view_impl.hpp>
#include <chemist/experimental/basis_set/spherical_ao_class.hpp>
#include <memory>
#include <type_traits>
#include <utility>

namespace chemist::experimental {

/** @brief An object which behaves like a SphericalAO, but aliases the
 *         Cartesian shell it is built on.
 *
 *  *this is the spherical counterpart of CartesianAOView. Per
 *  docs/source/developer/design/basis_set/ao_hierarchy.rst, it is what a
 *  spherical shell will hand out when indexed: every spherical AO in such a
 *  shell is built on the one Cartesian shell it owns, and handing out views
 *  of it, rather than @f$2\ell+1@f$ copies, is what keeps them from drifting.
 *
 *  The Cartesian shell is aliased through a polymorphic AOShellView, so *this
 *  can alias a shell of any ordering. @f$m_\ell@f$ is stored by value, for the
 *  same reason CartesianAOView stores its powers by value.
 *
 *  @tparam SAOType A const-qualified SphericalAO. Only read-only views are
 *                  supported for now: the polymorphic shell API is read-only,
 *                  so a mutable view would have nothing to write through. The
 *                  parameter is kept so that *this has the same shape as
 *                  CartesianAOView, and so that a mutable view can be added
 *                  without changing the spelling of the read-only one.
 */
template<typename SAOType>
class SphericalAOView
  : public SphericalAOCommon<SphericalAOView<SAOType>, SAOType>,
    public detail_::AOViewImpl<SphericalAOView<SAOType>> {
    static_assert(std::is_const_v<SAOType>,
                  "Only views of a read-only SphericalAO are supported.");

private:
    /// Type implementing the API shared with SphericalAO
    using common_type = SphericalAOCommon<SphericalAOView<SAOType>, SAOType>;

    /// Type implementing the AOView interface in terms of *this
    using impl_type = detail_::AOViewImpl<SphericalAOView<SAOType>>;

    /// Lets the CRTP base reach shell_() and m_()
    friend common_type;

    /// Lets the AOView implementation reach as_ao_value_()
    friend impl_type;

    /// Type of the ordering-agnostic Cartesian shell *this can alias
    using shell_type = typename ChemistClassTraits<SAOType>::shell_type;

    /// Type of the polymorphic view *this aliases its shell through
    using shell_view_type =
      typename ChemistClassTraits<SAOType>::shell_view_type;

    /// Type of the pointer *this holds its shell view through
    using shell_view_pointer = typename shell_view_type::base_pointer;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename common_type::angular_momentum_type;
    using typename common_type::center_type;
    using typename common_type::const_center_reference;
    using typename common_type::const_contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_type;
    using typename common_type::coord_type;
    using typename common_type::magnetic_index_type;
    using typename common_type::numerical_value;
    using typename common_type::numerical_vector;
    using typename common_type::size_type;
    using typename common_type::value_type;
    ///@}

    /// Pull the AOView interface's types into *this's API
    ///@{
    using typename AOView::ao_pointer;
    using typename AOView::base_pointer;
    using typename AOView::const_base_reference;
    ///@}

    /// Type of the owning spherical AO *this materializes into. AOViewImpl
    /// needs this to implement as_ao().
    using ao_type = typename ChemistClassTraits<SAOType>::value_type;

    /// Type of a read-only reference to the shell view *this holds
    using const_shell_reference = const shell_view_type&;

    /** @brief Resolves the shared API against the AOView interface.
     *
     *  See the corresponding block in CartesianAO.
     */
    ///@{
    using common_type::evaluate;
    using common_type::get_center;
    using common_type::get_contracted_gaussian;
    using common_type::get_l;
    using common_type::normalization_constant;
    using common_type::normalized_evaluate;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view of @p sao.
     *
     *  This ctor is deliberately implicit so that a SphericalAO can be passed
     *  wherever a SphericalAOView is expected. The result aliases @p sao's
     *  Cartesian shell, so it must not outlive @p sao.
     *
     *  @param[in] sao The spherical AO *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the shell view.
     *                        Strong throw guarantee.
     */
    SphericalAOView(const ao_type& sao) :
      m_shell_(sao.get_cartesian_shell().as_view()), m_m_(sao.get_m()) {}

    /** @brief Creates the component @p m of @p shell, aliasing @p shell.
     *
     *  @param[in] shell The Cartesian shell *this will alias.
     *  @param[in] m The component. Must satisfy @f$|m| \le \ell@f$.
     *
     *  @throw std::invalid_argument if @p shell is pure or @p m is out of
     *                               range. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the shell view.
     *                        Strong throw guarantee.
     */
    SphericalAOView(const shell_type& shell, magnetic_index_type m) :
      m_shell_((common_type::check_shell(shell, m), shell.as_view())),
      m_m_(m) {}

    /** @brief Creates the component @p m of the shell @p shell aliases.
     *
     *  This is what lets *this alias a shell which is itself only aliased,
     *  e.g. a view of the Cartesian shell owned by a spherical shell.
     *
     *  @param[in] shell A view of the Cartesian shell *this will alias.
     *  @param[in] m The component. Must satisfy @f$|m| \le \ell@f$.
     *
     *  @throw std::invalid_argument if @p shell is pure or @p m is out of
     *                               range. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the shell view.
     *                        Strong throw guarantee.
     */
    SphericalAOView(const shell_view_type& shell, magnetic_index_type m) :
      m_shell_((common_type::check_shell(shell, m), shell.clone())), m_m_(m) {}

    /** @brief Creates a view aliasing the same shell as @p other.
     *
     *  The copy is shallow: cloning a shell view gives another view of the
     *  same shell.
     *
     *  @throw std::bad_alloc if there is a problem allocating the shell view.
     *                        Strong throw guarantee.
     */
    SphericalAOView(const SphericalAOView& other) :
      m_shell_(other.m_shell_->clone()), m_m_(other.m_m_) {}

    /** @brief Takes the aliased state from @p other.
     *
     *  @note @p other is left without a shell, and may only be destroyed.
     *
     *  @throw None No throw guarantee.
     */
    SphericalAOView(SphericalAOView&& other) noexcept = default;

    /// A read-only view can not be assigned through. Use swap to rebind.
    ///@{
    SphericalAOView& operator=(const SphericalAOView&) = delete;
    SphericalAOView& operator=(SphericalAOView&&)      = delete;
    ///@}

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~SphericalAOView() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the view of the Cartesian shell *this is built on.
     *
     *  @throw None No throw guarantee.
     */
    const_shell_reference get_cartesian_shell() const noexcept {
        return *m_shell_;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a SphericalAO holding a copy of the aliased state.
     *
     *  The copy's shell has the same ordering as the aliased one.
     *
     *  @return An owning spherical AO, independent of what *this aliases.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    ao_type as_spherical_ao() const {
        return ao_type(*m_shell_->as_shell(), m_m_);
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  @throw None No throw guarantee.
     */
    void swap(SphericalAOView& other) noexcept {
        m_shell_.swap(other.m_shell_);
        std::swap(m_m_, other.m_m_);
    }

private:
    /// Implements AOViewImpl::as_ao_
    ao_type as_ao_value_() const { return as_spherical_ao(); }

    /// Implements the CRTP base's state access
    ///@{
    const_shell_reference shell_() const noexcept { return *m_shell_; }
    magnetic_index_type m_() const noexcept { return m_m_; }
    ///@}

    /** @brief Implements the AOView interface in terms of the shared API.
     *
     *  clone_, as_ao_, and are_equal_ are not here; AOViewImpl implements
     *  those from *this's copy ctor, as_ao_value_(), and operator==.
     */
    ///@{
    typename AOView::angular_momentum_type get_l_() const noexcept override {
        return common_type::get_l();
    }
    typename AOView::const_center_reference get_center_() const override {
        return common_type::get_center();
    }
    typename AOView::const_contracted_gaussian_reference
    get_contracted_gaussian_() const override {
        return common_type::get_contracted_gaussian();
    }
    typename AOView::numerical_value normalization_constant_() const override {
        return common_type::normalization_constant();
    }
    typename AOView::numerical_value evaluate_(
      typename AOView::const_point_reference r) const override {
        return common_type::evaluate(r);
    }
    typename AOView::numerical_value normalized_evaluate_(
      typename AOView::const_point_reference r) const override {
        return common_type::normalized_evaluate(r);
    }
    ///@}

    /// Aliases the Cartesian shell whose AOs *this is a linear combination of
    shell_view_pointer m_shell_;

    /// The component, @f$m_\ell@f$, of *this. Owned, not aliased.
    magnetic_index_type m_m_ = 0;
};

/// Type of a view of a read-only SphericalAO
using const_spherical_ao_view = SphericalAOView<const SphericalAO>;

extern template class SphericalAOView<const SphericalAO>;

} // namespace chemist::experimental
