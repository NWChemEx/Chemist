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
#include <chemist/experimental/traits/point_traits.hpp>
#include <chemist/traits/chemist_class_traits.hpp>
#include <cstddef>

namespace chemist::experimental {

class Primitive;

template<typename PrimitiveType>
class PrimitiveView;

namespace detail_ {

/** @brief Types shared by both ChemistClassTraits<Primitive> specializations.
 *
 *  The coefficient and the exponent are modeled the same way a Point models a
 *  coordinate, and the center *is* a Point, so none of that needs to be
 *  redefined here: it is pulled straight out of
 *  ChemistClassTraits<experimental::Point>, which already has both a mutable
 *  and a read-only flavor of everything a value/view pair needs (coord_type,
 *  coord_reference, const_coord_reference, view_type, const_view_type). Only
 *  ChemistClassTraits<Point> (never the const specialization) is used here;
 *  it already carries both flavors, so there is nothing the const primitive
 *  specialization needs that isn't reachable from it.
 *
 *  What is genuinely new for a Primitive, and thus lives directly in each
 *  ChemistClassTraits<Primitive> specialization instead of here, is only the
 *  const-qualification of the reference types: which of the point traits'
 *  mutable-vs-read-only flavors gets picked.
 */
struct PrimitiveTraitsCommon {
    /// Traits of the point *this is centered on
    using point_traits = ChemistClassTraits<experimental::Point>;

    /// Type of the primitive *this describes
    using value_type = experimental::Primitive;

    /// Type *this uses to model the coefficient and the exponent. A
    /// primitive's coefficient/exponent are floating-point values exactly
    /// like a point's coordinates, so this reuses Point's coord_type rather
    /// than redeclaring the same wtf::fp::Float type-erasure.
    using coefficient_type = typename point_traits::coord_type;
    using exponent_type    = coefficient_type;

    /// Type *this uses to model the total angular momentum. This is a plain,
    /// small, non-negative integer -- there is no benefit to type-erasing it
    /// the way the coefficient and exponent are -- so it is modeled directly
    /// as a std::size_t.
    using angular_momentum_type = std::size_t;

    /// Type used to model the point *this is centered on
    using center_type = typename point_traits::value_type;
};

} // namespace detail_

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::Primitive. */
template<>
struct ChemistClassTraits<experimental::Primitive>
  : experimental::detail_::PrimitiveTraitsCommon {
    /// Type of a mutable reference to a primitive
    using reference = value_type&;

    /// Type of a read-only reference to a primitive
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a primitive
    using view_type = experimental::PrimitiveView<value_type>;

    /// Type acting like a read-only reference to a primitive
    using const_view_type = experimental::PrimitiveView<const value_type>;

    /// Type of a mutable, aliasing view of the coefficient/exponent
    using coefficient_reference = typename point_traits::coord_reference;
    using exponent_reference    = coefficient_reference;

    /// Type of a read-only, aliasing view of the coefficient/exponent
    using const_coefficient_reference =
      typename point_traits::const_coord_reference;
    using const_exponent_reference = const_coefficient_reference;

    /// Type of a mutable reference to the angular momentum
    using angular_momentum_reference = angular_momentum_type&;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference = const angular_momentum_type&;

    /// Type of a mutable, aliasing view of the center
    using center_reference = typename point_traits::view_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = typename point_traits::const_view_type;
};

/** @brief Traits for a read-only experimental::Primitive.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  PrimitiveView.
 */
template<>
struct ChemistClassTraits<const experimental::Primitive>
  : experimental::detail_::PrimitiveTraitsCommon {
    /// Type of a reference to a primitive (read-only because *this is const)
    using reference = const value_type&;

    /// Type of a read-only reference to a primitive
    using const_reference = const value_type&;

    /// Type acting like a reference to a primitive (read-only because *this
    /// is const)
    using view_type = experimental::PrimitiveView<const value_type>;

    /// Type acting like a read-only reference to a primitive
    using const_view_type = experimental::PrimitiveView<const value_type>;

    /// Type of an aliasing view of the coefficient/exponent (read-only
    /// because *this is const)
    using coefficient_reference = typename point_traits::const_coord_reference;
    using exponent_reference    = coefficient_reference;

    /// Type of a read-only, aliasing view of the coefficient/exponent
    using const_coefficient_reference = coefficient_reference;
    using const_exponent_reference    = coefficient_reference;

    /// Type of a reference to the angular momentum (read-only because *this
    /// is const)
    using angular_momentum_reference = const angular_momentum_type&;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference = angular_momentum_reference;

    /// Type of an aliasing view of the center (read-only because *this is
    /// const)
    using center_reference = typename point_traits::const_view_type;

    /// Type of a read-only, aliasing view of the center
    using const_center_reference = center_reference;
};

} // namespace chemist
