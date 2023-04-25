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

#pragma once
#include <chemist/point/point_view2.hpp>
#include <memory>

namespace chemist {
namespace detail_ {
template<typename T>
class PointSetPIMPL;
}

/** @brief
 *
 *
 *
 */
template<typename T>
class PointSet {
public:
    /// The type of the PIMPL used to implement *this
    using pimpl_type = detail_::PointSetPIMPL<T>;

    /// The type of a pointer to a PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// The elements in the container
    using value_type = Point<T>;

    /// Read/write reference to an element
    using reference = PointView2<value_type>;

    /// Read-only reference to an element
    using const_reference = PointView2<const value_type>;

    /// Integral type used for indexing
    using size_type = std::size_t;

    PointSet() noexcept;
    explicit PointSet(pimpl_pointer pimpl) noexcept;
    ~PointSet() noexcept;

    reference at(size_type i);

    const_reference at(size_type i) const;

    size_type size() const noexcept;

private:
    bool has_pimpl_() const noexcept;

    void bounds_check_(size_type i) const;

    /// Actually implements the class
    pimpl_pointer m_pimpl_;
};

extern template class PointSet<float>;
extern template class PointSet<double>;

} // namespace chemist
