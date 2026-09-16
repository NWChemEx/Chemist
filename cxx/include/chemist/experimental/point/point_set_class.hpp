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
#include <chemist/experimental/point/point_set_common.hpp>
#include <chemist/types/floating_point.hpp>
#include <initializer_list>

namespace chemist::experimental {

/** @brief An ordered set of points in three-dimensional Cartesian space.
 *
 *  Points are almost never encountered alone. They are encountered in sets,
 *  and the layout of that set --- not the layout of any individual point ---
 *  is what determines whether downstream code can be vectorized or handed
 *  directly to an external library.
 *
 *  The  PointSet class decouples the abstraction of "a set of points" from
 *  how that data is actually stored.
 *
 *  @note The PointSet class currently stores the coordinates of the points in
 *        three separate arrays, one for each Cartesian direction. Additional
 *        layouts can be added by wrapping the state in a PIMPL. Doing so is
 *        left until another layout is actually needed.
 */
class PointSet : public PointSetCommon<PointSet, PointSet> {
private:
    /// Type *this inherits from
    using base_type = PointSetCommon<PointSet, PointSet>;

    /// Lets the CRTP base reach buffer_
    friend base_type;

    /// Lets a view alias *this's coordinate arrays directly
    template<typename PointSetType>
    friend class PointSetView;

    /** @brief The concrete types push_back is allowed to widen a buffer to.
     *
     *  wtf::buffer::FloatBuffer::push_back defaults its TupleType to
     *  wtf::default_fp_types, which is float/double/long double only. That
     *  would silently exclude every WTF-registered type this class supports,
     *  e.g., Sigma's uncertainty-quantification types. Naming chemist's list
     *  explicitly is what keeps them working.
     */
    using fp_types = chemist::types::floating_point_types;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::buffer_reference;
    using typename base_type::buffer_type;
    using typename base_type::const_buffer_reference;
    using typename base_type::const_reference;
    using typename base_type::reference;
    using typename base_type::size_type;
    using typename base_type::value_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates an empty set.
     *
     *  @throw None No throw guarantee.
     */
    PointSet() noexcept = default;

    /** @brief Creates a set holding @p points.
     *
     *  @param[in] points The points the new set should hold.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PointSet(std::initializer_list<value_type> points) :
      PointSet(points.begin(), points.end()) {}

    /** @brief Creates a set holding the points in the range [@p begin,
     *         @p end).
     *
     *  The range may hold Point objects or views of them; anything the
     *  `get_*` accessors work on will do.
     *
     *  @tparam BeginItr The type of the iterator to the first point.
     *  @tparam EndItr The type of the iterator to just past the last point.
     *
     *  @param[in] begin An iterator pointing to the first point to add.
     *  @param[in] end An iterator pointing to just past the last point to
     *                 add.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    PointSet(BeginItr&& begin, EndItr&& end) {
        for(auto itr = begin; itr != end; ++itr) push_back(*itr);
    }

    /** @brief Creates a set which takes ownership of three coordinate arrays.
     *
     *  This is the ctor to use when the coordinates have already been
     *  assembled elsewhere, e.g., read out of a file, since it avoids going
     *  through Point objects entirely.
     *
     *  @param[in] x The x-coordinate of every point, in order.
     *  @param[in] y The y-coordinate of every point, in order.
     *  @param[in] z The z-coordinate of every point, in order.
     *
     *  @throw std::invalid_argument if @p x, @p y, and @p z are not all the
     *                               same length. Strong throw guarantee.
     */
    PointSet(buffer_type x, buffer_type y, buffer_type z);

    /** @brief Creates a deep copy of @p other.
     *
     *  @param[in] other The set to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    PointSet(const PointSet& other) = default;

    /** @brief Takes ownership of @p other's coordinate arrays.
     *
     *  @param[in,out] other The set to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @throw None No throw guarantee.
     */
    PointSet(PointSet&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other.
     *
     *  @param[in] other The set to copy.
     *
     *  @return *this, after overwriting its state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    PointSet& operator=(const PointSet& other) = default;

    /** @brief Overwrites *this with @p other's state.
     *
     *  @param[in,out] other The set to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @return *this, after overwriting its state.
     *
     *  @throw None No throw guarantee.
     */
    PointSet& operator=(PointSet&& other) noexcept = default;

    /// Default, no-throw dtor
    ~PointSet() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Container modifiers
    // -------------------------------------------------------------------------

    /** @brief Adds @p point to the end of *this.
     *
     *  Each of @p point's coordinates is appended to the corresponding
     *  coordinate array, which is amortized constant time.
     *
     *  @warning This may reallocate the coordinate arrays and therefore
     *           invalidates every PointView and raw pointer previously
     *           obtained from *this.
     *
     *  @tparam PointType The type of the point being added. May be an owning
     *                    Point or a view of one.
     *
     *  @param[in] point The point to add.
     *
     *  @throw std::bad_alloc if there is a problem growing the coordinate
     *                        arrays. Weak throw guarantee.
     *  @throw std::runtime_error if @p point's coordinates are not holding
     *                            the same concrete floating-point type as the
     *                            coordinates already in *this. Weak throw
     *                            guarantee.
     */
    template<typename PointType>
    void push_back(const PointType& point) {
        auto x = point.get_x();
        auto y = point.get_y();
        auto z = point.get_z();
        m_x_.template push_back<fp_types>(x);
        m_y_.template push_back<fp_types>(y);
        m_z_.template push_back<fp_types>(z);
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @param[in,out] other The set to exchange state with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(PointSet& other) noexcept;

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what PointSetCommon::save wrote, replacing whatever *this
     *  held beforehand.
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
        size_type n{};
        ar(n);
        PointSet buffer;
        for(size_type i = 0; i < n; ++i) {
            value_type p;
            p.load(ar);
            buffer.push_back(p);
        }
        swap(buffer);
    }

private:
    /// Implements the CRTP base's mutable coordinate array access
    buffer_reference buffer_(size_type q);

    /// Implements the CRTP base's read-only coordinate array access
    const_buffer_reference buffer_(size_type q) const;

    /// Holds the x-coordinate of every point in *this, in order
    buffer_type m_x_;

    /// Holds the y-coordinate of every point in *this, in order
    buffer_type m_y_;

    /// Holds the z-coordinate of every point in *this, in order
    buffer_type m_z_;
};

} // namespace chemist::experimental
