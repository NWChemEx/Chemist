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

namespace chemist {

Grid::Grid(wtf::buffer::FloatBuffer weights, wtf::buffer::FloatBuffer coords) :
  m_size_(weights.size()) {
    tensorwrapper::shape::Smooth weights_shape{m_size_};
    tensorwrapper::shape::Smooth points_shape{m_size_, size_type(3)};

    tensorwrapper::buffer::Contiguous weights_buffer(std::move(weights),
                                                     weights_shape);
    tensorwrapper::buffer::Contiguous points_buffer(std::move(coords),
                                                    points_shape);

    m_weights_ = buffer_type(weights_shape, std::move(weights_buffer));
    m_points_  = buffer_type(points_shape, std::move(points_buffer));
}

Grid::reference Grid::at_(size_type i) {
    auto& wbuf = tensorwrapper::buffer::make_contiguous(m_weights_.buffer());
    auto& pbuf = tensorwrapper::buffer::make_contiguous(m_points_.buffer());
    auto wview = wbuf.get_mutable_data();
    auto pview = pbuf.get_mutable_data();
    return reference(wview.at(i), pview.at(3 * i), pview.at(3 * i + 1),
                     pview.at(3 * i + 2));
}

Grid::const_reference Grid::at_(size_type i) const {
    const auto& wbuf =
      tensorwrapper::buffer::make_contiguous(m_weights_.buffer());
    const auto& pbuf =
      tensorwrapper::buffer::make_contiguous(m_points_.buffer());
    auto wview = wbuf.get_immutable_data();
    auto pview = pbuf.get_immutable_data();
    return const_reference(wview.at(i), pview.at(3 * i), pview.at(3 * i + 1),
                           pview.at(3 * i + 2));
}

Grid::size_type Grid::size_() const noexcept { return m_size_; }

} // namespace chemist
