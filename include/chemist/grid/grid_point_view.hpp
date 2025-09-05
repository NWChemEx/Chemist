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
#include <chemist/grid/grid_point.hpp>
#include <chemist/point/point_view.hpp>
#include <chemist/traits/grid_traits.hpp>

namespace chemist {

/** @brief Aliases existing state so it can be used like a GridPoint object.
 *
 *  @tparam GridPointType The cv-qualified type of the GridPoint object *this
 *                        behaves like.
 *
 *  The GridPointView class enables aliased data to be used as if it were
 *  stored in a GridPoint object.
 */
template<typename GridPointType>
class GridPointView {
private:
    /// Struct defining the types for the GridPoint *this will act like.
    using grid_point_traits = ChemistClassTraits<GridPointType>;

    /// Struct defining the types for the point_type piece of *this.
    using point_traits = typename grid_point_traits::point_traits;

    /** @brief Determines if @p T is the same as GridPointType
     *
     *  @tparam T The type to compare to GridPointType
     *
     *  This variable will be set to true if @p T is the same type as
     *  GridPointType and if GridPointType is not cv-qualified (if it is
     *  cv-qualified, std::decay_t is not able to match it).
     */
    template<typename T>
    static constexpr bool is_grid_point_type =
      std::is_same_v<std::decay_t<T>, GridPointType>;

    /** @brief Enables a function when is_grid_point_type<T> is true.
     *
     *  @tparam T The type to compare to GridPointType
     */
    template<typename T>
    using enable_if_assign_from_point = std::enable_if_t<is_grid_point_type<T>>;

public:
    /// Type of a non-CV qualified GridPoint
    using grid_point_type = typename grid_point_traits::value_type;

    /// Type of a reference to a possibly cv-qualified grid_point_type object
    using grid_point_reference = typename grid_point_traits::reference;

    /// Type of a reference to a const-qualified grid_point_type
    using const_grid_point_reference =
      typename grid_point_traits::const_reference;

    /// Type of a reference to a possibly cv-qualified grid point's weight
    using weight_reference = typename grid_point_traits::weight_reference;

    /// Type of a read-only reference to a grid point's weight
    using const_weight_reference =
      typename grid_point_traits::const_weight_reference;

    /// Type of a view to a possibly cv-qualified point_type object
    using point_view = typename point_traits::view_type;

    /// Type of a read-only view to a point_type object
    using const_point_view = typename point_traits::const_view_type;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view that aliases an existing GridPoint object.
     *
     *  This ctor is used to alias an existing GridPoint object.
     *
     *  @param[in] other The object to alias.
     *
     *  @throw std::bad_alloc if allocating state for a PointView object fails.
     *                        Strong throw guarantee.
     */
    GridPointView(grid_point_reference other) :
      GridPointView(other.weight(), other.point()) {}

    /** @brief Creates a view that aliases existing state.
     *
     *  This ctor is used to create a view that aliases state, regardless of
     *  whether that state is owned by a GridPoint object or not.
     *
     *  @param[in] weight A reference to the weight of the grid point.
     *  @param[in] point A view of the Cartesian coordinates.
     *
     *  @throw None No throw guarantee.
     */
    GridPointView(weight_reference weight, point_view point) :
      m_pweight_(&weight), m_point_(std::move(point)) {}

    /** @brief Copies an existing view.
     *
     *  This ctor creates a new view which aliases the same GridPoint as
     *  @p other, i.e., the resulting view is a shallow copy of the aliased
     *  GridPoint.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if allocating the state for the copy fails.
     *                        Strong throw guarantee.
     */
    GridPointView(const GridPointView& other) = default;

    /** @brief Takes the state from another view.
     *
     *  This ctor moves the state from @p other into *this. After this operation
     *  *this will alias the GridPoint that @p other used to.
     *
     *  @param[in,out] other The view to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined state.
     *
     *  @throw None No throw guarantee.
     */
    GridPointView(GridPointView&& other) noexcept = default;

    /** @brief Assigns a copy of a view to *this.
     *
     *  @param[in] rhs The view to copy.
     *
     *  @return A mutable reference to *this after replacing the state with a
     *          copy of @p rhs's state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state for the
     *                        copy. Strong throw guarantee.
     */
    GridPointView& operator=(const GridPointView& rhs) = default;

    /** @brief Transfers the state of @p rhs to *this.
     *
     *  @param[in,out] rhs The object to take the state from. After this
     *                     operation @p rhs will be in a valid, but otherwise
     *                     undefined state.
     *
     *  @return *this after taking ownership of the state in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    GridPointView& operator=(GridPointView&& rhs) noexcept = default;

    /** @brief Causes *this to alias a different GridPoint object.
     *
     *  @tparam RHSType The cv-qualified type of the GridPoint to be aliased.
     *  @tparam <anonymous> Type used to disable this method via SFINAE when
     *                      either *this is a read-only view or when @p RHSType
     *                      is not a GridPoint type.
     *
     *  This method will overwrite the state in *this (if any) so that it
     *  instead aliases the state of @p rhs.
     *
     *  @param[in] rhs The GridPoint object to alias.
     *
     *  @return The current instance after it has been made to alias @p rhs.
     *
     *  @throw std::bad_alloc if allocating the new state throws. Strong throw
     *                        guarantee.
     */
    template<typename RHSType, typename = enable_if_assign_from_point<RHSType>>
    GridPointView& operator=(RHSType&& rhs) {
        m_pweight_ = &rhs.weight();
        m_point_   = rhs.point();
        return *this;
    }

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the weight aliased by *this.
     *
     *  @return A possibly mutable reference to the weight of *this.
     *
     *  @throw None No throw guarantee.
     */
    weight_reference weight() { return *m_pweight_; }

    /** @brief Returns the weight aliased by *this.
     *
     *  This method is the same as the non-const version except that it returns
     *  a read-only reference. See the documentation for the non-const version
     *  for more details.
     *
     *  @return A read-only reference to the aliased weight.
     *
     *  @throw None No throw guarantee.
     */
    const_weight_reference weight() const { return *m_pweight_; }

    /** @brief Returns a view of the Cartesian coordinates of *this.
     *
     *  @return A possibly mutable view of the coordinates.
     *
     *  @throw None No throw guarantee.
     */
    point_view& point() { return m_point_; }

    /** @brief Returns a view of the Cartesian coordinates of *this.
     *
     *  This method is the same as the non-const version except that it returns
     *  a read-only view. See the documentation for the non-const version for
     *  more details.
     *
     *  @return A read-only reference to the aliased Cartesian coordinates.
     *
     *  @throw std::bad_alloc if allocating the return throws. Strong throw
     *                        guarantee.
     */
    const_point_view point() const { return m_point_; }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if the aliased GridPoint is value equal to @p rhs.
     *
     *  *this is value equal to @p rhs if the:
     *  - aliased weight of *this is value equal to the weight owned by @p rhs
     *  - aliased coordinates in *this are value equal to the coordinates owned
     *    by @p rhs.
     *
     *  Of note, the addresses of the aliased state do NOT need to be the same,
     *  i.e., *this does NOT need to be aliasing @p rhs.
     *
     *  @param[in] rhs The GridPoint object to compare to.
     *
     *  @return True if the GridPoint aliased by *this is value equal to @p rhs
     *          and false otherwise.
     */
    bool operator==(const_grid_point_reference rhs) const noexcept {
        if(weight() != rhs.weight()) return false;
        if(point() != rhs.point()) return false;
        return true;
    }

    /** @brief Determines if the aliased GridPoints are value equal.
     *
     *  @tparam RHSType The cv-qualified type of the GridPoint @p rhs is acting
     *                  like.
     *
     *  This method compares the GridPoint aliased by *this to the GridPoint
     *  aliased by @p rhs. The aliased GridPoint objects are considered value
     *  equal if the:
     *  - weights are value equal
     *  - Cartesian coordinates are value equal.
     *
     *  Of note this comparison does not compare the addresses, i.e., it does
     *  NOT check if *this and @p rhs alias the same GridPoint.
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return False if *this aliases a GridPoint that is value equal to the
     *          GridPoint aliased by @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSType>
    bool operator==(const GridPointView<RHSType>& rhs) const noexcept {
        if(weight() != rhs.weight()) return false;
        if(point() != rhs.point()) return false;
        return true;
    }

    /** @brief Determines if the aliased GridPoint differs from @p rhs.
     *
     *  This method simply negates the result of operator==(GridPoint). See the
     *  description of operator==(GridPoint) for more details.
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return False if *this aliases a GridPoint that is value equal to @p rhs
     *          and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const_grid_point_reference rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Determines if the aliased GridPoints differ.
     *
     *  @tparam RHSType The cv-qualified type of the GridPoint @p rhs is acting
     *                  like.
     *
     *  This method simply negates the result of operator==(GridPointView). See
     *  the description of operator==(GridPointView) for more details.
     *
     *  @param[in] rhs The object being compared to *this.
     *
     *  @return False if *this aliases a GridPoint that is value equal to the
     *          GridPoint aliased by @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSType>
    bool operator!=(const GridPointView<RHSType>& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Type used to alias a GridPoint's weight
    using weight_pointer = typename grid_point_traits::weight_pointer;

    /// The aliased piece of *this storing the weight
    weight_pointer m_pweight_;

    /// The state of *this acting like a point_type
    point_view m_point_;
};

/** @brief Determines if a GridPoint is equal to a view of a GridPoint.
 *
 *  @related GridPointView
 *
 *  @tparam T The cv-qualified type of the aliased GridPoint.
 *
 *  This method behaves identical to GridPointView::operator==(GridPoint)
 *  except that the GridPoint object is on the left side of the operator. See
 *  the description for GridPointView::operator==(GridPoint) for more details.
 *
 *  @param[in] lhs The GridPoint being compared.
 *  @param[in] rhs The aliased GridPoint being compared.
 *
 *  @return True if @p lhs and @p rhs are value equal and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename T>
bool operator==(const GridPoint& lhs, const GridPointView<T>& rhs) {
    return rhs == lhs;
}

/** @brief Determines if a GridPoint is different from a GridPoint.
 *
 *  @related GridPointView
 *
 *  @tparam T The cv-qualified type of the aliased GridPoint.
 *
 *  This method behaves identical to GridPointView::operator!=(GridPoint)
 *  except that the GridPoint object is on the left side of the operator. See
 *  the description for GridPointView::operator!=(GridPoint) for more details.
 *
 *  @param[in] lhs The GridPoint being compared.
 *  @param[in] rhs The aliased GridPoint being compared.
 *
 *  @return False if @p lhs and @p rhs are value equal and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename T>
bool operator!=(const GridPoint& lhs, const GridPointView<T>& rhs) {
    return rhs != lhs;
}

} // namespace chemist
