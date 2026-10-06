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
#include <array>
#include <chemist/experimental/basis_set/ao_shell_enums.hpp>
#include <chemist/experimental/traits/ao_traits.hpp>
#include <chemist/experimental/traits/cartesian_ao_traits.hpp>
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <memory>
#include <type_traits>

namespace chemist::experimental {

class AOShellBase;
class AOShellBaseView;

template<typename AOType>
class AOShell;

template<typename AOType>
class AOShellView;

class SphericalAO;

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for experimental::AOShellBase.
 *
 *  A shell is one contracted Gaussian together with every AO of one kind
 *  (Cartesian or spherical) which can be built on it, so the types describing
 *  its state are pulled out of
 *  ChemistClassTraits<experimental::ContractedGaussian> and
 *  ChemistClassTraits<experimental::CartesianAO> rather than being redeclared.
 *
 *  Like ChemistClassTraits<experimental::AO>, there is no const
 *  specialization: the API a shell shares with a shell view
 *  (AOShellBaseCommon) is read-only, so only the read-only reference types are
 *  needed.
 */
template<>
struct ChemistClassTraits<experimental::AOShellBase> {
    /// Traits of the contracted Gaussian every shell is built on
    using contracted_gaussian_traits =
      ChemistClassTraits<experimental::ContractedGaussian>;

    /// Traits of the (read-only) Cartesian AOs a shell hands out
    using cartesian_ao_traits =
      ChemistClassTraits<const experimental::CartesianAO>;

    /// Type of the shell *this describes
    using value_type = experimental::AOShellBase;

    /// Type of a read-only reference to a shell
    using const_reference = const value_type&;

    /// Type acting like a read-only reference to a shell
    using const_view_type = experimental::AOShellBaseView;

    /// Type used to model the total angular momentum
    using angular_momentum_type =
      typename contracted_gaussian_traits::angular_momentum_type;

    /// Type used to model one Cartesian power
    using angular_index_type = typename cartesian_ao_traits::angular_index_type;

    /// Type holding the powers @f$(i,j,k)@f$ of one Cartesian AO
    using cartesian_powers_type = std::array<angular_index_type, 3>;

    /** @brief Type used to model the component, @f$m_\ell@f$, of one
     *         spherical AO.
     *
     *  The signed counterpart of angular_momentum_type, since
     *  @f$-\ell \le m_\ell \le \ell@f$.
     */
    using magnetic_index_type = std::make_signed_t<angular_momentum_type>;

    /// Type of the contracted Gaussian a shell is built on
    using contracted_gaussian_type =
      typename contracted_gaussian_traits::value_type;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename contracted_gaussian_traits::const_view_type;

    /** @brief Type of a read-only view of a Cartesian AO built on a shell's
     *         contracted Gaussian.
     *
     *  Only a Cartesian shell hands these out, but every shell can be viewed
     *  as the Cartesian shell underneath it, which is how SphericalAO builds
     *  the Cartesian AOs it sums over.
     */
    using const_cartesian_ao_reference =
      typename cartesian_ao_traits::const_view_type;

    /** @brief Type of a pointer to a polymorphic view of one AO in a shell.
     *
     *  Which kind of AO a shell holds is only known to the derived class, so
     *  the base hands its AOs out polymorphically. The views are built on
     *  demand, so the caller owns the pointer; the view it points to aliases
     *  the shell's state.
     */
    using ao_view_pointer = std::unique_ptr<experimental::AOView>;

    /// Type used to model the point a shell is centered on
    using center_type = typename contracted_gaussian_traits::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename contracted_gaussian_traits::const_center_reference;

    /// Type used to model a single Cartesian coordinate
    using coord_type = typename center_type::coord_type;
};

/** @brief Traits for experimental::AOShell<AOType>.
 *
 *  AOShell<AOType> is an AOShellBase whose purity is known statically, so
 *  every type describing the shell's state is pulled from
 *  ChemistClassTraits<experimental::AOShellBase>. What the purity adds is the
 *  kind of AO the shell holds, and with it the type of the angular index
 *  which picks one of those AOs out.
 *
 *  As for AOShellBase there is no const specialization, because the API shared
 *  by AOShell and AOShellView is read-only.
 *
 *  @tparam AOType The kind of AO the shell holds: CartesianAO or SphericalAO.
 */
template<typename AOType>
struct ChemistClassTraits<experimental::AOShell<AOType>> {
    static_assert(std::is_same_v<AOType, experimental::CartesianAO> ||
                    std::is_same_v<AOType, experimental::SphericalAO>,
                  "An AOShell must hold CartesianAO or SphericalAO objects.");

    /// Traits of the shell whose purity is not known statically
    using ao_shell_base_traits = ChemistClassTraits<experimental::AOShellBase>;

    /// Traits of the contracted Gaussian every shell is built on
    using contracted_gaussian_traits =
      typename ao_shell_base_traits::contracted_gaussian_traits;

    /// Type of the shell *this describes
    using value_type = experimental::AOShell<AOType>;

    /// Type of a read-only reference to a shell
    using const_reference = const value_type&;

    /// Type acting like a read-only reference to a shell
    using const_view_type = experimental::AOShellView<AOType>;

    /// Type of the AOs in the shell
    using ao_type = AOType;

    /// True if the shell holds spherical AOs, false if it holds Cartesian ones
    static constexpr bool is_pure =
      std::is_same_v<AOType, experimental::SphericalAO>;

    /// The purity of the shell, as the enumerator naming it
    static constexpr experimental::ShellPurity purity =
      is_pure ? experimental::ShellPurity::pure :
                experimental::ShellPurity::cartesian;

    /** @brief Type of a read-only view of one of the AOs in the shell.
     *
     *  This is what indexing the shell yields: a CartesianAOView for a
     *  Cartesian shell and a SphericalAOView for a spherical one.
     */
    using const_ao_reference =
      typename ChemistClassTraits<const AOType>::const_view_type;

    /** @brief Type of a pointer to a read-only view of one of the AOs in the
     *         shell.
     *
     *  The views are built on demand, so the caller owns the pointer; the
     *  view it points to aliases the shell's state.
     */
    using const_ao_pointer = std::unique_ptr<const_ao_reference>;

    /// Type of a read-only, polymorphic view of the Cartesian shell
    /// underneath a shell
    using cartesian_shell_view_type =
      experimental::AOShellView<experimental::CartesianAO>;

    /// Type of a pointer to a read-only, polymorphic view of the Cartesian
    /// shell underneath a shell
    using const_cartesian_shell_pointer =
      std::unique_ptr<cartesian_shell_view_type>;

    /// Type used to model the total angular momentum
    using angular_momentum_type =
      typename ao_shell_base_traits::angular_momentum_type;

    /// Type used to model one Cartesian power
    using angular_index_type =
      typename ao_shell_base_traits::angular_index_type;

    /// Type holding the powers @f$(i,j,k)@f$ of one Cartesian AO
    using cartesian_powers_type =
      typename ao_shell_base_traits::cartesian_powers_type;

    /// Type used to model the component, @f$m_\ell@f$, of one spherical AO
    using magnetic_index_type =
      typename ao_shell_base_traits::magnetic_index_type;

    /** @brief Type of the index picking one AO out of the shell.
     *
     *  The powers @f$(i,j,k)@f$ for a Cartesian shell and @f$m_\ell@f$ for a
     *  pure one.
     */
    using ao_index_type =
      std::conditional_t<is_pure, magnetic_index_type, cartesian_powers_type>;

    /// Type of the contracted Gaussian a shell is built on
    using contracted_gaussian_type =
      typename ao_shell_base_traits::contracted_gaussian_type;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename ao_shell_base_traits::const_contracted_gaussian_reference;

    /// Type of a read-only view of a Cartesian AO built on a shell's
    /// contracted Gaussian
    using const_cartesian_ao_reference =
      typename ao_shell_base_traits::const_cartesian_ao_reference;

    /// Type used to model the point a shell is centered on
    using center_type = typename ao_shell_base_traits::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename ao_shell_base_traits::const_center_reference;

    /// Type used to model a single Cartesian coordinate
    using coord_type = typename ao_shell_base_traits::coord_type;
};

} // namespace chemist
