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
#include <chemist/experimental/basis_set/ao_view.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian_view.hpp>
#include <chemist/experimental/traits/ao_shell_traits.hpp>
#include <cstddef>
#include <stdexcept>
#include <string>

namespace chemist::experimental {

/** @brief Implements the API shared by AOShellBase and AOShellBaseView.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, a shell is a
 *  set of AOs sharing one contracted Gaussian, a total angular momentum, and a
 *  purity, and both the purity and the order in which it enumerates those AOs
 *  are part of its type: AOShell<AOType> fixes the purity and the classes
 *  deriving from it (e.g. CCAShell) *are* the orderings. *this is the API
 *  which needs neither, written once.
 *
 *  The purity decides what the shell holds: the @f$(\ell+1)(\ell+2)/2@f$
 *  Cartesian AOs built on the contracted Gaussian, or the @f$2\ell+1@f$
 *  spherical AOs built on the Cartesian shell of that contracted Gaussian.
 *
 *  Note what is NOT here: the angular index at an offset (the powers
 *  @f$(i,j,k)@f$ of a Cartesian shell, @f$m_\ell@f$ of a pure one). Which of
 *  the two a shell has depends on its purity, so it is provided by
 *  AOShell<AOType>, which knows the purity statically (e.g.
 *  AOShell<CartesianAO>::cartesian_powers), rather than here, where asking the
 *  wrong one could only be a runtime error. Code holding an AOShellBase can
 *  still ask each AO it indexes about itself, or recover the purity with
 *  as_cartesian_shell/as_spherical_shell.
 *
 *  Exactly as AOCommon does for AO and AOView, *this implements each public
 *  method by calling a virtual hook on DerivedType (which is AOShellBase or
 *  AOShellBaseView, not the concrete class). Methods which follow from other
 *  methods, e.g. size() from get_l() and is_pure(), are computed here rather
 *  than being hooks, so the derived classes can not disagree about them.
 *
 *  @tparam DerivedType The class deriving from *this. Must define the
 *                      `*_()` hooks called below and declare *this a friend.
 */
template<typename DerivedType>
class AOShellBaseCommon {
private:
    /// Struct defining the types for the shell *this acts like
    using traits_type = ChemistClassTraits<AOShellBase>;

public:
    /// Type used to model the total angular momentum
    using angular_momentum_type = typename traits_type::angular_momentum_type;

    /// Type of the contracted Gaussian *this is built on
    using contracted_gaussian_type =
      typename traits_type::contracted_gaussian_type;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename traits_type::const_contracted_gaussian_reference;

    /// Type of a pointer to a polymorphic view of one AO in *this
    using ao_view_pointer = typename traits_type::ao_view_pointer;

    /// Type used to model the point *this is centered on
    using center_type = typename traits_type::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = typename traits_type::const_center_reference;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = typename traits_type::coord_type;

    /// Type of a normalization constant
    using numerical_value = coord_type;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the total angular momentum shared by the AOs in *this.
     *
     *  @throw None No throw guarantee.
     */
    angular_momentum_type get_l() const noexcept {
        return downcast_().get_l_();
    }

    /** @brief Is *this a pure (spherical) shell?
     *
     *  @return True if the AOs in *this are spherical AOs and false if they
     *          are Cartesian AOs.
     *
     *  @throw None No throw guarantee.
     */
    bool is_pure() const noexcept { return downcast_().is_pure_(); }

    /** @brief Is *this a Cartesian shell?
     *
     *  This is a convenience for `!is_pure()`.
     *
     *  @throw None No throw guarantee.
     */
    bool is_cartesian() const noexcept { return !is_pure(); }

    /** @brief Returns the number of AOs in *this.
     *
     *  This is @f$2\ell+1@f$ for a pure shell and @f$(\ell+1)(\ell+2)/2@f$ (the
     *  number of ways of writing @f$\ell@f$ as @f$i + j + k@f$) for a Cartesian
     *  one. It depends only on @f$\ell@f$ and the purity, not on the ordering,
     *  and so is computed here.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept {
        const auto l = get_l();
        return is_pure() ? 2 * l + 1 : (l + 1) * (l + 2) / 2;
    }

    /** @brief Returns the point *this is centered on.
     *
     *  @throw None No throw guarantee.
     */
    const_center_reference get_center() const {
        return downcast_().get_center_();
    }

    /** @brief Returns the contracted Gaussian every AO in *this is built on.
     *
     *  For a pure shell this is the contracted Gaussian of the Cartesian shell
     *  its AOs transform. Either way the returned view aliases the one
     *  contracted Gaussian *this owns or aliases.
     *
     *  @throw None No throw guarantee.
     */
    const_contracted_gaussian_reference get_contracted_gaussian() const {
        return downcast_().get_contracted_gaussian_();
    }

    /** @brief Returns the AO at offset @p i.
     *
     *  Which kind of AO a shell holds is only known to its derived class, so
     *  the result is a polymorphic, read-only view: a CartesianAOView for a
     *  Cartesian shell and a SphericalAOView for a pure one. The view is built
     *  on demand and the caller owns the returned pointer, but the view aliases
     *  the state of *this; it is not a copy, and so must not outlive *this.
     *  Code which knows the concrete shell type can index it directly instead,
     *  and get a pointer to the concrete view.
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
    ao_view_pointer at(size_type i) const {
        check_offset_(i);
        return downcast_().at_(i);
    }

    /// Same as at()
    ao_view_pointer operator[](size_type i) const { return at(i); }

    // -------------------------------------------------------------------------
    // -- Normalization
    // -------------------------------------------------------------------------

    /** @brief Returns the normalization constant of *this.
     *
     *  This is @f$N^{G}@f$, the contracted Gaussian's own constant, which is
     *  the one scalar all the AOs in a shell share. Per
     *  docs/source/developer/design/basis_set/normalization.rst it is what
     *  integral libraries expect (together with the per-primitive
     *  @f$N^{\chi}@f$). It does not include the per-AO factors, i.e.
     *  @f$N^{AO}_{ijk}@f$ for a Cartesian shell and the transformation
     *  coefficients for a pure one, which the AOs *this hands out apply
     *  themselves.
     *
     *  @throw std::runtime_error if the parameters of the contraction are not
     *                            all holding the same concrete floating-point
     *                            type. Strong throw guarantee.
     */
    numerical_value normalization_constant() const {
        return get_contracted_gaussian().normalization_constant();
    }

protected:
    /// Only the derived class should be creating/copying *this
    ///@{
    AOShellBaseCommon() noexcept                                    = default;
    AOShellBaseCommon(const AOShellBaseCommon&) noexcept            = default;
    AOShellBaseCommon(AOShellBaseCommon&&) noexcept                 = default;
    AOShellBaseCommon& operator=(const AOShellBaseCommon&) noexcept = default;
    AOShellBaseCommon& operator=(AOShellBaseCommon&&) noexcept      = default;
    ~AOShellBaseCommon() noexcept                                   = default;
    ///@}

    /// Throws std::out_of_range if @p i is not a valid offset into *this.
    /// Protected so that AOShellCommon can check its own accessors with it.
    void check_offset_(size_type i) const {
        if(i < size()) return;
        throw std::out_of_range("chemist::experimental::AOShellBase: offset " +
                                std::to_string(i) +
                                " is out of range for a shell with " +
                                std::to_string(size()) + " AOs.");
    }

private:
    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::experimental
