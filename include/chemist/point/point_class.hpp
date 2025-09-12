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
#include <cmath>
#include <iomanip>
#include <memory>

namespace chemist {
namespace detail_ {
template<typename T>
class PointPIMPL;
}

/** @brief An object that is associated with a point in 3-D Cartesian space.
 *
 *  Conceptually a point is nothing more than three floating point values, which
 *  respectively indicate its Cartesian `x`, `y`, and `z` coordinates. By
 *  convention these coordinates can also be thought of respectively as the
 *  0-th, 1-st, and 2-nd components of a three-dimensional vector. The API of
 *  the Point class allows for both views by aliasing `x()`, `y()` and `z()` to
 *  `coord(0)`, `coord(1)`, and `coord(2)` respectively.
 *
 *  @note The implementation of this class is contained in point.cpp. Thus to
 *        use this class with a new type one needs to add an explicit template
 *        instantiation for that type to point.cpp (and declare it in this
 *        file).
 *
 * @tparam T The type used to store each Cartesian coordinate. Assumed to be a
 *         POD type like double or float.
 */
template<typename T>
class Point {
private:
    /// The type of the PIMPL used to implement this instance
    using pimpl_type = detail_::PointPIMPL<T>;

    /// The type of a pointer holding the PIMPL
    using pimpl_ptr = std::unique_ptr<pimpl_type>;

public:
    /// Type used to store the points coordinates, alias of @p T
    using coord_type = T;

    /// Type of a read/write reference to a coordinate of the point
    using coord_reference = coord_type&;

    /// Type of a read-only reference to a coordinate of the point
    using const_coord_reference = const coord_type&;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /** @brief Creates a new Point instance situated at the origin.
     *
     *  This ctor will create a new Point instance. The instance will sit at the
     *  origin and can be translated by directly modifying the references
     *  returned through `coord` or any of the convenience `x`, `y`, `z`
     *  functions.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    Point();

    /** @brief Ctor which allows users to specify the PIMPL used by the Poin
     *
     *  This ctor allows the user to control the PIMPL used for the Point. Right
     *  now the only use case is for making Points that actually contain
     *  aliases. This is done by the PointView class as well as the various
     *  basis set views.
     *
     * @param[in] pimpl The object that is actually providing the
     *            implementation.
     * @throw none No throw guarantee.
     */
    explicit Point(pimpl_ptr pimpl) noexcept;

    /** @brief Creates a new Point, which is a deep copy of another Point
     *
     *  This ctor creates a new Point instance by deep copying the coordinates
     *  of another Point instance.
     *
     *  @param[in] rhs The Point instance to copy the state from.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *         new PIMPL. Strong throw guarantee.
     */
    Point(const Point<T>& rhs) : Point(rhs.x(), rhs.y(), rhs.z()) {}

    /** @brief Creates a new Point whose state is taken from @p rhs.
     *
     *  Creates a new Point instance which now owns @p rhs's PIMPL. After this
     *  ctor all references to @p rhs's state remain valid, except that they now
     *  point to the state of this instance.
     *
     *  @param[in,out] rhs The instance whose state is being transferred. After
     *                 this operation @p rhs has no PIMPL and is thus in an
     *                 invalid state. The state can be made valid by move
     *                 assigning to @p rhs.
     *
     *  @throw none No throw guarantee
     */
    Point(Point<T>&& rhs) noexcept;

    /** @brief Makes the current instance contain a deep copy of another Point
     *
     *  This function overwrites the coordinates held within the Point with a
     *  deep copy of another Point's coordinates. References and pointers to
     *  the current instance's coordinates remain valid after this operation,
     *  but will contain @p rhs's values.
     *
     *  @param[in] rhs The Point instance to copy the state from.
     *
     *  @return The current instance with its components set to deep copys of
     *          @p rhs's values.
     *
     *  @throw none no throw guarantee.
     */
    Point<T>& operator=(const Point<T>& rhs) noexcept;

    /** @brief Transfers @p rhs's state to this instance.
     *
     *  This function will transfer @p rhs's PIMPL to the current instance.
     *  After this operation all references to @p rhs's state remain valid,
     *  except that they now point to the state of this instance. All references
     *  to this instance's previous state are invalidated by this operation.
     *
     *  @param[in,out] rhs The instance whose state is being transferred. After
     *                 this operation @p rhs has no PIMPL and is thus in an
     *                 invalid state. The state can be made valid by move
     *                 assigning to @p rhs.
     *
     *  @return The current instance containing @p rhs's state.
     *
     *  @throw none No throw guarantee
     */
    Point<T>& operator=(Point<T>&& rhs) noexcept;

    /** @brief Initializes a new Point instance so that it resides at the
     *         specified coordinate.
     *
     *  @param[in] x The value of the first component of the Point's location.
     *  @param[in] y The value of the second component of the Point's location.
     *  @param[in] z The value of the third component of the Point's location.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    Point(coord_type x, coord_type y, coord_type z);

    /** @brief Standard defaulted dtor
     *
     *  This dtor will free up the memory associated with the PIMPL. As a
     *  result, all references to the Point's coordinates will be invalidated.
     *
     *  @throw none No throw guarantee.
     */
    virtual ~Point() noexcept;

    /** @brief Returns the @p q-th coordinate of the point.
     *
     *  This function can be used to retrieve one of the point's coordinates
     *  by index. The resulting coordinate is read-/write-able.
     *
     *  @param[in] q The index of the coordinate to retrieve. Should be in the
     *             range [0, 3).
     *  @return The requested coordinate in a read-/write-able form.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *         throw guarantee.
     */
    coord_reference coord(size_type q);

    /** @brief Returns the @p q-th coordinate of the point in a read-only format
     *
     *  This function can be used to retrieve one of the point's coordinates
     *  by index. The resulting coordinate is read-only.
     *
     *  @param[in] q The index of the coordinate to retrieve. Should be in the
     *             range [0, 3).
     *  @return The requested coordinate as a read-only reference.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *         throw guarantee.
     */
    const_coord_reference coord(size_type q) const;

    /** @brief Returns the x-coordinate of the point in a read/write format.
     *
     *  This function is a convenience function for calling `coord(0)`.
     *
     *  @return A read/write reference to the x-coordinate of the point.
     *
     *  @throw none No throw guarantee.
     */
    coord_reference x() noexcept { return coord(0); }

    /** @brief Returns the x-coordinate of the point in a read-only format.
     *
     *  This function is a convenience function for calling `coord(0) const`.
     *
     *  @return A read-only reference to the x-coordinate of the point.
     *
     *  @throw none No throw guarantee.
     */
    const_coord_reference x() const noexcept { return coord(0); }

    /** @brief Returns the y-coordinate of the point in a read/write format.
     *
     *  This function is a convenience function for calling `coord(1)`.
     *
     *  @return A read/write reference to the y-coordinate of the point.
     *
     *  @throw none No throw guarantee.
     */
    coord_reference y() noexcept { return coord(1); }

    /** @brief Returns the y-coordinate of the point in a read-only format.
     *
     *  This function is a convenience function for calling `coord(1) const`.
     *
     *  @return A read-only reference to the y-coordinate of the point.
     *
     *  @throw none No throw guarantee.
     */
    const_coord_reference y() const noexcept { return coord(1); }

    /** @brief Returns the z-coordinate of the point in a read/write format.
     *
     *  This function is a convenience function for calling `coord(2)`.
     *
     *  @return A read/write reference to the z-coordinate of the point.
     *
     *  @throw none No throw guarantee.
     */
    coord_reference z() noexcept { return coord(2); }

    /** @brief Returns the z-coordinate of the point in a read-only format.
     *
     *  This function is a convenience function for calling `coord(2) const`.
     *
     *  @return A read-only reference to the z-coordinate of the point.
     *
     *  @throw none No throw guarantee.
     */
    const_coord_reference z() const noexcept { return coord(2); }

    /** @brief Returns the magnitude of the point
     *
     *  This function returns the magnitude of the current point, *i.e.*, the
     *  distance from the origin.
     *
     *  @return The magnitude of the current point.
     *
     *  @throw None No throw guarantee.
     */
    coord_type magnitude() const noexcept {
        return std::sqrt(inner_product(*this));
    }

    /** @brief Returns the inner product of *this with @p rhs.
     *
     * The inner product of two points is defined as:
     *
     * @f[
     *  \vec{a} \cdot \vec{b} = a_x b_x + a_y b_y + a_z b_z
     * @f]
     *
     * and is the scalar projection of point @f$\vec{a}@f$ onto the
     * point @f$\vec{b}@f$.
     *
     * @param[in] rhs The Point to take the inner product with.
     *
     * @return The inner product of *this with @p rhs.
     *
     * @throw None No throw guarantee.
     */
    coord_type inner_product(const Point<T>& rhs) const noexcept {
        return x() * rhs.x() + y() * rhs.y() + z() * rhs.z();
    }

    /** @brief Serialize Point instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& x() & y() & z();
    }

    /** @brief Deserialize for Point instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        ar& x() & y() & z();
    }

protected:
    /// Convenience fxn that makes a PIMPL that aliases this instance's state
    pimpl_ptr point_alias() const;

private:
    /// The instance actually implementing the Point class
    pimpl_ptr m_pimpl_;
}; // class Point

/** @brief Vector difference of two points.
 *
 *  This function returns the vector difference of two points, *i.e.*, for
 *  points @f$\vec{a}@f$ and @f$\vec{b}@f$, this function returns a point
 *  @f$\vec{c}@f$ such that the @f$i@f$-th component of @f$\vec{c}@f$ is
 *  given by:
 *  @f{
 *    c_i = a_i - b_i
 *  @f}
 *
 *  @tparam T The scalar type used to hold the components of the point.
 *
 *  @param[in] lhs The point we are subtracting @p rhs from.
 *  @param[in] rhs The point we are subtracting from @p lhs
 *
 *  @return The difference between @p lhs and @p rhs.
 */
template<typename T>
Point<T> operator-(const Point<T>& lhs, const Point<T>& rhs) noexcept {
    return Point<T>(lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z());
}

/** @brief Compares two Points for equality.
 *
 *  @relates Point
 *
 *  Two Point instances are equal if each of their three components compare
 *  bitwise equivalent (*i.e.*, Points with x-coordinates 3.12345 and 3.12346
 *  respectively will not compare equal).
 *
 *  @tparam T The type used to store the Points' components
 *
 *  @param[in] lhs The Point on the left-side of the operator.
 *  @param[in] rhs The Point on the right-side of the operator.
 *
 *  @return True if the Points are equal and false otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
bool operator==(const Point<T>& lhs, const Point<T>& rhs) noexcept {
    return std::tie(lhs.x(), lhs.y(), lhs.z()) ==
           std::tie(rhs.x(), rhs.y(), rhs.z());
}

/** @brief Determines if two Points are different.
 *
 *  @relates Point
 *
 *  Two Point instances are equal if each of their three components compare
 *  bitwise equivalent (*i.e.*, Points with x-coordinates 3.12345 and 3.12346
 *  respectively will not compare equal). We define "different" as not equal.
 *
 *  @tparam T The type used to store the Points' components
 *
 *  @param[in] lhs The Point on the left-side of the operator.
 *  @param[in] rhs The Point on the right-side of the operator.
 *
 *  @return False if the Points are equal and true otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
bool operator!=(const Point<T>& lhs, const Point<T>& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Adds a string representation of @p p to @p os.
 *
 *  This function is used to print a point to a string. The resulting format
 *  will be of the form:
 *
 *  ```
 *  x : <x-value>,
 *  y : <y-value>,
 *  z : <z-value>
 *  ```
 *  Coordinates will be printed with all significant figures.
 *
 *  @warning The string representation is primarily for logging purposes. It is
 *           not considered stable and should not be used for archiving at this
 *           point.
 *
 *  @param[in,out] os The stream to add the string representation of @p p to.
 *  @param[in] p The Point to convert to a string.
 *
 *  @return @p os after adding the string representation of @p p to it.
 *
 *  @throw ??? if an error arises in adding @p p to @p os an exception may be
 *             thrown. If this happens @p os is in a valid, but otherwise
 *             undefined state. Weak throw guarantee.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const Point<T>& p) {
    // Number of sig figs in a floating point number of type T
    static constexpr auto digits = std::numeric_limits<T>::digits10;
    auto old_precision           = os.precision();
    os << std::setprecision(digits);
    os << "x : " << p.x() << "," << std::endl;
    os << "y : " << p.y() << "," << std::endl;
    os << "z : " << p.z() << "";
    os << std::setprecision(old_precision);
    return os;
}

extern template class Point<double>;
extern template class Point<float>;

} // namespace chemist
