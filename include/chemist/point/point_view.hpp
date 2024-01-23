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
#include <array>
#include <chemist/detail_/view/traits.hpp>
#include <chemist/point/point.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

namespace chemist {

/** @brief Allows viewing data as if it was a PointType object.
 *
 *  PointView objects act like references to a Point object. More specifically,
 *  the state inside a PointView is an alias of data owned by another object
 *  (usually a PointSet or related classes).
 *
 *
 *  @tparam PointType The type of Point<T> object this class is behaving like a
 *                    view of. Assumed to be either Point<T> or const Point<T>
 *                    (T being either float or double).
 */
template<typename PointType>
class PointView {
private:
    /// Type helping us do the template meta-programming
    using traits_type = detail_::ViewTraits<PointType>;

    /// Pull in apply_const_xxx (so we can drop typename and template)
    template<typename U>
    using apply_const_ref = typename traits_type::template apply_const_ref<U>;

public:
    /// Type of a non-CV qualified Point<T> object
    using point_type = typename traits_type::type;

    /// Type of reference to a Point<T> object with parallel const of *this
    using point_reference = apply_const_ref<point_type>;

    /// Type of a read-only reference to a Point<T> object
    using const_point_reference = const point_type&;

    /// Type of a PointView to a read-only point
    using const_point_view = PointView<const point_type>;

    /// Forward types from Point<T> class
    ///@{
    using coord_type            = typename point_type::coord_type;
    using const_coord_reference = typename point_type::const_coord_reference;
    using size_type             = typename point_type::size_type;
    ///@}

    /// Type of a coordinate reference returned when *this is non-const
    using coord_reference = apply_const_ref<coord_type>;

    /** @brief Value to reference converter.
     *
     *  This ctor allows you to make a PointView which aliases an existing
     *  Point object.
     *
     *  @param[in] point The Point object *this is a view of.
     *
     *  @throw None No throw guarantee.
     */
    PointView(point_reference point) :
      PointView(point.x(), point.y(), point.z()) {}

    /** @brief Value ctor.
     *
     *  This ctor will create a PointView which aliases the provided
     *  coordinates. The coordinates are held by reference and the caller is
     *  responsible for ensuring the memory of the coordinates remains in
     *  scope for the lifetime of the PointView.
     *
     *  @param[in] x The x-coordinate of the PointView
     *  @param[in] y The y-coordinate of the PointView
     *  @param[in] z The z-coordinate of the PointView
     *
     *  @throw None No throw guarantee.
     */
    PointView(coord_reference x, coord_reference y, coord_reference z) :
      m_pr_{&x, &y, &z} {}

    /** @brief Sets the value of the aliased Point to @p point
     *
     *  @note This is NOT the copy assignment operator!
     *
     *  This operator does not cause *this to alias @p point, but instead
     *  modifies the values of the Point aliased by *this. i.e. if before
     *  calling this method *this has state
     *  `{px, py, pz}` with `*px = 1.0`, `*py = 2.0`, and `*pz = 3.0` (`px`,
     *  `py`, and `pz` being pointers to the x, y, and z coordinates
     *  respectively) then after calling this method, `px`, `py`, and `pz`
     *  will still point to the same memory locations, but now
     *  `*px = point.x()`, `*py = point.y()`, and `*pz = point.z()`.
     *
     *  If you want to change the Point *this aliases you need to call the
     *  copy assignment operator, *i.e.* do something like:
     *
     *  ```.cpp
     *  Point r0, r1;
     *  PointView me(r0);
     *  r0 = PointView(r1);
     *  ```
     *
     *  @param[in] point The value we want to assign to the aliased Point.
     *
     *  @return *this after modifying the aliased Point.
     *
     *  @throw None No throw guarantee.
     */
    PointView& operator=(point_reference point) noexcept {
        (*m_pr_[0]) = point.x();
        (*m_pr_[1]) = point.y();
        (*m_pr_[2]) = point.z();
        return *this;
    }

    /** @brief Makes the aliased Point a copy of @p point
     *
     *  This method is used to make the state of the Point aliased by *this be
     *  a copy of the state in @p point.
     *
     *  @tparam PointType2 The type of @p point. Must be either Point<T> or
     *                     a reference to Point<T>.
     *  @tparam <Anonymous> Used to disable this method when *this aliases a
     *                      read-only Point<T> object and/or when PointType2 is
     *                      not  Point<T> (or a reference to Point<T>).
     *
     *  @param[in] point The object whose state will be copied.
     *
     *  @throw None No throw guarantee.
     */
    template<typename PointType2, typename = std::enable_if_t<std::is_same_v<
                                    std::decay_t<PointType2>, PointType>>>
    PointView& operator=(PointType2&& point) {
        (*m_pr_[0]) = point.x();
        (*m_pr_[1]) = point.y();
        (*m_pr_[2]) = point.z();
        return *this;
    }

    /** @brief Retrieve's the PointView's coordinates by offset.
     *
     *  @param[in] q which coordinate (i.e., x, y, or z) to return. @p q must
     *               be in the range [0, 3).
     *
     *  @return A reference to the requested coordinate. If *this is const, the
     *          returned reference will be read-only, otherwise whether or not
     *          the reference is read-only or read/write respectively depends
     *          on whether or not PointType is const-qualified.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    coord_reference coord(size_type q) { return *m_pr_[q]; }
    const_coord_reference coord(size_type q) const { return *m_pr_[q]; }
    ///@}

    /** @brief Retrieves the method's namesake coordinate.
     *
     *  The `x()` method is an alias for calling `coord(0)`, `y()` is an alias
     *  of `coord(1)`, and `z()` is an alias of `coord(1)`.
     *
     *  @return Non-const versions return (possibly) read/write references,
     *          whereas const versions always return read-only references. The
     *          non-const versions return read-only references if @p PointType
     *          is const-qualified.
     *
     *  @throw None No-throw guarantee
     */
    ///@{
    coord_reference x() noexcept { return coord(0); }
    coord_reference y() noexcept { return coord(1); }
    coord_reference z() noexcept { return coord(2); }

    const_coord_reference x() const noexcept { return coord(0); }
    const_coord_reference y() const noexcept { return coord(1); }
    const_coord_reference z() const noexcept { return coord(2); }
    ///@}

    coord_type magnitude() const noexcept {
        return std::sqrt(x() * x() + y() * y() + z() * z());
    }

    /** @brief Value comparison.
     *
     *  This method compares the coordinates aliased by *this to the coordinates
     *  owned by @p rhs.
     *
     *  @param[in] rhs The Point-like object we are comparing to.
     *
     *  @return True if the coordinates in *this are value equal to those in
     *          @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    bool operator==(const_point_reference& rhs) const noexcept {
        return std::tie(x(), y(), z()) == std::tie(rhs.x(), rhs.y(), rhs.z());
    }

    template<typename T>
    bool operator==(const PointView<T>& rhs) const noexcept {
        return std::tie(x(), y(), z()) == std::tie(rhs.x(), rhs.y(), rhs.z());
    }
    ///@}

    /** @brief Determines if *this represents a different point than @p rhs.
     *
     *  This method compares the coordinates aliased by *this to the coordinates
     *  owned by @p rhs.
     *
     *  @param[in] rhs The Point-like object we are comparing to.
     *
     *  @return False if the coordinates in *this are value equal to those in
     *          @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    bool operator!=(const_point_reference& rhs) const noexcept {
        return !(*this == rhs);
    }

    template<typename T>
    bool operator!=(const PointView<T>& rhs) const noexcept {
        return !(*this == rhs);
    }
    ///@}

    /** @brief Conversion to a Point object.
     *
     *  This method will deep copy the aliased state in to a new Point object.
     *
     *  @return A new Point instance which owns its coordinates.
     *
     *  @throw std::bad_alloc if there is a problem allocating the Point's
     *                        PIMPL. Strong throw guarantee.
     */
    point_type as_point() const { return point_type(x(), y(), z()); }

    /** @brief Implicit conversion from a mutable view to a const view.
     *
     *  @return A view of a read-only Point object.
     *
     *  @throw None No throw guarantee.
     */
    operator const_point_view() const {
        return const_point_view(x(), y(), z());
    }

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @param[in,out] other The PointView to exchange state with. After this
     *                       method has been called @p other will contain the
     *                       state which previously was in *this.
     *
     *  @throw None no throw guarantee.
     */
    void swap(PointView& other) noexcept { m_pr_.swap(other.m_pr_); }

private:
    /// The type of pointer used to alias a coordinate
    using internal_pointer =
      typename traits_type::template apply_const_ptr<coord_type>;

    /// Pointers to the x, y, and z coordinates (respectively)
    std::array<internal_pointer, 3> m_pr_;
};

/// Same as PointView::operator==, but when a Point is the LHS
template<typename CoordType, typename PointType>
bool operator==(const Point<CoordType>& lhs, const PointView<PointType>& rhs) {
    return rhs == lhs;
}

/// Same as PointView::operator!=, but when a Point is the LHS
template<typename CoordType, typename PointType>
bool operator!=(const Point<CoordType>& lhs, const PointView<PointType>& rhs) {
    return rhs != lhs;
}

/// Allows a Point to be printed
template<typename PointType>
std::ostream& operator<<(std::ostream& os, const PointView<PointType>& view) {
    using coord_type = typename PointView<PointType>::coord_type;
    os << std::fixed
       << std::setprecision(std::numeric_limits<coord_type>::digits10)
       << view.coord(0) << " " << view.coord(1) << " " << view.coord(2);
    return os;
}

} // namespace chemist
