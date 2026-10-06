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
#include <chemist/experimental/basis_set/ao_shell_common.hpp>
#include <memory>

namespace chemist::experimental {

/** @brief Abstract base class of the owning shells of one purity.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, the purity of
 *  a shell and the order it enumerates its AOs in are independent, and code
 *  frequently needs the former without caring about the latter. *this is the
 *  layer of the shell hierarchy which fixes the purity --- AOShell<CartesianAO>
 *  holds Cartesian AOs and AOShell<SphericalAO> holds spherical ones --- but
 *  not the ordering: the classes deriving from *this (e.g. CCAShell<AOType>)
 *  are the orderings. SphericalAO, for example, holds an AOShell<CartesianAO>,
 *  because its value is a sum over all of that shell's AOs, so it needs the
 *  shell to be Cartesian but not to be in any particular order.
 *
 *  *this derives from AOShellBase, which fixes neither; as_cartesian_shell
 *  and as_spherical_shell are the checked way from an AOShellBase to *this.
 *  The API *this adds is in AOShellCommon, which *this shares with
 *  AOShellView<AOType>; the members here are the ones whose result is typed
 *  by the owning/aliasing distinction.
 *
 *  @tparam AOType The kind of AO *this holds: CartesianAO or SphericalAO.
 */
template<typename AOType>
class AOShell : public AOShellCommon<AOShellBase, AOType> {
private:
    /// Type *this inherits from
    using base_type = AOShellCommon<AOShellBase, AOType>;

public:
    /// Type of a pointer to an AOShell<AOType>, which is how a polymorphic
    /// shell of known purity is owned
    using pointer = std::unique_ptr<AOShell>;

    /// Type of a pointer to a read-only view of *this, of the same purity.
    /// Hides AOShellBase::view_pointer.
    using view_pointer = std::unique_ptr<AOShellView<AOType>>;

    /// Polymorphic, no-throw dtor
    ~AOShell() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a deep copy of *this.
     *
     *  This is AOShellBase::clone with the purity kept in the result's type.
     *
     *  @return A pointer to a newly allocated copy of *this, of the same
     *          derived type.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    pointer clone() const {
        // Every class deriving from *this clones into its own type, which is
        // an AOShell<AOType>, so the downcast can not fail.
        return pointer(static_cast<AOShell*>(this->clone_().release()));
    }

    /** @brief Returns a read-only view aliasing *this.
     *
     *  This is AOShellBase::as_view with the purity kept in the result's type.
     *  Defined out of line, because AOShellView is incomplete here.
     *
     *  @return A pointer to a newly allocated view of *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    view_pointer as_view() const;

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AOShell() noexcept                          = default;
    AOShell(const AOShell&) noexcept            = default;
    AOShell(AOShell&&) noexcept                 = default;
    AOShell& operator=(const AOShell&) noexcept = default;
    AOShell& operator=(AOShell&&) noexcept      = default;
    ///@}
};

/// Type of an owning, Cartesian shell of any ordering
using cartesian_ao_shell = AOShell<CartesianAO>;

/// Type of an owning, pure shell of any ordering
using spherical_ao_shell = AOShell<SphericalAO>;

/** @brief Returns @p shell as the Cartesian shell it is.
 *
 *  This is the one checked step from a shell whose purity is only known at
 *  runtime to one whose purity is part of its type.
 *
 *  @param[in] shell The shell to downcast.
 *
 *  @return @p shell, as an AOShell<CartesianAO>. The result aliases
 *          @p shell.
 *
 *  @throw std::invalid_argument if @p shell is not a Cartesian shell. Strong
 *                               throw guarantee.
 */
inline const AOShell<CartesianAO>& as_cartesian_shell(
  const AOShellBase& shell) {
    return detail_::checked_shell_downcast<AOShell<CartesianAO>>(shell);
}

/** @brief Returns @p shell as the pure shell it is.
 *
 *  See as_cartesian_shell.
 *
 *  @param[in] shell The shell to downcast.
 *
 *  @return @p shell, as an AOShell<SphericalAO>. The result aliases
 *          @p shell.
 *
 *  @throw std::invalid_argument if @p shell is not a pure shell. Strong throw
 *                               guarantee.
 */
inline const AOShell<SphericalAO>& as_spherical_shell(
  const AOShellBase& shell) {
    return detail_::checked_shell_downcast<AOShell<SphericalAO>>(shell);
}

extern template class AOShell<CartesianAO>;
extern template class AOShell<SphericalAO>;

} // namespace chemist::experimental
