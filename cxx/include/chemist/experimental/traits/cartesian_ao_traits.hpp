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
#include <chemist/experimental/traits/contracted_gaussian_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <cstddef>

namespace chemist::experimental {

class CartesianAO;

template<typename CartesianAOType>
class CartesianAOView;

namespace detail_ {

/** @brief Types shared by both ChemistClassTraits<CartesianAO>
 *         specializations.
 *
 *  A Cartesian AO pairs a contracted Gaussian with the Cartesian powers
 *  @f$(i,j,k)@f$, so the radial half of its state is described entirely by
 *  ChemistClassTraits<experimental::ContractedGaussian> and is pulled from
 *  there rather than redeclared. What is genuinely new is the type used for
 *  one Cartesian power.
 */
struct CartesianAOTraitsCommon {
    /// Traits of the contracted Gaussian *this is built on
    using contracted_gaussian_traits =
      ChemistClassTraits<experimental::ContractedGaussian>;

    /// Type of the Cartesian AO *this describes
    using value_type = experimental::CartesianAO;

    /// Type of the contracted Gaussian *this is built on
    using contracted_gaussian_type =
      typename contracted_gaussian_traits::value_type;

    /** @brief Type used to model one Cartesian power.
     *
     *  This is the same type used for the total angular momentum, and
     *  deliberately so: the powers sum to it, so a power which could not be
     *  held by an angular_momentum_type would not be meaningful.
     */
    using angular_index_type =
      typename contracted_gaussian_traits::angular_momentum_type;

    /// Type *this uses to model the total angular momentum
    using angular_momentum_type = angular_index_type;

    /// Type used to model the point *this is centered on
    using center_type = typename contracted_gaussian_traits::center_type;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = typename center_type::coord_type;
};

} // namespace detail_

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::CartesianAO. */
template<>
struct ChemistClassTraits<experimental::CartesianAO>
  : experimental::detail_::CartesianAOTraitsCommon {
    /// Type of a mutable reference to a Cartesian AO
    using reference = value_type&;

    /// Type of a read-only reference to a Cartesian AO
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a Cartesian AO
    using view_type = experimental::CartesianAOView<value_type>;

    /// Type acting like a read-only reference to a Cartesian AO
    using const_view_type = experimental::CartesianAOView<const value_type>;

    /** @brief Type acting like a mutable reference to the contracted Gaussian.
     *
     *  Note this is a ContractedGaussianView and not a
     *  ContractedGaussian&: a Cartesian AO which aliases its radial part
     *  needs to hand out something which aliases it too, and a C++ reference
     *  could not be rebound when the view is swapped.
     */
    using contracted_gaussian_reference =
      typename contracted_gaussian_traits::view_type;

    /// Type acting like a read-only reference to the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename contracted_gaussian_traits::const_view_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename contracted_gaussian_traits::const_center_reference;
};

/** @brief Traits for a read-only experimental::CartesianAO.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  CartesianAOView, all the way down to the ContractedGaussianView it hands
 *  out.
 */
template<>
struct ChemistClassTraits<const experimental::CartesianAO>
  : experimental::detail_::CartesianAOTraitsCommon {
    /// Type of a reference to a Cartesian AO (read-only because *this is
    /// const)
    using reference = const value_type&;

    /// Type of a read-only reference to a Cartesian AO
    using const_reference = const value_type&;

    /// Type acting like a reference to a Cartesian AO (read-only because
    /// *this is const)
    using view_type = experimental::CartesianAOView<const value_type>;

    /// Type acting like a read-only reference to a Cartesian AO
    using const_view_type = view_type;

    /// Type acting like a reference to the contracted Gaussian (read-only
    /// because *this is const)
    using contracted_gaussian_reference =
      typename contracted_gaussian_traits::const_view_type;

    /// Type acting like a read-only reference to the contracted Gaussian
    using const_contracted_gaussian_reference = contracted_gaussian_reference;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename contracted_gaussian_traits::const_center_reference;
};

} // namespace chemist
