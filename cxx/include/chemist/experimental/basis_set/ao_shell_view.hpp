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

/** @brief Abstract base class of the shells which alias their state.
 *
 *  *this is to AOShell what AOView is to AO: it answers the same questions,
 *  through the same AOShellCommon, but is a separate hierarchy so that not
 *  owning the underlying state is visible in the type system. The only places
 *  the two bases differ are:
 *
 *  - clone() returns another view aliasing the same state, and
 *  - as_shell() materializes the aliased state into an owning AOShell.
 */
class AOShellView : public AOShellCommon<AOShellView> {
private:
    /// Type *this inherits from
    using base_type = AOShellCommon<AOShellView>;

    /// Lets the CRTP base reach the virtual methods below
    friend base_type;

public:
    /// Type of a pointer to an AOShellView, which is how a polymorphic view is
    /// held
    using base_pointer = std::unique_ptr<AOShellView>;

    /// Type of a, possibly read-only, reference to an AOShellView
    ///@{
    using base_reference       = AOShellView&;
    using const_base_reference = const AOShellView&;
    ///@}

    /// Type of a pointer to the owning shell *this can be materialized into
    using shell_pointer = typename AOShell::base_pointer;

    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_type;
    using typename base_type::ao_view_pointer;
    using typename base_type::center_type;
    using typename base_type::const_center_reference;
    using typename base_type::const_contracted_gaussian_reference;
    using typename base_type::contracted_gaussian_type;
    using typename base_type::coord_type;
    using typename base_type::numerical_value;
    using typename base_type::size_type;
    ///@}

    /// Polymorphic, no-throw dtor. Does not affect the aliased state.
    virtual ~AOShellView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a new view aliasing the same state as *this.
     *
     *  The copy is shallow; see AOView::clone.
     *
     *  @return A pointer to a newly allocated view of the same state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Returns a shell owning a copy of the state *this aliases.
     *
     *  The result has the same ordering and purity as *this --- a view of a
     *  CCAShell<SphericalAO> materializes into a CCAShell<SphericalAO> --- and
     *  is independent of whatever *this aliases.
     *
     *  Defined out of line, for symmetry with AOShell::as_view.
     *
     *  @return A pointer to a newly allocated, owning shell.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    shell_pointer as_shell() const;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two views are value equal if they are views of shells with the same
     *  ordering and purity and the state they alias is value equal. Whether
     * they alias the *same* state is not considered.
     *
     *  @param[in] rhs The view to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const_base_reference rhs) const noexcept {
        return are_equal_(rhs);
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See are_equal for
     *  the definition of value equal.
     */
    bool are_different(const_base_reference rhs) const noexcept {
        return !are_equal(rhs);
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

    /// Implements get_l
    virtual angular_momentum_type get_l_() const noexcept = 0;

    /// Implements get_center
    virtual const_center_reference get_center_() const = 0;

    /// Implements get_contracted_gaussian
    virtual const_contracted_gaussian_reference get_contracted_gaussian_()
      const = 0;

    /// Implements is_pure
    virtual bool is_pure_() const noexcept = 0;

    /// Implements at. The offset has already been checked.
    virtual ao_view_pointer at_(size_type i) const = 0;

    /// Implements clone
    virtual base_pointer clone_() const = 0;

    /// Implements as_shell
    virtual shell_pointer as_shell_() const = 0;

    /// Implements are_equal
    virtual bool are_equal_(const_base_reference rhs) const noexcept = 0;
};

} // namespace chemist::experimental
