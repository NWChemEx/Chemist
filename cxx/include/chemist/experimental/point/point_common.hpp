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
#include <chemist/concepts.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/experimental/detail_/float_serialization.hpp>
#include <chemist/experimental/traits/point_traits.hpp>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

namespace chemist::experimental {

/** @brief Implements the API shared by Point and PointView.
 *
 *  A point can either own its coordinates (Point) or alias coordinates owned
 *  by something else (PointView). Both need the same API; code which computes
 *  a distance should not have to care which one it was handed. Historically
 *  each such pair in Chemist declared, documented, and implemented its API
 *  twice.
 *
 *  This class breaks that pattern. Everything which can be expressed in terms
 *  of "give me the q-th coordinate" lives here, exactly once. The derived
 *  class supplies only `coord_(q)` (and its const overload) and is passed to
 *  *this as a template parameter, i.e., the curiously recurring template
 *  pattern. Because the derived type is known at compile time, none of this
 *  costs a virtual call --- points are used in the innermost loops of the
 *  library, so that matters.
 *
 *  @tparam DerivedType The class deriving from *this. Must define
 *                      `coord_(size_type)` and `coord_(size_type) const` and
 *                      must declare *this a friend.
 *  @tparam PointType The, possibly const-qualified, Point type the derived
 *                    class models. This is what decides whether the
 *                    coordinates handed out are mutable, and thus whether the
 *                    setters exist at all.
 */
template<typename DerivedType, typename PointType>
class PointCommon {
private:
    /// Struct defining the types for the point *this acts like
    using traits_type = ChemistClassTraits<PointType>;

    /// Enables a method only when *this can mutate the aliased coordinates
    template<typename U, typename ReturnType = void>
    using enable_if_mutable_t =
      std::enable_if_t<!std::is_const_v<U>, ReturnType>;

public:
    /// Type of a non-const-qualified Point object
    using point_type = typename traits_type::value_type;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = typename traits_type::coord_type;

    /// Type of a possibly mutable, aliasing view of a coordinate
    using coord_reference = typename traits_type::coord_reference;

    /// Type of a read-only, aliasing view of a coordinate
    using const_coord_reference = typename traits_type::const_coord_reference;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// The number of Cartesian coordinates a point has
    static constexpr size_type n_coords = 3;

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the @p q-th Cartesian coordinate of *this.
     *
     *  By convention coordinate 0 is x, coordinate 1 is y, and coordinate 2
     *  is z.
     *
     *  @param[in] q The index of the coordinate to retrieve. Must be in the
     *               range [0, 3).
     *
     *  @return A read-only view of the requested coordinate.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *                           throw guarantee.
     */
    const_coord_reference get_coord(size_type q) const {
        assert_coord_in_range_(q);
        return downcast_().coord_(q);
    }

    /** @brief Returns the Cartesian x-coordinate of *this.
     *
     *  Convenience function for calling `get_coord(0)`.
     *
     *  @return A read-only view of the x-coordinate.
     *
     *  @throw None No throw guarantee.
     */
    const_coord_reference get_x() const { return get_coord(0); }

    /** @brief Returns the Cartesian y-coordinate of *this.
     *
     *  Convenience function for calling `get_coord(1)`.
     *
     *  @return A read-only view of the y-coordinate.
     *
     *  @throw None No throw guarantee.
     */
    const_coord_reference get_y() const { return get_coord(1); }

    /** @brief Returns the Cartesian z-coordinate of *this.
     *
     *  Convenience function for calling `get_coord(2)`.
     *
     *  @return A read-only view of the z-coordinate.
     *
     *  @throw None No throw guarantee.
     */
    const_coord_reference get_z() const { return get_coord(2); }

    // -------------------------------------------------------------------------
    // -- Setters
    // -------------------------------------------------------------------------

    /** @brief Sets the @p q-th Cartesian coordinate of *this to @p value.
     *
     *  This method mutates the coordinate in place, so it does not invalidate
     *  any view which may currently be aliasing *this. When *this aliases
     *  read-only state this method does not participate in overload
     *  resolution.
     *
     *  @tparam T The concrete floating-point type of @p value.
     *
     *  @param[in] q The index of the coordinate to set. Must be in the range
     *               [0, 3).
     *  @param[in] value The new value of the coordinate.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *                           throw guarantee.
     *  @throw std::runtime_error if @p T does not match the concrete
     *                            floating-point type the coordinate is
     *                            currently holding. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T, typename U = PointType>
    enable_if_mutable_t<U> set_coord(size_type q, T value) {
        assert_coord_in_range_(q);
        downcast_().coord_(q) = std::move(value);
    }

    /** @brief Sets the Cartesian x-coordinate of *this to @p value.
     *
     *  Convenience function for calling `set_coord(0, value)`.
     */
    template<concepts::FloatingPoint T, typename U = PointType>
    enable_if_mutable_t<U> set_x(T value) {
        set_coord(0, std::move(value));
    }

    /** @brief Sets the Cartesian y-coordinate of *this to @p value.
     *
     *  Convenience function for calling `set_coord(1, value)`.
     */
    template<concepts::FloatingPoint T, typename U = PointType>
    enable_if_mutable_t<U> set_y(T value) {
        set_coord(1, std::move(value));
    }

    /** @brief Sets the Cartesian z-coordinate of *this to @p value.
     *
     *  Convenience function for calling `set_coord(2, value)`.
     */
    template<concepts::FloatingPoint T, typename U = PointType>
    enable_if_mutable_t<U> set_z(T value) {
        set_coord(2, std::move(value));
    }

    // -------------------------------------------------------------------------
    // -- Math
    // -------------------------------------------------------------------------

    /** @brief Returns the inner product of *this with @p rhs.
     *
     *  The inner product of two points is defined as:
     *
     *  @f[
     *   \vec{a} \cdot \vec{b} = a_x b_x + a_y b_y + a_z b_z
     *  @f]
     *
     *  @p rhs may be any point-like object, i.e., an owning Point or a view
     *  of one, and may hold a different const-qualification than *this.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherPoint The point type @p rhs models.
     *
     *  @param[in] rhs The point to take the inner product with.
     *
     *  @return The inner product, held in the same concrete floating-point
     *          type as the coordinates.
     *
     *  @throw std::runtime_error if the coordinates of *this and @p rhs are
     *                            not all holding the same concrete
     *                            floating-point type. Strong throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    coord_type inner_product(
      const PointCommon<OtherDerived, OtherPoint>& rhs) const {
        auto xx = detail_::multiply(get_x(), rhs.get_x());
        auto yy = detail_::multiply(get_y(), rhs.get_y());
        auto zz = detail_::multiply(get_z(), rhs.get_z());
        auto xy = detail_::add(as_const_view_(xx), as_const_view_(yy));
        return detail_::add(as_const_view_(xy), as_const_view_(zz));
    }

    /** @brief Returns the magnitude of *this.
     *
     *  The magnitude of a point is its distance from the origin, i.e., the
     *  square root of the point's inner product with itself.
     *
     *  @return The magnitude, held in the same concrete floating-point type
     *          as the coordinates.
     *
     *  @throw std::runtime_error if the coordinates of *this are not all
     *                            holding the same concrete floating-point
     *                            type. Strong throw guarantee.
     */
    coord_type magnitude() const {
        auto ip = inner_product(*this);
        return detail_::sqrt(as_const_view_(ip));
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two points are value equal if their x-, y-, and z-coordinates are
     *  respectively value equal. Coordinates held in different concrete
     *  floating-point types are never value equal. Whether either point owns
     *  or aliases its coordinates is NOT considered.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherPoint The point type @p rhs models.
     *
     *  @param[in] rhs The point to compare to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    bool operator==(
      const PointCommon<OtherDerived, OtherPoint>& rhs) const noexcept {
        return get_x() == rhs.get_x() && get_y() == rhs.get_y() &&
               get_z() == rhs.get_z();
    }

    /** @brief Determines if *this differs from @p rhs.
     *
     *  This method defines "different" as not value equal. See operator== for
     *  the definition of value equal.
     *
     *  @param[in] rhs The point to compare to *this.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    bool operator!=(
      const PointCommon<OtherDerived, OtherPoint>& rhs) const noexcept {
        return !((*this) == rhs);
    }

    /** @brief Serializes *this into @p ar.
     *
     *  Each coordinate is written as the name of its concrete floating-point
     *  type followed by its value. Only save is implemented here; loading
     *  requires replacing the coordinates outright, which only a point which
     *  owns them can do.
     *
     *  @tparam Archive The type of the cereal output archive.
     *
     *  @param[in,out] ar The archive to write to.
     *
     *  @throw std::runtime_error if any coordinate is holding an
     *                            uncertainty-quantification type, which can
     *                            not be serialized. Weak throw guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        for(size_type q = 0; q < n_coords; ++q)
            detail_::save_float(ar, get_coord(q));
    }

protected:
    /** @brief Throws std::out_of_range if @p q is not in the range [0, 3).
     *
     *  The bounds check lives here, rather than in each derived class, so
     *  that `coord_` has exactly one job: map an index which is already known
     *  to be valid onto the right coordinate. Every path which can reach
     *  `coord_` from outside the component goes through get_coord or
     *  set_coord, so checking in those two places is sufficient.
     *
     *  @param[in] q The index to check.
     *
     *  @throw std::out_of_range if @p q is not in the range [0, 3). Strong
     *                           throw guarantee.
     */
    static void assert_coord_in_range_(size_type q) {
        if(q < n_coords) return;
        throw std::out_of_range("chemist::experimental: coordinate index " +
                                std::to_string(q) +
                                " is not in the range [0, 3).");
    }

    /// Only the derived class should be creating/copying *this
    ///@{
    PointCommon() noexcept                              = default;
    PointCommon(const PointCommon&) noexcept            = default;
    PointCommon(PointCommon&&) noexcept                 = default;
    PointCommon& operator=(const PointCommon&) noexcept = default;
    PointCommon& operator=(PointCommon&&) noexcept      = default;
    ~PointCommon() noexcept                             = default;
    ///@}

private:
    /// Wraps casting *this to the derived class
    DerivedType& downcast_() noexcept {
        return static_cast<DerivedType&>(*this);
    }

    /// Wraps casting *this to a read-only reference to the derived class
    const DerivedType& downcast_() const noexcept {
        return static_cast<const DerivedType&>(*this);
    }

    /** @brief Wraps taking a read-only view of an owning Float.
     *
     *  The arithmetic helpers return owning Float objects, which then need to
     *  be fed back in as read-only views. Calling as_view() directly on the
     *  temporary would alias an object which is about to die, so the value
     *  has to be named first; this helper is the reminder to do so.
     */
    static detail_::const_float_reference as_const_view_(
      const coord_type& value) {
        return value.as_view();
    }
};

} // namespace chemist::experimental
