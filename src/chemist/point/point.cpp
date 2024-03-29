/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/point/point.hpp"
#include "detail_/point_pimpl.hpp"
#include <cassert>

namespace chemist {

template<typename T>
Point<T>::Point() : Point(0.0, 0.0, 0.0) {}

template<typename T>
Point<T>::Point(Point<T>&& rhs) noexcept = default;

template<typename T>
Point<T>& Point<T>::operator=(const Point<T>& rhs) noexcept {
    x() = rhs.x();
    y() = rhs.y();
    z() = rhs.z();
    return *this;
}

template<typename T>
Point<T>& Point<T>::operator=(Point<T>&& rhs) noexcept = default;

template<typename T>
Point<T>::Point(T x, T y, T z) : Point(std::make_unique<pimpl_type>(x, y, z)) {}

template<typename T>
Point<T>::~Point<T>() noexcept = default;

template<typename T>
typename Point<T>::coord_reference Point<T>::coord(size_type i) {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->coord(i);
}

template<typename T>
typename Point<T>::const_coord_reference Point<T>::coord(size_type i) const {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->coord(i);
}

template<typename T>
Point<T>::Point(pimpl_ptr pimpl) noexcept : m_pimpl_(std::move(pimpl)) {}

template<typename T>
std::unique_ptr<detail_::PointPIMPL<T>> Point<T>::point_alias() const {
    return std::make_unique<pimpl_type>(
      const_cast<T*>(&x()), const_cast<T*>(&y()), const_cast<T*>(&z()));
}

template class Point<double>;
template class Point<float>;

} // namespace chemist
