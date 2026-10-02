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
#include <chemist/experimental/basis_set/ao_shell_enums.hpp>
#include <chemist/experimental/traits/ao_shell_traits.hpp>
#include <chemist/experimental/traits/cartesian_ao_traits.hpp>
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <chemist/experimental/traits/spherical_ao_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <memory>
#include <type_traits>

namespace chemist::experimental {

template<typename AOType>
class CCAShell;

template<typename CCAShellType>
class CCAShellView;

namespace detail_ {

/** @brief Types shared by every ChemistClassTraits<CCAShell<AOType>>
 *         specialization.
 *
 *  A CCA shell is an AOShell with a particular ordering, so every type which
 *  depends neither on const-ness nor on the kind of AO is pulled from
 *  ChemistClassTraits<AOShell> rather than redeclared. What the kind of AO
 *  decides is whether the shell is pure and what indexing it hands out.
 *
 *  @tparam AOType The kind of AO the shell holds: CartesianAO or SphericalAO.
 */
template<typename AOType>
struct CCAShellTraitsCommon {
    static_assert(std::is_same_v<AOType, experimental::CartesianAO> ||
                    std::is_same_v<AOType, experimental::SphericalAO>,
                  "A CCAShell must hold CartesianAO or SphericalAO objects.");

    /// Traits of the ordering-agnostic shell *this is one kind of
    using ao_shell_traits = ChemistClassTraits<experimental::AOShell>;

    /// Traits of the contracted Gaussian *this is built on
    using contracted_gaussian_traits =
      typename ao_shell_traits::contracted_gaussian_traits;

    /// Type of the shell *this describes
    using value_type = experimental::CCAShell<AOType>;

    /// Type of the AOs in the shell
    using ao_type = AOType;

    /// True if the shell holds spherical AOs, false if it holds Cartesian ones
    static constexpr bool is_pure =
      std::is_same_v<AOType, experimental::SphericalAO>;

    /// The purity of the shell, as the enumerator naming it
    static constexpr experimental::ShellPurity purity =
      is_pure ? experimental::ShellPurity::pure :
                experimental::ShellPurity::cartesian;

    /// The order the shell enumerates its AOs in, as the enumerator naming it
    static constexpr experimental::AOOrdering ordering =
      experimental::AOOrdering::cca;

    /** @brief Type of a read-only view of one of the AOs in the shell.
     *
     *  This is what indexing the concrete shell yields: a CartesianAOView for
     *  a Cartesian shell and a SphericalAOView for a spherical one.
     */
    using const_ao_reference =
      typename ChemistClassTraits<const AOType>::const_view_type;

    /** @brief Type of a pointer to a read-only view of one of the AOs in the
     *         shell.
     *
     *  This is what indexing the concrete shell yields. The views are built on
     *  demand, so the caller owns the pointer; the view it points to aliases
     *  the shell's state.
     */
    using const_ao_pointer = std::unique_ptr<const_ao_reference>;

    /// Type of the Cartesian shell, in CCA order, underneath a shell
    using cartesian_shell_type =
      experimental::CCAShell<experimental::CartesianAO>;

    /// Type of a read-only view of the Cartesian shell underneath a shell
    using const_cartesian_shell_reference =
      experimental::CCAShellView<const cartesian_shell_type>;

    /// Type used to model the total angular momentum
    using angular_momentum_type =
      typename ao_shell_traits::angular_momentum_type;

    /// Type used to model one Cartesian power
    using angular_index_type = typename ao_shell_traits::angular_index_type;

    /// Type holding the powers @f$(i,j,k)@f$ of one Cartesian AO
    using cartesian_powers_type =
      typename ao_shell_traits::cartesian_powers_type;

    /// Type used to model the component, @f$m_\ell@f$, of one spherical AO
    using magnetic_index_type = typename ao_shell_traits::magnetic_index_type;

    /// Type of the contracted Gaussian *this is built on
    using contracted_gaussian_type =
      typename ao_shell_traits::contracted_gaussian_type;

    /// Type of a read-only view of a Cartesian AO built on the shell's
    /// contracted Gaussian
    using const_cartesian_ao_reference =
      typename ao_shell_traits::const_cartesian_ao_reference;

    /// Type used to model the point a shell is centered on
    using center_type = typename ao_shell_traits::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename ao_shell_traits::const_center_reference;

    /// Type used to model a single Cartesian coordinate
    using coord_type = typename ao_shell_traits::coord_type;
};

} // namespace detail_

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::CCAShell. */
template<typename AOType>
struct ChemistClassTraits<experimental::CCAShell<AOType>>
  : experimental::detail_::CCAShellTraitsCommon<AOType> {
private:
    /// Type of the base, so its members can be named
    using base_type = experimental::detail_::CCAShellTraitsCommon<AOType>;

public:
    /// Type of the shell *this describes
    using typename base_type::value_type;

    /// Type of a mutable reference to a CCA shell
    using reference = value_type&;

    /// Type of a read-only reference to a CCA shell
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a CCA shell
    using view_type = experimental::CCAShellView<value_type>;

    /// Type acting like a read-only reference to a CCA shell
    using const_view_type = experimental::CCAShellView<const value_type>;

    /// Type acting like a mutable reference to the contracted Gaussian
    using contracted_gaussian_reference =
      typename base_type::contracted_gaussian_traits::view_type;

    /// Type acting like a read-only reference to the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename base_type::contracted_gaussian_traits::const_view_type;
};

/** @brief Traits for a read-only experimental::CCAShell.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  CCAShellView, all the way down to the ContractedGaussianView it hands out.
 */
template<typename AOType>
struct ChemistClassTraits<const experimental::CCAShell<AOType>>
  : experimental::detail_::CCAShellTraitsCommon<AOType> {
private:
    /// Type of the base, so its members can be named
    using base_type = experimental::detail_::CCAShellTraitsCommon<AOType>;

public:
    /// Type of the shell *this describes
    using typename base_type::value_type;

    /// Type of a reference to a CCA shell (read-only because *this is const)
    using reference = const value_type&;

    /// Type of a read-only reference to a CCA shell
    using const_reference = const value_type&;

    /// Type acting like a reference to a CCA shell (read-only because *this
    /// is const)
    using view_type = experimental::CCAShellView<const value_type>;

    /// Type acting like a read-only reference to a CCA shell
    using const_view_type = view_type;

    /// Type acting like a reference to the contracted Gaussian (read-only
    /// because *this is const)
    using contracted_gaussian_reference =
      typename base_type::contracted_gaussian_traits::const_view_type;

    /// Type acting like a read-only reference to the contracted Gaussian
    using const_contracted_gaussian_reference = contracted_gaussian_reference;
};

} // namespace chemist
