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

#include <chemist/experimental/point/point_set_class.hpp>
#include <stdexcept>

namespace chemist::experimental {

PointSet::PointSet(buffer_type x, buffer_type y, buffer_type z) :
  m_x_(std::move(x)), m_y_(std::move(y)), m_z_(std::move(z)) {
    if(m_x_.size() == m_y_.size() && m_y_.size() == m_z_.size()) return;
    throw std::invalid_argument(
      "chemist::experimental::PointSet: the x-, y-, and z-coordinate arrays "
      "must all be the same length.");
}

void PointSet::swap(PointSet& other) noexcept {
    std::swap(m_x_, other.m_x_);
    std::swap(m_y_, other.m_y_);
    std::swap(m_z_, other.m_z_);
}

PointSet::buffer_reference PointSet::buffer_(size_type q) {
    if(q == 0) return m_x_.as_view();
    if(q == 1) return m_y_.as_view();
    return m_z_.as_view();
}

PointSet::const_buffer_reference PointSet::buffer_(size_type q) const {
    if(q == 0) return m_x_.as_view();
    if(q == 1) return m_y_.as_view();
    return m_z_.as_view();
}

} // namespace chemist::experimental
