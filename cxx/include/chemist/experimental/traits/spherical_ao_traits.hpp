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
#include <chemist/experimental/traits/ao_shell_traits.hpp>
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>

namespace chemist::experimental {

class SphericalAO;

template<typename SphericalAOType>
class SphericalAOView;

namespace detail_ {

/** @brief Types shared by both ChemistClassTraits<SphericalAO>
 *         specializations.
 *
 *  Per docs/source/developer/design/basis_set/ao_hierarchy.rst, a spherical AO
 *  pairs a Cartesian shell with a component @f$m_\ell@f$. The types describing
 *  the shell half are therefore pulled from
 *  ChemistClassTraits<AOShell<CartesianAO>>; what is genuinely new is the type
 *  used for @f$m_\ell@f$.
 *
 *  Unlike the Cartesian AO traits, the reference types do not change with the
 *  const-qualification of the AO. The shell is held polymorphically, and the
 *  polymorphic shell API is read-only, so there is no mutable reference to
 *  the shell (or to the contracted Gaussian beneath it) to hand out.
 */
struct SphericalAOTraitsCommon {
    /// Traits of the Cartesian shell *this is built on
    using ao_shell_traits =
      ChemistClassTraits<experimental::AOShell<experimental::CartesianAO>>;

    /// Type of the spherical AO *this describes
    using value_type = experimental::SphericalAO;

    /// Type of a read-only reference to a spherical AO
    using const_reference = const value_type&;

    /// Type acting like a read-only reference to a spherical AO
    using const_view_type = experimental::SphericalAOView<const value_type>;

    /// Type used to model the total angular momentum
    using angular_momentum_type =
      typename ao_shell_traits::angular_momentum_type;

    /// Type used to model the component, @f$m_\ell@f$
    using magnetic_index_type = typename ao_shell_traits::magnetic_index_type;

    /** @brief Type of the ordering-agnostic Cartesian shell *this is built on.
     *
     *  The shell's purity is part of this type, so a SphericalAO can not be
     *  built on a pure shell.
     */
    using shell_type = typename ao_shell_traits::value_type;

    /// Type of a read-only, aliasing view of a Cartesian shell
    using shell_view_type = typename ao_shell_traits::const_view_type;

    /// Type of a read-only view of one Cartesian AO of the shell
    using const_cartesian_ao_reference =
      typename ao_shell_traits::const_cartesian_ao_reference;

    /// Type of the contracted Gaussian *this is built on
    using contracted_gaussian_type =
      typename ao_shell_traits::contracted_gaussian_type;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename ao_shell_traits::const_contracted_gaussian_reference;

    /// Type used to model the point *this is centered on
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

/** @brief Traits for a mutable experimental::SphericalAO.
 *
 *  The only mutable state of a SphericalAO is @f$m_\ell@f$, which is not
 *  aliased, so a mutable reference is the only type which differs from the
 *  read-only specialization.
 */
template<>
struct ChemistClassTraits<experimental::SphericalAO>
  : experimental::detail_::SphericalAOTraitsCommon {
    /// Type of a mutable reference to a spherical AO
    using reference = value_type&;
};

/** @brief Traits for a read-only experimental::SphericalAO. */
template<>
struct ChemistClassTraits<const experimental::SphericalAO>
  : experimental::detail_::SphericalAOTraitsCommon {
    /// Type of a reference to a spherical AO (read-only because *this is
    /// const)
    using reference = const value_type&;
};

} // namespace chemist
