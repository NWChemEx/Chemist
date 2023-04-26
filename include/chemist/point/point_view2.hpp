#pragma once
#include <array>
#include <chemist/detail_/view/traits.hpp>
#include <chemist/point/point.hpp>
#include <type_traits>

namespace chemist {

/** @brief Allows viewing data as if it was a PointType object.
 *
 *  PointView objects act like references to a Point object. More specifically,
 *  the state inside a PointView is an alias of data owned by another object
 *  (usually a PointSet or related classes). Importantly, PointView objects can
 *  implicitly convert to Point objects, so that the user need not concern
 *  themselves with the existence of the PointView object unless they want to.
 *
 *  TODO: This class should replace PointView, at which point we should drop the
 *        2 from the name. This requires switching over Chemist's basis set
 *        component.
 *
 *  @tparam PointType The type of Point<T> object this class is behaving like a
 *                    view of. Assumed to be either Point<T> or const Point<T>
 *                    (T being either float or double).
 */
template<typename PointType>
class PointView2 {
private:
    /// Type helping us do the template meta-programming
    using traits_type = detail_::ViewTraits<PointType>;

public:
    /// Type of a non-CV qualified Point<T> object
    using point_type = typename traits_type::type;

    /// Forward types from Point<T> class
    ///@{
    using coord_type            = typename point_type::coord_type;
    using const_coord_reference = typename point_type::const_coord_reference;
    using size_type             = typename point_type::size_type;
    ///@}

    /// Type of a coordinate reference returned when *this is non-const
    using coord_reference = typename traits_type::apply_const_ref<coord_type>;

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
    PointView2(coord_reference x, coord_reference y, coord_reference z) :
      m_pr_{&x, &y, &z} {}

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
     *  @param[in] rhs The Point we are comparing to.
     *
     *  @return True if the coordinates in *this are value equal to those in
     *          @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Point<coord_type>& rhs) const noexcept {
        return std::tie(x(), y(), z()) == std::tie(rhs.x(), rhs.y(), rhs.z());
    }

    /** @brief Determines if *this represents a different point than @p rhs.
     *
     *  This method compares the coordinates aliased by *this to the coordinates
     *  owned by @p rhs.
     *
     *  @param[in] rhs The Point we are comparing to.
     *
     *  @return False if the coordinates in *this are value equal to those in
     *          @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Point<coord_type>& rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Implicit conversion to a Point object.
     *
     *  @return A new Point instance which owns its coordinates.
     *
     */
    operator Point<coord_type>() const {
        return Point<coord_type>(x(), y(), z());
    }

private:
    /// The type of pointer used to alias a coordinate
    using internal_pointer = typename traits_type::apply_const_ptr<coord_type>;

    /// Pointers to the x, y, and z coordinates (respectively)
    std::array<internal_pointer, 3> m_pr_;
};

/// Same as PointView2::operator==, but when a Point is the LHS
template<typename CoordType, typename PointType>
bool operator==(const Point<CoordType>& lhs, const PointView2<PointType>& rhs) {
    return rhs == lhs;
}

/// Same as PointView2::operator!=, but when a Point is the LHS
template<typename CoordType, typename PointType>
bool operator!=(const Point<CoordType>& lhs, const PointView2<PointType>& rhs) {
    return rhs != lhs;
}

} // namespace chemist
