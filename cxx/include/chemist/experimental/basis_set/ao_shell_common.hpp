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
#include <chemist/experimental/basis_set/ao_shell_base.hpp>
#include <chemist/experimental/basis_set/ao_shell_base_view.hpp>
#include <chemist/experimental/traits/ao_shell_traits.hpp>
#include <chemist/experimental/traits/spherical_ao_traits.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace chemist::experimental {

/** @brief Implements the API shared by AOShell<AOType> and AOShellView<AOType>.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, a shell's
 *  purity and its ordering are independent, and a great deal of code needs to
 *  know the former without caring about the latter. AOShell<AOType> and
 *  AOShellView<AOType> are the shells whose purity, but not ordering, is
 *  known statically, and *this is the API that knowledge buys them, written
 *  once:
 *
 *  - is_pure()/is_cartesian() are compile-time constants,
 *  - the angular index of the AO at an offset is available, as
 *    cartesian_powers for a Cartesian shell and magnetic_index for a pure one
 *    (each only exists for the purity it makes sense for), and
 *  - indexing returns a pointer to the concrete AO view, rather than to a
 *    polymorphic AOView.
 *
 *  How the angular index depends on the offset is the ordering, so it is the
 *  one thing *this asks the derived class for, through the ao_index_ hook.
 *  Everything else follows from the purity and from AOShellBaseCommon.
 *
 *  Unlike the other *Common classes, *this is a mixin which derives from the
 *  base it extends, rather than a CRTP sibling of it, so that a concrete shell
 *  does not inherit the same names from three unrelated bases. It is
 *  instantiated once with AOShellBase (by AOShell) and once with
 *  AOShellBaseView (by AOShellView).
 *
 *  @tparam BaseType The base *this extends: AOShellBase or AOShellBaseView.
 *  @tparam AOType The kind of AO the shell holds: CartesianAO or SphericalAO.
 */
template<typename BaseType, typename AOType>
class AOShellCommon : public BaseType {
private:
    /// Struct defining the types for the shell *this acts like
    using traits_type = ChemistClassTraits<AOShell<AOType>>;

    /// Enables a method only when the shell holds U, which must be CartesianAO
    template<typename U, typename ReturnType>
    using enable_if_cartesian_t =
      std::enable_if_t<!ChemistClassTraits<AOShell<U>>::is_pure, ReturnType>;

    /// Enables a method only when the shell holds U, which must be SphericalAO
    template<typename U, typename ReturnType>
    using enable_if_pure_t =
      std::enable_if_t<ChemistClassTraits<AOShell<U>>::is_pure, ReturnType>;

public:
    /// Type of the AOs in *this
    using ao_type = typename traits_type::ao_type;

    /// Type of a read-only view of one of the AOs in *this
    using const_ao_reference = typename traits_type::const_ao_reference;

    /// Type of a pointer to a read-only view of one of the AOs in *this
    using const_ao_pointer = typename traits_type::const_ao_pointer;

    /// Type of a pointer to a read-only view of the Cartesian shell underneath
    /// *this
    using const_cartesian_shell_pointer =
      typename traits_type::const_cartesian_shell_pointer;

    /// Type of the index picking one AO out of *this: the powers for a
    /// Cartesian shell and @f$m_\ell@f$ for a pure one
    using ao_index_type = typename traits_type::ao_index_type;

    /// Type holding the powers @f$(i,j,k)@f$ of one Cartesian AO
    using cartesian_powers_type = typename traits_type::cartesian_powers_type;

    /// Type used to model the component, @f$m_\ell@f$, of one spherical AO
    using magnetic_index_type = typename traits_type::magnetic_index_type;

    /// Pull the base's types into *this's API
    ///@{
    using typename BaseType::ao_view_pointer;
    using typename BaseType::size_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Is *this a pure (spherical) shell?
     *
     *  Known statically: AOShell<SphericalAO> is pure and AOShell<CartesianAO>
     *  is not. Hides the base's virtually dispatched version.
     *
     *  @throw None No throw guarantee.
     */
    constexpr bool is_pure() const noexcept { return traits_type::is_pure; }

    /** @brief Is *this a Cartesian shell?
     *
     *  This is a convenience for `!is_pure()`.
     *
     *  @throw None No throw guarantee.
     */
    constexpr bool is_cartesian() const noexcept { return !is_pure(); }

    /** @brief Returns the Cartesian powers of the AO at offset @p i.
     *
     *  Which powers sit at which offset is the ordering, and is supplied by
     *  the derived class. When *this is a pure shell this method does not
     *  participate in overload resolution.
     *
     *  @param[in] i The offset of the AO. Must be in [0, size()).
     *
     *  @return The powers of @f$x@f$, @f$y@f$, and @f$z@f$, in that order.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    template<typename U = AOType>
    enable_if_cartesian_t<U, cartesian_powers_type> cartesian_powers(
      size_type i) const {
        this->check_offset_(i);
        return ao_index_(i);
    }

    /** @brief Returns the component, @f$m_\ell@f$, of the AO at offset @p i.
     *
     *  Which component sits at which offset is the ordering, and is supplied
     *  by the derived class. When *this is a Cartesian shell this method does
     *  not participate in overload resolution.
     *
     *  @param[in] i The offset of the AO. Must be in [0, size()).
     *
     *  @return The component of the AO at offset @p i.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     */
    template<typename U = AOType>
    enable_if_pure_t<U, magnetic_index_type> magnetic_index(size_type i) const {
        this->check_offset_(i);
        return ao_index_(i);
    }

    /** @brief Returns the AO at offset @p i.
     *
     *  The result is a read-only view which aliases the state of *this: a
     *  CartesianAOView whose contracted Gaussian is *this's for a Cartesian
     *  shell, and a SphericalAOView of get_cartesian_shell() for a pure one.
     *  The view is built on demand and the caller owns the returned pointer,
     *  but since the view aliases *this it must not outlive *this. This hides
     *  the base's version, which returns a pointer to a polymorphic AOView.
     *
     *  Defined out of line, because SphericalAOView needs this header and so
     *  is incomplete here. For the same reason, using the result requires
     *  including cartesian_ao_view.hpp or spherical_ao_view.hpp, as the
     *  concrete shell headers (e.g. cca_shell_class.hpp) already do.
     *
     *  @param[in] i The offset of the AO. Must be in [0, size()).
     *
     *  @return A pointer to a newly allocated, read-only view of the requested
     *          AO.
     *
     *  @throw std::out_of_range if @p i is not in [0, size()). Strong throw
     *                           guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the view. Strong
     *                        throw guarantee.
     */
    const_ao_pointer at(size_type i) const;

    /// Same as at()
    const_ao_pointer operator[](size_type i) const { return at(i); }

    /** @brief Returns the Cartesian shell underneath *this.
     *
     *  For a pure shell this is the Cartesian shell its spherical AOs are
     *  linear combinations of. For a Cartesian shell it is a view of *this.
     *  Either way the result has the ordering of *this and aliases its
     *  contracted Gaussian, so it must not outlive *this.
     *
     *  @return A pointer to a newly allocated, read-only view of the Cartesian
     *          shell.
     *
     *  @throw std::bad_alloc if there is a problem allocating the view. Strong
     *                        throw guarantee.
     */
    const_cartesian_shell_pointer get_cartesian_shell() const {
        return cartesian_shell_();
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AOShellCommon() noexcept                                = default;
    AOShellCommon(const AOShellCommon&) noexcept            = default;
    AOShellCommon(AOShellCommon&&) noexcept                 = default;
    AOShellCommon& operator=(const AOShellCommon&) noexcept = default;
    AOShellCommon& operator=(AOShellCommon&&) noexcept      = default;
    ///@}

    /** @brief Hook for the ordering: the angular index of the AO at @p i.
     *
     *  @p i has already been checked. The derived class returns the powers
     *  @f$(i,j,k)@f$ for a Cartesian shell and @f$m_\ell@f$ for a pure one.
     *  This is one hook, rather than one per purity, because a virtual method
     *  can not be conditionally declared.
     */
    virtual ao_index_type ao_index_(size_type i) const = 0;

    /// Implements get_cartesian_shell
    virtual const_cartesian_shell_pointer cartesian_shell_() const = 0;

    /// Implements the base's is_pure from the purity *this knows statically
    bool is_pure_() const noexcept final { return traits_type::is_pure; }

    /// Implements the base's at in terms of *this's. Defined out of line for
    /// the same reason as at.
    ao_view_pointer at_(size_type i) const final;
};

namespace detail_ {

/** @brief Implements as_cartesian_shell and as_spherical_shell.
 *
 *  @tparam TargetType The shell type to downcast to: AOShell<AOType> or
 *                     AOShellView<AOType>.
 *  @tparam BaseType The type of @p shell: AOShellBase or AOShellBaseView.
 *
 *  @param[in] shell The shell to downcast.
 *
 *  @return @p shell, as a TargetType.
 *
 *  @throw std::invalid_argument if @p shell is not a TargetType. Strong throw
 *                               guarantee.
 */
template<typename TargetType, typename BaseType>
const TargetType& checked_shell_downcast(const BaseType& shell) {
    if(const auto* pshell = dynamic_cast<const TargetType*>(&shell))
        return *pshell;
    const bool want_pure =
      ChemistClassTraits<AOShell<typename TargetType::ao_type>>::is_pure;
    throw std::invalid_argument(
      std::string("chemist::experimental: expected a ") +
      (want_pure ? "pure" : "Cartesian") + " shell, but the shell is " +
      (shell.is_pure() ? "pure" : "Cartesian") + ".");
}

} // namespace detail_

extern template class AOShellCommon<AOShellBase, CartesianAO>;
extern template class AOShellCommon<AOShellBase, SphericalAO>;
extern template class AOShellCommon<AOShellBaseView, CartesianAO>;
extern template class AOShellCommon<AOShellBaseView, SphericalAO>;

} // namespace chemist::experimental
