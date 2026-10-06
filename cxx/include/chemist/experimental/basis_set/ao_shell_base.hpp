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
#include <chemist/experimental/basis_set/ao_shell_base_common.hpp>
#include <memory>

namespace chemist::experimental {

/** @brief Abstract base class of the shells which own their state.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, the purity of
 *  a shell and the order it enumerates its AOs in are both part of its type.
 *  Deriving from *this, AOShell<CartesianAO> and AOShell<SphericalAO> fix the
 *  purity, and the classes deriving from those (e.g. CCAShell<AOType>) fix the
 *  ordering. *this is the layer which fixes neither, and so is what lets code
 *  which cares about neither hold any shell, e.g. a basis set whose purity is
 *  chosen at runtime. Code which needs the purity recovers it with
 *  as_cartesian_shell or as_spherical_shell.
 *
 *  *this is to AOShellBaseView what AO is to AOView; see AOView for why the
 *  owning and aliasing bases are kept separate. The shared, read-only API is in
 *  AOShellBaseCommon.
 */
class AOShellBase : public AOShellBaseCommon<AOShellBase> {
private:
    /// Type *this inherits from
    using base_type = AOShellBaseCommon<AOShellBase>;

    /// Lets the CRTP base reach the virtual methods below
    friend base_type;

public:
    /// Type of a pointer to an AOShellBase, which is how a polymorphic shell is
    /// owned
    using base_pointer = std::unique_ptr<AOShellBase>;

    /// Type of a, possibly read-only, reference to an AOShellBase
    ///@{
    using base_reference       = AOShellBase&;
    using const_base_reference = const AOShellBase&;
    ///@}

    /// Type of a pointer to a read-only view of *this
    using view_pointer = std::unique_ptr<AOShellBaseView>;

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

    /// Polymorphic, no-throw dtor
    virtual ~AOShellBase() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a deep copy of *this.
     *
     *  @return A pointer to a newly allocated copy of *this, of the same
     *          derived type.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    base_pointer clone() const { return clone_(); }

    /** @brief Returns a read-only view aliasing *this.
     *
     *  The result is the view counterpart of *this's derived type (a CCAShell
     *  gives a view of a read-only CCAShell), and aliases *this's state, so it
     *  must not outlive *this. This is how something wanting to alias a shell
     *  without knowing its ordering gets hold of one. AOShell<AOType>::as_view
     *  returns the same view, typed by its purity.
     *
     *  Defined out of line, because AOShellBaseView is incomplete here.
     *
     *  @return A pointer to a newly allocated view of *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view.
     *                        Strong throw guarantee.
     */
    view_pointer as_view() const;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two shells are value equal if they have the same derived type (i.e.
     *  the same ordering and the same purity) and their contracted Gaussians
     *  are value equal.
     *
     *  @param[in] rhs The shell to compare to *this.
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
    AOShellBase() noexcept                              = default;
    AOShellBase(const AOShellBase&) noexcept            = default;
    AOShellBase(AOShellBase&&) noexcept                 = default;
    AOShellBase& operator=(const AOShellBase&) noexcept = default;
    AOShellBase& operator=(AOShellBase&&) noexcept      = default;
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

    /// Implements as_view
    virtual view_pointer as_view_() const = 0;

    /// Implements are_equal
    virtual bool are_equal_(const_base_reference rhs) const noexcept = 0;
};

} // namespace chemist::experimental
