#pragma once
#include <chemist/point/point_class.hpp>
#include <chemist/traits/grid_traits.hpp>

namespace chemist {

/** @brief Class describing a point on a grid.
 *
 *  The GridPoint class models a single grid point. Each grid point within the
 *  grid is assumed to have:
 *    - A weight.
 *    - A x coordinate
 *    - A y coordinate
 *    - A z coordinate
 */
class GridPoint {
private:
    /// Struct defining the types for *this
    using traits_type = chemist::ChemistClassTraits<GridPoint>;

    /// Struct defining the types for the Point piece of *this
    using point_traits = typename traits_type::point_traits;

public:
    /// Type modeling the "point" piece of *this
    using point_type = typename traits_type::point_type;

    /// Type of a mutable reference to a point_type object
    using point_reference = typename point_traits::reference;

    /// Type of a read-only reference to a point_type object
    using const_point_reference = typename point_traits::const_reference;

    /// Type *this uses to model each coordinate
    using coord_type = typename point_traits::coord_type;

    /// Type *this uses to model the grid point's weight
    using weight_type = typename traits_type::weight_type;

    /// Type of a mutable reference to a weight_type object
    using weight_reference = typename traits_type::weight_reference;

    /// Type of a read-only reference to a weight_type object
    using const_weight_reference = typename traits_type::const_weight_reference;

    // -------------------------------------------------------------------------
    // -- Ctors
    // -------------------------------------------------------------------------

    /** @brief Creates a grid point at the origin with a weight of zero.
     *
     *  @throw None No throw guarantee.
     */
    GridPoint() = default;

    /** @brief Creates a point from coordinates and a weight.
     *
     *  @param[in] weight The weight of the grid point.
     *  @param[in] x The Cartesian x-coordinate of the grid point.
     *  @param[in] y The Cartesian y-coordinate of the grid point.
     *  @param[in] z The Cartesian z-coordinate of the grid point.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         state of the point_type piece of *this. Strong throw guarantee.
     */
    GridPoint(weight_type weight, coord_type x, coord_type y, coord_type z) :
      GridPoint(weight, point_type(x, y, z)) {}

    /** @brief Creates a point from a weight and a Point object.
     *
     *  @param[in] weight The weight of the grid point.
     *  @param[in] point The Cartesian coordinates of the grid point.
     *
     *  @throw None No throw guarantee.
     */
    GridPoint(weight_type weight, point_type point) :
      m_weight_(weight), m_point_(std::move(point)) {}

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the weight of this grid point.
     *
     *  @return A mutable reference to the weight of *this.
     *
     *  @throw None No throw guarantee.
     */
    weight_reference weight() { return m_weight_; }

    /** @brief Returns the weight of this grid point.
     *
     *  This method is the same as the non-const version except that the
     *  resulting reference is read-only.
     *
     *  @return A read-only reference to the weight of *this.
     *
     *  @throw None No throw guarantee.
     */
    const_weight_reference weight() const { return m_weight_; }

    /** @brief Returns the Cartesian coordinates of *this.
     *
     *  @return A mutable reference to the Cartesian coordinates of *this.
     *
     *  @throw None No throw guarantee.
     */
    point_reference point() { return m_point_; }

    /** @brief Returns the Cartesian coordinates of *this.
     *
     *  This method is the same as the non-const version except that the result
     *  is read-only.
     *
     *  @return A read-only reference to the Cartesian coordinates of *this.
     *
     *  @throw None No throw guarantee.
     */
    const_point_reference point() const { return m_point_; }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if @p rhs and *this are value equal.
     *
     *  Two GridPoint objects are value equal if their weights are value equal
     *  and if their Cartesian coordinates are value equal. Note that this is
     *  a check for exact equality (i.e., to machine epsilon).
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return true if *this and @p rhs are value equal and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const GridPoint& rhs) const noexcept {
        return std::tie(m_weight_, m_point_) ==
               std::tie(rhs.m_weight_, rhs.m_point_);
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

    /// The cartesian coordinates of *this.
    point_type m_point_;
};

} // namespace chemist