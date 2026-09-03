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
#include <chemist/grid/grid_point.hpp>
#include <chemist/traits/grid_traits.hpp>

namespace chemist {

/** @brief Aliases existing state so it can be used like a GridPoint object.
 *
 *  @tparam GridPointType The cv-qualified type of the GridPoint object *this
 *                        behaves like.
 *
 *  The GridPointView class enables aliased data to be used as if it were
 *  stored in a GridPoint object. The weight and each coordinate are aliased
 *  via wtf::fp::FloatView objects, which means *this can alias state owned by
 *  a GridPoint object directly, or state owned by a type-erased buffer (e.g.,
 *  the buffers backing a Grid object).
 */
template<typename GridPointType>
class GridPointView {
private:
    /// Struct defining the types for the GridPoint *this will act like.
    using grid_point_traits = ChemistClassTraits<GridPointType>;

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

    /// Type of a possibly mutable, aliasing view of the weight
    using weight_reference = typename grid_point_traits::weight_reference;

    /// Type of a read-only, aliasing view of the weight
    using const_weight_reference =
      typename grid_point_traits::const_weight_reference;

    /// Type of a possibly mutable, aliasing view of a coordinate
    using coord_reference = typename grid_point_traits::coord_reference;

    /// Type of a read-only, aliasing view of a coordinate
    using const_coord_reference =
      typename grid_point_traits::const_coord_reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view that aliases an existing GridPoint object.
     *
     *  This ctor is used to alias an existing GridPoint object.
     *
     *  @param[in] other The object to alias.
     *
     *  @throw std::bad_alloc if allocating state for *this fails. Strong
     *                        throw guarantee.
     */
    GridPointView(grid_point_reference other) :
      GridPointView(other.m_weight_, other.m_x_, other.m_y_, other.m_z_) {}

    /** @brief Creates a view that aliases existing state.
     *
     *  This ctor is used to create a view that aliases state, regardless of
     *  whether that state is owned by a GridPoint object or not.
     *
     *  @param[in] weight A view aliasing the weight of the grid point.
     *  @param[in] x A view aliasing the x-coordinate of the grid point.
     *  @param[in] y A view aliasing the y-coordinate of the grid point.
     *  @param[in] z A view aliasing the z-coordinate of the grid point.
     *
     *  @throw None No throw guarantee.
     */
    GridPointView(weight_reference weight, coord_reference x, coord_reference y,
                  coord_reference z) :
      m_weight_(std::move(weight)),
      m_x_(std::move(x)),
      m_y_(std::move(y)),
      m_z_(std::move(z)) {}

    /** @brief Copies an existing view.
     *
     *  This ctor creates a new view which aliases the same state as
     *  @p other, i.e., the resulting view is a shallow copy of the aliased
     *  state.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if allocating the state for the copy fails.
     *                        Strong throw guarantee.
     */
    GridPointView(const GridPointView& other) = default;

    /** @brief Takes the state from another view.
     *
     *  @param[in,out] other The view to take the state from. After this
     *                       operation @p other is in a valid but otherwise
     *                       undefined state.
     *
     *  @throw None No throw guarantee.
     */
    GridPointView(GridPointView&& other) noexcept = default;

    /** @brief Assigns a copy of a view to *this.
     *
     *  After this operation *this aliases the same state that @p rhs aliases.
     *
     *  @param[in] rhs The view to copy.
     *
     *  @return A mutable reference to *this after replacing the state with a
     *          copy of @p rhs's state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state for
     *                        the copy. Strong throw guarantee.
     */
    GridPointView& operator=(const GridPointView& rhs) = default;

    /** @brief Transfers the state of @p rhs to *this.
     *
     *  @param[in,out] rhs The object to take the state from. After this
     *                     operation @p rhs will be in a valid but otherwise
     *                     undefined state.
     *
     *  @return *this after taking ownership of the state in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    GridPointView& operator=(GridPointView&& rhs) noexcept = default;

    /** @brief Sets the aliased state to the state of @p rhs.
     *
     *  @tparam RHSType The cv-qualified type of the GridPoint whose value is
     *                  being assigned. @p RHSType must not be cv-qualified
     *                  for this method to participate in overload
     *                  resolution.
     *
     *  Unlike copy assignment (which causes *this to alias the same state
     *  that @p rhs aliases), this method writes @p rhs's weight/coordinates
     *  into the state currently aliased by *this.
     *
     *  @param[in] rhs The GridPoint object whose value should be written into
     *                 the state aliased by *this.
     *
     *  @return The current instance after writing @p rhs's value into the
     *          aliased state.
     *
     *  @throw std::runtime_error if the concrete floating-point type of
     *                            @p rhs's weight/coordinates does not match
     *                            that of the state aliased by *this. Strong
     *                            throw guarantee.
     */
    template<typename RHSType, typename = enable_if_assign_from_point<RHSType>>
    GridPointView& operator=(RHSType&& rhs) {
        m_weight_ = rhs.m_weight_;
        m_x_      = rhs.m_x_;
        m_y_      = rhs.m_y_;
        m_z_      = rhs.m_z_;
        return *this;
    }

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    /** @brief Returns the weight aliased by *this.
     *
     *  @return A read-only view of the aliased weight.
     *
     *  @throw None No throw guarantee.
     */
    const_weight_reference get_weight() const { return m_weight_; }

    /** @brief Sets the weight aliased by *this to @p value.
     *
     *  @tparam T The concrete floating-point type of @p value. Must satisfy
     *            the chemist::concepts::FloatingPoint concept.
     *
     *  Unlike assigning through a reference, this method mutates the aliased
     *  weight in place (i.e., it does not disturb what *this aliases).
     *
     *  @param[in] value The new value for the aliased weight.
     *
     *  @throw std::runtime_error if @p T does not match the concrete
     *                            floating-point type currently aliased by
     *                            *this. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T>
    void set_weight(T value) {
        m_weight_ = value;
    }

    /** @brief Returns the x-coordinate aliased by *this (read-only). */
    const_coord_reference get_x() const { return m_x_; }

    /** @brief Sets the x-coordinate aliased by *this to @p value. */
    template<concepts::FloatingPoint T>
    void set_x(T value) {
        m_x_ = value;
    }

    /** @brief Returns the y-coordinate aliased by *this (read-only). */
    const_coord_reference get_y() const { return m_y_; }

    /** @brief Sets the y-coordinate aliased by *this to @p value. */
    template<concepts::FloatingPoint T>
    void set_y(T value) {
        m_y_ = value;
    }

    /** @brief Returns the z-coordinate aliased by *this (read-only). */
    const_coord_reference get_z() const { return m_z_; }

    /** @brief Sets the z-coordinate aliased by *this to @p value. */
    template<concepts::FloatingPoint T>
    void set_z(T value) {
        m_z_ = value;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Determines if the aliased GridPoint is value equal to @p rhs.
     *
     *  *this is value equal to @p rhs if the aliased weight and coordinates
     *  of *this are value equal to the weight and coordinates owned by
     *  @p rhs.
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
        if(get_weight() != rhs.get_weight()) return false;
        if(get_x() != rhs.get_x()) return false;
        if(get_y() != rhs.get_y()) return false;
        if(get_z() != rhs.get_z()) return false;
        return true;
    }

    /** @brief Determines if the aliased GridPoints are value equal.
     *
     *  @tparam RHSType The cv-qualified type of the GridPoint @p rhs is acting
     *                  like.
     *
     *  This method compares the GridPoint aliased by *this to the GridPoint
     *  aliased by @p rhs. Of note this comparison does not compare the
     *  addresses, i.e., it does NOT check if *this and @p rhs alias the same
     *  GridPoint.
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
        if(get_weight() != rhs.get_weight()) return false;
        if(get_x() != rhs.get_x()) return false;
        if(get_y() != rhs.get_y()) return false;
        if(get_z() != rhs.get_z()) return false;
        return true;
    }

    /** @brief Determines if the aliased GridPoint differs from @p rhs. */
    bool operator!=(const_grid_point_reference rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Determines if the aliased GridPoints differ. */
    template<typename RHSType>
    bool operator!=(const GridPointView<RHSType>& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// The aliased weight of *this
    weight_reference m_weight_;

    /// The aliased x-coordinate of *this
    coord_reference m_x_;

    /// The aliased y-coordinate of *this
    coord_reference m_y_;

    /// The aliased z-coordinate of *this
    coord_reference m_z_;
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
