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
#include <chemist/experimental/traits/ao_traits.hpp>
#include <chemist/experimental/traits/cartesian_ao_traits.hpp>
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <type_traits>

namespace chemist::experimental {

class AOShell;
class AOShellView;

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for experimental::AOShell.
 *
 *  A shell is one contracted Gaussian together with every AO of one kind
 *  (Cartesian or spherical) which can be built on it, so the types describing
 *  its state are pulled out of
 *  ChemistClassTraits<experimental::ContractedGaussian> and
 *  ChemistClassTraits<experimental::CartesianAO> rather than being redeclared.
 *
 *  Like ChemistClassTraits<experimental::AO>, there is no const
 *  specialization: the API a shell shares with a shell view (AOShellCommon) is
 *  read-only, so only the read-only reference types are needed.
 */
template<>
struct ChemistClassTraits<experimental::AOShell> {
    /// Traits of the contracted Gaussian every shell is built on
    using contracted_gaussian_traits =
      ChemistClassTraits<experimental::ContractedGaussian>;

    /// Traits of the (read-only) Cartesian AOs a shell hands out
    using cartesian_ao_traits =
      ChemistClassTraits<const experimental::CartesianAO>;

    /// Type of the shell *this describes
    using value_type = experimental::AOShell;

    /// Type of a read-only reference to a shell
    using const_reference = const value_type&;

    /// Type acting like a read-only reference to a shell
    using const_view_type = experimental::AOShellView;

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

    /** @brief Type of a read-only reference to a polymorphic view of one AO
     *         in a shell.
     *
     *  Which kind of AO a shell holds is only known to the derived class, so
     *  the base hands its AOs out polymorphically. The shell owns the views,
     *  so they can be handed out by reference.
     */
    using const_ao_view_reference = const experimental::AOView&;

    /// Type used to model the point a shell is centered on
    using center_type = typename contracted_gaussian_traits::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename contracted_gaussian_traits::const_center_reference;

    /// Type used to model a single Cartesian coordinate
    using coord_type = typename center_type::coord_type;
};

} // namespace chemist
