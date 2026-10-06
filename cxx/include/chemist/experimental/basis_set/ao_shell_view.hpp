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
#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <chemist/experimental/basis_set/ao_shell_common.hpp>
#include <memory>

namespace chemist::experimental {

/** @brief Abstract base class of the aliasing shells of one purity.
 *
 *  *this is to AOShell<AOType> what AOShellBaseView is to AOShellBase: it
 *  answers the same questions, through the same AOShellCommon, but aliases
 *  the state rather than owning it. See AOShell for why the purity is a layer
 *  of its own. The members here are the ones whose result is typed by the
 *  owning/aliasing distinction.
 *
 *  @tparam AOType The kind of AO *this holds: CartesianAO or SphericalAO.
 */
template<typename AOType>
class AOShellView : public AOShellCommon<AOShellBaseView, AOType> {
private:
    /// Type *this inherits from
    using base_type = AOShellCommon<AOShellBaseView, AOType>;

public:
    /// Type of a pointer to an AOShellView<AOType>, which is how a
    /// polymorphic view of a shell of known purity is held
    using pointer = std::unique_ptr<AOShellView>;

    /// Type of a pointer to the owning shell *this can be materialized into.
    /// Hides AOShellBaseView::shell_pointer.
    using shell_pointer = typename AOShell<AOType>::pointer;

    /// Polymorphic, no-throw dtor. Does not affect the aliased state.
    ~AOShellView() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a new view aliasing the same state as *this.
     *
     *  This is AOShellBaseView::clone with the purity kept in the result's
     *  type. The copy is shallow.
     *
     *  @return A pointer to a newly allocated view of the same state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    pointer clone() const {
        // Every class deriving from *this clones into its own type, which is
        // an AOShellView<AOType>, so the downcast can not fail.
        return pointer(static_cast<AOShellView*>(this->clone_().release()));
    }

    /** @brief Returns a shell owning a copy of the state *this aliases.
     *
     *  This is AOShellBaseView::as_shell with the purity kept in the result's
     *  type.
     *
     *  @return A pointer to a newly allocated, owning shell.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    shell_pointer as_shell() const {
        // A view materializes into the shell it is a view of, which has the
        // same purity, so the downcast can not fail.
        return shell_pointer(
          static_cast<AOShell<AOType>*>(this->as_shell_().release()));
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AOShellView() noexcept                              = default;
    AOShellView(const AOShellView&) noexcept            = default;
    AOShellView(AOShellView&&) noexcept                 = default;
    AOShellView& operator=(const AOShellView&) noexcept = default;
    AOShellView& operator=(AOShellView&&) noexcept      = default;
    ///@}
};

/// Type of a read-only view of a Cartesian shell of any ordering
using cartesian_ao_shell_view = AOShellView<CartesianAO>;

/// Type of a read-only view of a pure shell of any ordering
using spherical_ao_shell_view = AOShellView<SphericalAO>;

/** @brief Returns @p shell as the view of a Cartesian shell it is.
 *
 *  The view counterpart of as_cartesian_shell(const AOShellBase&).
 *
 *  @param[in] shell The shell view to downcast.
 *
 *  @return @p shell, as an AOShellView<CartesianAO>. The result aliases
 *          @p shell.
 *
 *  @throw std::invalid_argument if @p shell is not a view of a Cartesian
 *                               shell. Strong throw guarantee.
 */
inline const AOShellView<CartesianAO>& as_cartesian_shell(
  const AOShellBaseView& shell) {
    return detail_::checked_shell_downcast<AOShellView<CartesianAO>>(shell);
}

/** @brief Returns @p shell as the view of a pure shell it is.
 *
 *  The view counterpart of as_spherical_shell(const AOShellBase&).
 *
 *  @param[in] shell The shell view to downcast.
 *
 *  @return @p shell, as an AOShellView<SphericalAO>. The result aliases
 *          @p shell.
 *
 *  @throw std::invalid_argument if @p shell is not a view of a pure shell.
 *                               Strong throw guarantee.
 */
inline const AOShellView<SphericalAO>& as_spherical_shell(
  const AOShellBaseView& shell) {
    return detail_::checked_shell_downcast<AOShellView<SphericalAO>>(shell);
}

extern template class AOShellView<CartesianAO>;
extern template class AOShellView<SphericalAO>;

} // namespace chemist::experimental
