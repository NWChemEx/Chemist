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
#include <chemist/experimental/traits/point_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>

namespace chemist::experimental {

class AO;
class AOView;

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for experimental::AO.
 *
 *  Every AO, whatever its kind, is built on exactly one contracted Gaussian,
 *  so the types describing the angular momentum, the center, and the
 *  contracted Gaussian itself are pulled straight out of
 *  ChemistClassTraits<experimental::ContractedGaussian> rather than being
 *  redeclared here.
 *
 *  Unlike most traits there is no const specialization. The API an AO shares
 *  with an AOView (i.e., AOCommon) is entirely read-only, so the only
 *  reference types needed are the read-only ones, and those are the same
 *  regardless of the const-qualification of the AO.
 */
template<>
struct ChemistClassTraits<experimental::AO> {
    /// Traits of the contracted Gaussian every AO is built on
    using contracted_gaussian_traits =
      ChemistClassTraits<experimental::ContractedGaussian>;

    /// Traits of the points an AO is centered on and evaluated at
    using point_traits = ChemistClassTraits<experimental::Point>;

    /// Type of the AO *this describes
    using value_type = experimental::AO;

    /// Type of a read-only reference to an AO
    using const_reference = const value_type&;

    /// Type acting like a read-only reference to an AO
    using const_view_type = experimental::AOView;

    /// Type used to model the total angular momentum
    using angular_momentum_type =
      typename contracted_gaussian_traits::angular_momentum_type;

    /// Type of the contracted Gaussian an AO is built on
    using contracted_gaussian_type =
      typename contracted_gaussian_traits::value_type;

    /// Type of a read-only, aliasing view of the contracted Gaussian
    using const_contracted_gaussian_reference =
      typename contracted_gaussian_traits::const_view_type;

    /// Type used to model the point an AO is centered on
    using center_type = typename contracted_gaussian_traits::center_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename contracted_gaussian_traits::const_center_reference;

    /// Type of a read-only, aliasing view of a point to evaluate an AO at
    using const_point_reference = typename point_traits::const_view_type;

    /// Type used to model a single Cartesian coordinate
    using coord_type = typename point_traits::coord_type;
};

} // namespace chemist
