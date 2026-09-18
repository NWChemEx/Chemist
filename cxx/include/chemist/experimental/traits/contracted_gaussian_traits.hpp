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
#include <chemist/experimental/traits/primitive_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <wtf/wtf.hpp>

namespace chemist::experimental {

class ContractedGaussian;

template<typename ContractedGaussianType>
class ContractedGaussianView;

namespace detail_ {

/** @brief Types shared by both ChemistClassTraits<ContractedGaussian>
 *         specializations.
 *
 *  A ContractedGaussian hands out experimental::Primitive objects, so the
 *  coefficient/exponent/angular-momentum/center types are pulled straight out
 *  of ChemistClassTraits<experimental::Primitive> rather than being
 *  redeclared here. What is genuinely new for a ContractedGaussian is the
 *  buffer type used to store the coefficients and exponents as a structure of
 *  arrays --- the same wtf::buffer::FloatBuffer type PointSet uses for its
 *  coordinate arrays, restated directly here rather than reused via
 *  ChemistClassTraits<PointSet> to avoid coupling the basis_set component to
 *  the point component's PointSet traits struct for one typedef.
 */
struct ContractedGaussianTraitsCommon {
    /// Traits of the primitives *this hands out
    using primitive_traits = ChemistClassTraits<experimental::Primitive>;

    /// Type of the contracted Gaussian *this describes
    using value_type = experimental::ContractedGaussian;

    /// Type *this uses to model a coefficient/exponent. Reused from the
    /// primitive traits rather than redeclared.
    using coefficient_type = typename primitive_traits::coefficient_type;
    using exponent_type    = coefficient_type;

    /// Type *this uses to model the (shared) total angular momentum
    using angular_momentum_type =
      typename primitive_traits::angular_momentum_type;

    /// Type used to model the (shared) point *this is centered on
    using center_type = typename primitive_traits::center_type;

    /// Type used to own one parameter's worth of values (coefficients or
    /// exponents), across every primitive in the contraction
    using buffer_type = wtf::buffer::FloatBuffer;
};

} // namespace detail_

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::ContractedGaussian. */
template<>
struct ChemistClassTraits<experimental::ContractedGaussian>
  : experimental::detail_::ContractedGaussianTraitsCommon {
    /// Type of a mutable reference to a contracted Gaussian
    using reference = value_type&;

    /// Type of a read-only reference to a contracted Gaussian
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a contracted Gaussian
    using view_type = experimental::ContractedGaussianView<value_type>;

    /// Type acting like a read-only reference to a contracted Gaussian
    using const_view_type =
      experimental::ContractedGaussianView<const value_type>;

    /// Type of a primitive in *this
    using primitive_type = typename primitive_traits::value_type;

    /// Type acting like a mutable reference to a primitive in *this. This is
    /// what indexing *this yields.
    using primitive_reference = typename primitive_traits::view_type;

    /// Type acting like a read-only reference to a primitive in *this
    using const_primitive_reference =
      typename primitive_traits::const_view_type;

    /// Type of a mutable, aliasing view of the coefficient/exponent
    using coefficient_reference =
      typename primitive_traits::coefficient_reference;
    using exponent_reference = coefficient_reference;

    /// Type of a read-only, aliasing view of the coefficient/exponent
    using const_coefficient_reference =
      typename primitive_traits::const_coefficient_reference;
    using const_exponent_reference = const_coefficient_reference;

    /// Type of a mutable reference to the angular momentum
    using angular_momentum_reference =
      typename primitive_traits::angular_momentum_reference;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference =
      typename primitive_traits::const_angular_momentum_reference;

    /// Type of a mutable, aliasing view of the center
    using center_reference = typename primitive_traits::center_reference;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference =
      typename primitive_traits::const_center_reference;

    /// Type of a mutable, aliasing view of a coefficient/exponent array
    using buffer_reference = wtf::buffer::BufferView<wtf::fp::Float>;

    /// Type of a read-only, aliasing view of a coefficient/exponent array
    using const_buffer_reference =
      wtf::buffer::BufferView<const wtf::fp::Float>;
};

/** @brief Traits for a read-only experimental::ContractedGaussian.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  ContractedGaussianView, all the way down to the PrimitiveView objects it
 *  hands out.
 */
template<>
struct ChemistClassTraits<const experimental::ContractedGaussian>
  : experimental::detail_::ContractedGaussianTraitsCommon {
    /// Type of a reference to a contracted Gaussian (read-only because *this
    /// is const)
    using reference = const value_type&;

    /// Type of a read-only reference to a contracted Gaussian
    using const_reference = const value_type&;

    /// Type acting like a reference to a contracted Gaussian (read-only
    /// because *this is const)
    using view_type = experimental::ContractedGaussianView<const value_type>;

    /// Type acting like a read-only reference to a contracted Gaussian
    using const_view_type = view_type;

    /// Type of a primitive in *this
    using primitive_type = typename primitive_traits::value_type;

    /// Type acting like a reference to a primitive in *this (read-only
    /// because *this is const). This is what indexing *this yields.
    using primitive_reference = typename primitive_traits::const_view_type;

    /// Type acting like a read-only reference to a primitive in *this
    using const_primitive_reference = primitive_reference;

    /// Type of an aliasing view of the coefficient/exponent (read-only
    /// because *this is const)
    using coefficient_reference =
      typename primitive_traits::const_coefficient_reference;
    using exponent_reference = coefficient_reference;

    /// Type of a read-only, aliasing view of the coefficient/exponent
    using const_coefficient_reference = coefficient_reference;
    using const_exponent_reference    = coefficient_reference;

    /// Type of a reference to the angular momentum (read-only because *this
    /// is const)
    using angular_momentum_reference =
      typename primitive_traits::const_angular_momentum_reference;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference = angular_momentum_reference;

    /// Type of an aliasing view of the center (read-only because *this is
    /// const)
    using center_reference = typename primitive_traits::const_center_reference;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = center_reference;

    /// Type of an aliasing view of a coefficient/exponent array (read-only
    /// because *this is const)
    using buffer_reference = wtf::buffer::BufferView<const wtf::fp::Float>;

    /// Type of a read-only, aliasing view of a coefficient/exponent array
    using const_buffer_reference = buffer_reference;
};

} // namespace chemist
