/*
 * Copyright 2023 NWChemEx-Project
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

namespace chemist::basis_set {

template<typename PrimitiveType>
struct PrimitiveTraits {
    /// Floating-point type used to hold the coefficient
    using coefficient_type = typename PrimitiveType::coefficient_type;

    /// Type of a mutable reference to the coefficient
    using coefficient_reference = typename PrimitiveType::coefficient_reference;

    /// Type of a read-only reference to the coefficient
    using const_coefficient_reference =
      typename PrimitiveType::const_coefficient_reference;

    /// Type of a pointer to a mutable coefficient
    using coefficient_pointer = typename PrimitiveType::coefficient_pointer;

    /// Type of a pointer to a read-only coefficient
    using const_coefficient_pointer =
      typename PrimitiveType::const_coefficient_pointer;

    /// Floating-point type used to hold the exponent
    using exponent_type = typename PrimitiveType::exponent_type;

    /// Type of a read/write reference to the exponent
    using exponent_reference = typename PrimitiveType::exponent_reference;

    /// Type of a read-only reference to a parameter
    using const_exponent_reference =
      typename PrimitiveType::const_exponent_reference;

    /// Type of a pointer to a mutable exponent
    using exponent_pointer = typename PrimitiveType::exponent_pointer;

    /// Type of a pointer to a read-only exponent
    using const_exponent_pointer =
      typename PrimitiveType::const_exponent_pointer;

    /// Rank 1 tensor-like type used to hold the center
    using center_type = typename PrimitiveType::center_type;

    /// Type of a mutable reference to the center
    using center_reference = typename PrimitiveType::center_reference;

    /// Type of a read-only reference to the center
    using const_center_reference =
      typename PrimitiveType::const_center_reference;

    /// Floating point type used for storing the center's coordinates
    using coord_type = typename PrimitiveType::coord_type;

    /// Type of a reference to a mutable coordinate
    using coord_reference = typename PrimitiveType::coord_reference;

    /// Type of a reference to a read-only coordinate
    using const_coord_reference = typename PrimitiveType::const_coord_reference;

    /// Type of a read-only view of a set of points
    using const_point_set_view = typename PrimitiveType::const_point_set_view;
};

} // namespace chemist::basis_set
