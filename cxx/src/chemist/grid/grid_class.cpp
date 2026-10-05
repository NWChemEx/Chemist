/*
 * Copyright 2025 NWChemEx-Project
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

#include <chemist/grid/grid_class.hpp>
#include <stdexcept>
#include <string>
#include <utility>

namespace chemist {

Grid::Grid(wtf::buffer::FloatBuffer weights, point_set_type points) {
    const auto n_points = points.size();
    if(weights.size() != n_points)
        throw std::invalid_argument("chemist::Grid: number of weights (" +
                                    std::to_string(weights.size()) +
                                    ") does not match the number of points (" +
                                    std::to_string(n_points) + ").");

    tensorwrapper::shape::Smooth weights_shape{n_points};
    tensorwrapper::buffer::Contiguous weights_buffer(std::move(weights),
                                                     weights_shape);

    m_weights_ = buffer_type(weights_shape, std::move(weights_buffer));
    m_points_  = std::move(points);
}

Grid::point_set_reference Grid::get_points() {
    return point_set_reference(m_points_);
}

Grid::const_point_set_reference Grid::get_points() const {
    return const_point_set_reference(m_points_);
}

Grid::reference Grid::at_(size_type i) {
    auto& wbuf = tensorwrapper::buffer::make_contiguous(m_weights_.buffer());
    auto wview = wbuf.get_mutable_data();
    return reference(wview.at(i), m_points_.get_x_buffer().at(i),
                     m_points_.get_y_buffer().at(i),
                     m_points_.get_z_buffer().at(i));
}

Grid::const_reference Grid::at_(size_type i) const {
    const auto& wbuf =
      tensorwrapper::buffer::make_contiguous(m_weights_.buffer());
    auto wview = wbuf.get_immutable_data();
    return const_reference(wview.at(i), m_points_.get_x_buffer().at(i),
                           m_points_.get_y_buffer().at(i),
                           m_points_.get_z_buffer().at(i));
}

Grid::size_type Grid::size_() const noexcept { return m_points_.size(); }

} // namespace chemist
