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
#include <chemist/experimental/point/point_class.hpp>

namespace chemist::experimental {

/** @brief An object which behaves like a Point, but aliases its coordinates.
 *
 *  A PointSet stores its points as a structure of arrays: one contiguous
 *  array per Cartesian direction. That layout is what integral libraries
 *  expect and what vectorizes, but it means the set has no Point objects in
 *  it to hand out. *this is what bridges the gap. Indexing a PointSet yields
 *  a PointView which aliases one element of each of the set's three
 *  coordinate arrays; the result behaves exactly like a Point --- it has the
 *  same API, inherited from the same PointCommon --- but writing through it
 *  writes into the set.
 *
 *  *this is also useful on its own, as a non-owning handle to a Point.
 *
 *  @tparam PointType A, possibly const-qualified, Point. PointView<Point>
 *                    aliases mutable coordinates; PointView<const Point>
 *                    aliases read-only ones and has no setters at all.
 */
template<typename PointType>
class PointView : public PointCommon<PointView<PointType>, PointType> {
private:
    /// Type *this inherits from
    using base_type = PointCommon<PointView<PointType>, PointType>;

    /// Lets the CRTP base reach coord_
    friend base_type;

    /// Lets the other const-qualification of *this reach m_x_ and friends
    template<typename OtherPointType>
    friend class PointView;

    /// True when @p T is the non-const-qualified point type *this models
    template<typename T>
    static constexpr bool is_mutable_point_v =
      std::is_same_v<std::decay_t<T>, PointType>;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<std::is_const_v<PointType> && !std::is_const_v<T>>;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::const_coord_reference;
    using typename base_type::coord_reference;
    using typename base_type::coord_type;
    using typename base_type::point_type;
    using typename base_type::size_type;
    ///@}

    /// Type of a, possibly read-only, reference to the aliased Point
    using point_reference = typename ChemistClassTraits<PointType>::reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view of @p point.
     *
     *  This ctor is deliberately implicit so that a Point can be passed
     *  wherever a PointView is expected. The result aliases @p point; it does
     *  not copy it, so writing through *this writes into @p point.
     *
     *  @param[in] point The point *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PointView(point_reference point) :
      PointView(point.m_x_.as_view(), point.m_y_.as_view(),
                point.m_z_.as_view()) {}

    /** @brief Creates a view of three already-aliased coordinates.
     *
     *  This ctor is what lets *this alias state which is not owned by a Point
     *  at all, which is how a PointSet hands out its points.
     *
     *  @param[in] x A view aliasing the Cartesian x-coordinate.
     *  @param[in] y A view aliasing the Cartesian y-coordinate.
     *  @param[in] z A view aliasing the Cartesian z-coordinate.
     *
     *  @throw None No throw guarantee.
     */
    PointView(coord_reference x, coord_reference y, coord_reference z) :
      m_x_(std::move(x)), m_y_(std::move(y)), m_z_(std::move(z)) {}

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided; a read-only view can not be widened into a mutable one.
     *
     *  @tparam OtherPointType The point type @p other models. This ctor only
     *                         participates in overload resolution when
     *                         @p other is mutable and *this is not.
     *
     *  @param[in] other The mutable view to convert.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherPointType,
             typename = enable_mutable_to_const_t<OtherPointType>>
    PointView(const PointView<OtherPointType>& other) :
      m_x_(other.m_x_), m_y_(other.m_y_), m_z_(other.m_z_) {}

    /** @brief Creates a view aliasing the same state as @p other.
     *
     *  The copy is shallow: the resulting view aliases the same coordinates
     *  @p other does.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PointView(const PointView& other) = default;

    /** @brief Takes the aliased state from @p other.
     *
     *  @param[in,out] other The view to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @throw None No throw guarantee.
     */
    PointView(PointView&& other) noexcept = default;

    /** @brief Overwrites the coordinates *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's values through into whatever *this is already aliasing,
     *        which is what makes `point_set[i] = some_point` do the expected
     *        thing. Use swap if you want to rebind *this.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherPoint The point type @p rhs models.
     *
     *  @param[in] rhs The point whose coordinates should be copied into the
     *                 state *this aliases.
     *
     *  @return *this, after overwriting the aliased coordinates.
     *
     *  @throw std::runtime_error if the coordinates of @p rhs are not holding
     *                            the same concrete floating-point types as
     *                            the ones *this aliases. Weak throw
     *                            guarantee.
     */
    template<typename OtherDerived, typename OtherPoint>
    PointView& operator=(const PointCommon<OtherDerived, OtherPoint>& rhs) {
        static_assert(!std::is_const_v<PointType>,
                      "Can not assign through a read-only PointView.");
        detail_::assign(m_x_, rhs.get_x());
        detail_::assign(m_y_, rhs.get_y());
        detail_::assign(m_z_, rhs.get_z());
        return *this;
    }

    /** @brief Overwrites the coordinates *this aliases with @p rhs's.
     *
     *  This is the copy assignment operator. Like the templated assignment
     *  above, and unlike the copy ctor, it is a deep assignment: it writes
     *  through to the aliased state rather than rebinding *this. Assignment
     *  and construction deliberately differ here, because that is what makes
     *  `point_set[i] = point_set[j]` move a point rather than quietly
     *  rebinding a temporary and doing nothing. Use swap to rebind.
     *
     *  This method only exists when *this aliases mutable state; instantiating
     *  it for a read-only view is a compilation error.
     *
     *  @param[in] rhs The view whose values should be copied into the state
     *                 *this aliases.
     *
     *  @return *this, after overwriting the aliased coordinates.
     *
     *  @throw std::runtime_error if the concrete floating-point types do not
     *                            match. Weak throw guarantee.
     */
    PointView& operator=(const PointView& rhs) {
        static_assert(!std::is_const_v<PointType>,
                      "Can not assign through a read-only PointView.");
        if(this != &rhs) {
            detail_::assign(m_x_, rhs.m_x_);
            detail_::assign(m_y_, rhs.m_y_);
            detail_::assign(m_z_, rhs.m_z_);
        }
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~PointView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a Point holding a copy of the aliased coordinates.
     *
     *  Use this when a copy really is wanted. The resulting Point owns its
     *  coordinates and is completely decoupled from what *this aliases.
     *
     *  @return A Point value equal to *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    point_type as_point() const {
        return point_type(detail_::copy(m_x_), detail_::copy(m_y_),
                          detail_::copy(m_z_));
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @param[in,out] other The view to exchange aliases with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(PointView& other) noexcept {
        m_x_.swap(other.m_x_);
        m_y_.swap(other.m_y_);
        m_z_.swap(other.m_z_);
    }

private:
    /** @brief Implements the CRTP base's coordinate access.
     *
     *  @param[in] q Which coordinate is wanted. The CRTP base has already
     *               checked that this is in the range [0, 3).
     */
    coord_reference coord_(size_type q) const {
        if(q == 0) return m_x_;
        if(q == 1) return m_y_;
        return m_z_;
    }

    /// Aliases the Cartesian x-coordinate
    coord_reference m_x_;

    /// Aliases the Cartesian y-coordinate
    coord_reference m_y_;

    /// Aliases the Cartesian z-coordinate
    coord_reference m_z_;
};

/// Type of a view of a mutable Point
using point_view = PointView<Point>;

/// Type of a view of a read-only Point
using const_point_view = PointView<const Point>;

/** @note Only the mutable instantiation is declared here. Explicitly
 *        instantiating PointView<const Point> would force the compiler to
 *        instantiate its copy-assignment operator, which deliberately does
 *        not compile for a read-only view. The read-only instantiation is
 *        covered by the unit tests instead.
 */
extern template class PointView<Point>;

} // namespace chemist::experimental
