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
#include <chemist/point/point_set.hpp>
#include <vector>

namespace chemist::detail_ {

/** @brief Implements a PointSet<T> instance.
 *
 *  @tparam T The type used for the Point's coordinates. Assumed to be either
 *            double or float.
 */
template<typename T>
class PointSetPIMPL {
public:
    /// Type we are implementing
    using parent_type = PointSet<T>;

    /// Reuse parent class's types
    ///@{
    using pimpl_pointer       = typename parent_type::pimpl_pointer;
    using value_type          = typename parent_type::value_type;
    using reference           = typename parent_type::reference;
    using const_reference     = typename parent_type::const_reference;
    using coord_pointer       = typename parent_type::coord_pointer;
    using const_coord_pointer = typename parent_type::const_coord_pointer;
    using size_type           = typename parent_type::size_type;
    ///@}

    /// Implements adding a Point<T> to the PointSet<T>
    void push_back(value_type point) {
        m_x_.push_back(point.x());
        m_y_.push_back(point.y());
        m_z_.push_back(point.z());
    }

    /// Implements retrieving a read/write reference to a Point<T>
    reference operator[](size_type i) {
        return reference(m_x_.at(i), m_y_.at(i), m_z_.at(i));
    }

    /// Implements retrieving a read-only reference to a Point<T>
    const_reference operator[](size_type i) const {
        return const_reference(m_x_.at(i), m_y_.at(i), m_z_.at(i));
    }

    coord_pointer x_data() noexcept {
        return size() != 0 ? m_x_.data() : nullptr;
    }

    const_coord_pointer x_data() const noexcept {
        return size() != 0 ? m_x_.data() : nullptr;
    }

    coord_pointer y_data() noexcept {
        return size() != 0 ? m_y_.data() : nullptr;
    }

    const_coord_pointer y_data() const noexcept {
        return size() != 0 ? m_y_.data() : nullptr;
    }

    coord_pointer z_data() noexcept {
        return size() != 0 ? m_z_.data() : nullptr;
    }

    const_coord_pointer z_data() const noexcept {
        return size() != 0 ? m_z_.data() : nullptr;
    }

    /// Implements PointSet<T>::size()
    size_type size() const noexcept { return m_x_.size(); }

    /// Implements most of PointSet<T>::operator==
    bool operator==(const PointSetPIMPL<T>& rhs) const noexcept {
        return std::tie(m_x_, m_y_, m_z_) ==
               std::tie(rhs.m_x_, rhs.m_y_, rhs.m_z_);
    }

private:
    /// Type of coordinate, should be T, but from Point<T> for consistency
    using coord_type = typename value_type::coord_type;

    /// All the x-coordinates
    std::vector<coord_type> m_x_;

    /// All the y-coordinates
    std::vector<coord_type> m_y_;

    /// All the z-coordinates
    std::vector<coord_type> m_z_;
};

} // namespace chemist::detail_
