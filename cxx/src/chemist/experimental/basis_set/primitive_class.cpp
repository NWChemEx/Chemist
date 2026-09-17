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

#include <chemist/experimental/basis_set/primitive_class.hpp>

namespace chemist::experimental {

void Primitive::swap(Primitive& other) noexcept {
    m_coefficient_.swap(other.m_coefficient_);
    m_exponent_.swap(other.m_exponent_);
    std::swap(m_l_, other.m_l_);
    m_center_.swap(other.m_center_);
}

Primitive::coefficient_reference Primitive::coefficient_() {
    return m_coefficient_.as_view();
}

Primitive::const_coefficient_reference Primitive::coefficient_() const {
    return m_coefficient_.as_view();
}

Primitive::exponent_reference Primitive::exponent_() {
    return m_exponent_.as_view();
}

Primitive::const_exponent_reference Primitive::exponent_() const {
    return m_exponent_.as_view();
}

} // namespace chemist::experimental
