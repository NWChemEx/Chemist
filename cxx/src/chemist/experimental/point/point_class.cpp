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

#include <chemist/experimental/point/point_class.hpp>

namespace chemist::experimental {

void Point::swap(Point& other) noexcept {
    m_x_.swap(other.m_x_);
    m_y_.swap(other.m_y_);
    m_z_.swap(other.m_z_);
}

Point::coord_reference Point::coord_(size_type q) {
    if(q == 0) return m_x_.as_view();
    if(q == 1) return m_y_.as_view();
    return m_z_.as_view();
}

Point::const_coord_reference Point::coord_(size_type q) const {
    if(q == 0) return m_x_.as_view();
    if(q == 1) return m_y_.as_view();
    return m_z_.as_view();
}

} // namespace chemist::experimental
