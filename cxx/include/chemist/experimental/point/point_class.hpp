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

#pragma once
#include <chemist/experimental/point/point_common.hpp>
#include <iomanip>
#include <ostream>

namespace chemist::experimental {

/** @brief A location in three-dimensional Cartesian space.
 *
 *  Conceptually a point is nothing more than three floating-point values
 *  giving its Cartesian x-, y-, and z-coordinates. By convention those
 *  coordinates can also be thought of as the 0-th, 1-st, and 2-nd components
 *  of a three-dimensional vector, and the API supports both views by aliasing
 *  `get_x()`, `get_y()`, and `get_z()` to `get_coord(0)`, `get_coord(1)`, and
 *  `get_coord(2)` respectively.
 *
 *  Unlike chemist::Point, *this is not templated on the type used to store
 *  the coordinates. Coordinates are stored in the type-erased wtf::fp::Float
 *  class, which means the concrete floating-point type is chosen by whoever
 *  creates the point and never appears in the API. Consumers which genuinely
 *  need a concrete type are the ones obliged to name it.
 *
 *  *this owns its coordinates. See PointView for a class with the same API
 *  which aliases coordinates owned by something else, e.g., by a PointSet.
 *
 */
class Point : public PointCommon<Point, Point> {
private:
    /// Type *this inherits from
    using base_type = PointCommon<Point, Point>;

    /// Lets the CRTP base reach coord_
    friend base_type;

    /// Lets a view alias *this's coordinates directly
    template<typename PointType>
    friend class PointView;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::const_coord_reference;
    using typename base_type::coord_reference;
    using typename base_type::coord_type;
    using typename base_type::size_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a point situated at the origin.
     *
     *  The resulting point's coordinates are all double-precision zeros. The
     *  point can be moved by calling the `set_*` methods.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    Point() = default;

    /** @brief Creates a point at (@p x, @p y, @p z).
     *
     *  This is the ctor most callers want. The concrete floating-point type
     *  of the coordinates is deduced from the arguments and is what the point
     *  will hold from then on.
     *
     *  @tparam T The concrete floating-point type of @p x, @p y, and @p z.
     *
     *  @param[in] x The Cartesian x-coordinate of the point.
     *  @param[in] y The Cartesian y-coordinate of the point.
     *  @param[in] z The Cartesian z-coordinate of the point.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T>
    Point(T x, T y, T z) :
      m_x_(std::move(x)), m_y_(std::move(y)), m_z_(std::move(z)) {}

    /** @brief Creates a point from already type-erased coordinates.
     *
     *  This ctor is primarily for generic code which has received the
     *  coordinates from another type-erased source and thus does not know
     *  their concrete floating-point type.
     *
     *  @param[in] x The Cartesian x-coordinate of the point.
     *  @param[in] y The Cartesian y-coordinate of the point.
     *  @param[in] z The Cartesian z-coordinate of the point.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    Point(coord_type x, coord_type y, coord_type z) :
      m_x_(std::move(x)), m_y_(std::move(y)), m_z_(std::move(z)) {}

    /** @brief Creates a deep copy of @p other.
     *
     *  @param[in] other The point to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    Point(const Point& other) = default;

    /** @brief Takes ownership of @p other's coordinates.
     *
     *  @param[in,out] other The point to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @throw None No throw guarantee.
     */
    Point(Point&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @param[in] other The point to copy.
     *
     *  @return *this, after overwriting its coordinates.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    Point& operator=(const Point& other) = default;

    /** @brief Overwrites *this with @p other's coordinates.
     *
     *  @param[in,out] other The point to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @return *this, after overwriting its coordinates.
     *
     *  @throw None No throw guarantee.
     */
    Point& operator=(Point&& other) noexcept = default;

    /// Default, no-throw dtor
    ~Point() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @param[in,out] other The point to exchange state with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(Point& other) noexcept;

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what PointCommon::save wrote. Unlike PointView, *this owns
     *  its coordinates and so can replace them outright; the coordinates are
     *  therefore restored to whichever concrete floating-point type they were
     *  saved as, regardless of what *this was holding beforehand.
     *
     *  @tparam Archive The type of the cereal input archive.
     *
     *  @param[in,out] ar The archive to read from.
     *
     *  @throw std::runtime_error if the archive names a floating-point type
     *                            chemist does not know about or can not
     *                            deserialize. Weak throw guarantee.
     */
    template<typename Archive>
    void load(Archive& ar) {
        m_x_ = detail_::load_float(ar);
        m_y_ = detail_::load_float(ar);
        m_z_ = detail_::load_float(ar);
    }

private:
    /** @brief Implements the CRTP base's coordinate access.
     *
     *  @param[in] q Which coordinate is wanted. The CRTP base has already
     *               checked that this is in the range [0, 3).
     */
    ///@{
    coord_reference coord_(size_type q);
    const_coord_reference coord_(size_type q) const;
    ///@}

    /// The Cartesian x-coordinate of *this
    coord_type m_x_ = 0.0;

    /// The Cartesian y-coordinate of *this
    coord_type m_y_ = 0.0;

    /// The Cartesian z-coordinate of *this
    coord_type m_z_ = 0.0;
};

/** @brief Returns the vector difference of two points.
 *
 *  @relates Point
 *
 *  For points @f$\vec{a}@f$ and @f$\vec{b}@f$ this function returns the point
 *  @f$\vec{c}@f$ whose @f$i@f$-th component is @f$c_i = a_i - b_i@f$.
 *
 *  Either operand may be an owning Point or a view of one; the result always
 *  owns its coordinates.
 *
 *  @param[in] lhs The point being subtracted from.
 *  @param[in] rhs The point being subtracted.
 *
 *  @return A new Point holding the difference.
 *
 *  @throw std::runtime_error if the coordinates of @p lhs and @p rhs are not
 *                            all holding the same concrete floating-point
 *                            type. Strong throw guarantee.
 */
template<typename LHSDerived, typename LHSPoint, typename RHSDerived,
         typename RHSPoint>
Point operator-(const PointCommon<LHSDerived, LHSPoint>& lhs,
                const PointCommon<RHSDerived, RHSPoint>& rhs) {
    return Point(detail_::subtract(lhs.get_x(), rhs.get_x()),
                 detail_::subtract(lhs.get_y(), rhs.get_y()),
                 detail_::subtract(lhs.get_z(), rhs.get_z()));
}

/** @brief Adds a string representation of @p p to @p os.
 *
 *  @relates Point
 *
 *  The resulting format is:
 *
 *  ```
 *  x : <x-value>,
 *  y : <y-value>,
 *  z : <z-value>
 *  ```
 *
 *  @warning The string representation is primarily for logging purposes. It
 *           is not considered stable and should not be used for archiving.
 *
 *  @param[in,out] os The stream to add the string representation of @p p to.
 *  @param[in] p The point to convert to a string.
 *
 *  @return @p os, after adding the string representation of @p p to it.
 *
 *  @throw ??? if adding @p p to @p os throws. Weak throw guarantee.
 */
template<typename DerivedType, typename PointType>
std::ostream& operator<<(std::ostream& os,
                         const PointCommon<DerivedType, PointType>& p) {
    os << "x : " << detail_::to_string(p.get_x()) << "," << std::endl;
    os << "y : " << detail_::to_string(p.get_y()) << "," << std::endl;
    os << "z : " << detail_::to_string(p.get_z());
    return os;
}

} // namespace chemist::experimental
