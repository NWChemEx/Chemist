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
#include <chemist/basis_set2/primitive/primitive.hpp>
#include <chemist/basis_set2/primitive/primitive_traits.hpp>
#include <tuple>

namespace chemist::basis_set::detail_ {

/** @brief Class implementing a Primitive object.
 *
 *  @tparam T The type used for the coefficient, exponent, and coordinates of
 *            the center.
 */
template<typename T>
struct PrimitivePIMPL {
    /// Type *this implements
    using parent_type = Primitive<T>;

    /// Contains all the types needed to work with a Primitive
    using prim_traits = PrimitiveTraits<parent_type>;

    /// Centered at origin, with coefficients and exponent of 0.0
    PrimitivePIMPL() = default;

    /// Stateful construction
    PrimitivePIMPL(typename prim_traits::center_type center,
                   typename prim_traits::coefficient_type c,
                   typename prim_traits::exponent_type e) :
      m_center(std::move(center)), m_coefficient(c), m_exponent(e) {}

    bool operator==(const PrimitivePIMPL& rhs) const noexcept {
        auto lhs_state = std::tie(m_center, m_coefficient, m_exponent);
        auto rhs_state =
          std::tie(rhs.m_center, rhs.m_coefficient, rhs.m_exponent);
        return lhs_state == rhs_state;
    }

    /// Where *this is centered
    typename prim_traits::center_type m_center{};

    /// The scale factor in front of the Gaussian function
    typename prim_traits::coefficient_type m_coefficient{};

    /// The exponent's scale factor
    typename prim_traits::exponent_type m_exponent{};
};

} // namespace chemist::basis_set::detail_
