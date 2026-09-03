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

#pragma once
#include <chemist/concepts.hpp>
#include <chemist/traits/grid_traits.hpp>
#include <tuple>

namespace chemist {

template<typename GridPointType>
class GridPointView;

/** @brief Class describing a weighted grid point.
 *
 *  The GridPoint class models a single grid point. Each grid point within the
 *  grid is assumed to have:
 *    - A weight.
 *    - A x coordinate
 *    - A y coordinate
 *    - A z coordinate
 *
 *  The weight and each coordinate are stored as type-erased
 *  WeaklyTypedFloat (WTF) floating-point values (wtf::fp::Float), so a
 *  GridPoint may be built from any WTF-registered floating-point type (e.g.,
 *  float, double, or an interval type), not just double.
 *
 *  The accessors follow a get/set pattern: `get_weight()`/`get_x()`/
 *  `get_y()`/`get_z()` are read-only, and `set_weight()`/`set_x()`/
 *  `set_y()`/`set_z()` mutate the corresponding value in place (via a
 *  FloatView, not by assigning a new wtf::fp::Float outright). This matters
 *  because wtf::fp::Float's own assignment operator replaces its type-erased
 *  holder wholesale, which would invalidate any GridPointView currently
 *  aliasing *this; mutating through a FloatView does not.
 */
class GridPoint {
private:
    /// Struct defining the types for *this
    using traits_type = chemist::ChemistClassTraits<GridPoint>;

    /// Grants GridPointView direct access to *this's private state so it can
    /// alias it. A public mutable accessor is deliberately not provided (see
    /// the class-level note on get/set above).
    template<typename GridPointType>
    friend class GridPointView;

public:
    /// Type *this uses to model each coordinate
    using coord_type = typename traits_type::coord_type;

    /// Type of a mutable, aliasing view of a coordinate
    using coord_reference = typename traits_type::coord_reference;

    /// Type of a read-only, aliasing view of a coordinate
    using const_coord_reference = typename traits_type::const_coord_reference;

    /// Type *this uses to model the grid point's weight
    using weight_type = typename traits_type::weight_type;

    /// Type of a mutable, aliasing view of the weight
    using weight_reference = typename traits_type::weight_reference;

    /// Type of a read-only, aliasing view of the weight
    using const_weight_reference = typename traits_type::const_weight_reference;

    // -------------------------------------------------------------------------
    // -- Ctors
    // -------------------------------------------------------------------------

    /** @brief Creates a grid point at the origin with a weight of zero.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *         *this. Strong throw guarantee.
     */
    GridPoint() = default;

    /** @brief Creates a point from coordinates and a weight, all of the same
     *         concrete floating-point type.
     *
     *  @tparam T The concrete floating-point type of @p weight, @p x, @p y,
     *            and @p z. Must satisfy the chemist::concepts::FloatingPoint
     *            concept.
     *
     *  @param[in] weight The weight of the grid point.
     *  @param[in] x The Cartesian x-coordinate of the grid point.
     *  @param[in] y The Cartesian y-coordinate of the grid point.
     *  @param[in] z The Cartesian z-coordinate of the grid point.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         state of *this. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T>
    GridPoint(T weight, T x, T y, T z) :
      m_weight_(std::move(weight)),
      m_x_(std::move(x)),
      m_y_(std::move(y)),
      m_z_(std::move(z)) {}

    /** @brief Creates a point from already type-erased coordinates/weight.
     *
     *  This ctor is primarily intended for internal/generic code which has
     *  already type-erased the weight/coordinates (e.g., because it received
     *  them from another type-erased source) and thus does not know their
     *  concrete floating-point type.
     *
     *  @param[in] weight The weight of the grid point.
     *  @param[in] x The Cartesian x-coordinate of the grid point.
     *  @param[in] y The Cartesian y-coordinate of the grid point.
     *  @param[in] z The Cartesian z-coordinate of the grid point.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         state of *this. Strong throw guarantee.
     */
    GridPoint(weight_type weight, coord_type x, coord_type y, coord_type z) :
      m_weight_(std::move(weight)),
      m_x_(std::move(x)),
      m_y_(std::move(y)),
      m_z_(std::move(z)) {}

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the weight of this grid point (read-only).
     *
     *  @return A read-only view of the weight of *this.
     *
     *  @throw None No throw guarantee.
     */
    const_weight_reference get_weight() const { return m_weight_.as_view(); }

    /** @brief Sets the weight of this grid point to @p value.
     *
     *  @tparam T The concrete floating-point type of @p value. Must satisfy
     *            the chemist::concepts::FloatingPoint concept.
     *
     *  This method mutates the weight of *this in place (via a FloatView),
     *  so it does not invalidate any GridPointView that may currently be
     *  aliasing *this.
     *
     *  @param[in] value The new value for the weight of *this.
     *
     *  @throw std::runtime_error if @p T does not match the concrete
     *                            floating-point type currently held by the
     *                            weight of *this. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T>
    void set_weight(T value) {
        m_weight_.as_view() = value;
    }

    /** @brief Returns the Cartesian x-coordinate of *this (read-only). */
    const_coord_reference get_x() const { return m_x_.as_view(); }

    /** @brief Sets the Cartesian x-coordinate of *this to @p value. */
    template<concepts::FloatingPoint T>
    void set_x(T value) {
        m_x_.as_view() = value;
    }

    /** @brief Returns the Cartesian y-coordinate of *this (read-only). */
    const_coord_reference get_y() const { return m_y_.as_view(); }

    /** @brief Sets the Cartesian y-coordinate of *this to @p value. */
    template<concepts::FloatingPoint T>
    void set_y(T value) {
        m_y_.as_view() = value;
    }

    /** @brief Returns the Cartesian z-coordinate of *this (read-only). */
    const_coord_reference get_z() const { return m_z_.as_view(); }

    /** @brief Sets the Cartesian z-coordinate of *this to @p value. */
    template<concepts::FloatingPoint T>
    void set_z(T value) {
        m_z_.as_view() = value;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if @p rhs and *this are value equal.
     *
     *  Two GridPoint objects are value equal if their weights are value equal
     *  and if their Cartesian coordinates are value equal. Note that this is
     *  a check for exact equality (i.e., to machine epsilon), and that two
     *  values wrapping different concrete floating-point types are never
     *  value equal (see wtf::fp::Float::operator== for more details).
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return true if *this and @p rhs are value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const GridPoint& rhs) const noexcept {
        return std::tie(m_weight_, m_x_, m_y_, m_z_) ==
               std::tie(rhs.m_weight_, rhs.m_x_, rhs.m_y_, rhs.m_z_);
    }

    /** @brief Determines if *this is different from @p rhs.
     *
     *  Two GridPoint objects are different if they are not value equal, i.e.,
     *  this method simply negates the result of operator==. See the description
     *  for operator== for the definition of value equality.
     *
     *  @param[in] rhs The object to compare *this to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const GridPoint& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// The weight associated with *this.
    weight_type m_weight_ = 0.0;

    /// The Cartesian x-coordinate of *this.
    coord_type m_x_ = 0.0;

    /// The Cartesian y-coordinate of *this.
    coord_type m_y_ = 0.0;

    /// The Cartesian z-coordinate of *this.
    coord_type m_z_ = 0.0;
};

} // namespace chemist
