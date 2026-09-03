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
#include <chemist/grid/grid_point_view.hpp>
#include <chemist/traits/grid_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

/** @brief Models an ordered list of GridPoint objects.
 *
 *  A grid is an ordered set of grid points, where each grid point has
 *  coordinates (presently described in Cartesian coordinates) and a weight.
 *
 *  Internally the weights and coordinates of the grid points are stored in
 *  two tensorwrapper::Tensor objects (one of shape (N) for the weights, one
 *  of shape (N, 3) for the coordinates). Since Tensor type-erases its scalar
 *  type (via WeaklyTypedFloat), a Grid can hold values of any WTF-registered
 *  floating-point type.
 *
 *  @note This class does not attempt to enforce the set-like nature of the
 *  grid (i.e., it will allow duplicate grid points).
 *
 */
class Grid : public utilities::IndexableContainerBase<Grid> {
private:
    /// Type *this inherits from
    using base_type = utilities::IndexableContainerBase<Grid>;

    /// Struct defining the types for *this
    using traits_type = ChemistClassTraits<Grid>;

    /// Struct defining the types for a GridPoint
    using grid_point_traits = typename traits_type::grid_point_traits;

public:
    /// Type used to store the weights/coordinates of the grid points
    using buffer_type = typename traits_type::buffer_type;

    /// Type of a grid point
    using value_type = typename traits_type::grid_point_type;

    /// Type acting like a mutable reference to a grid point
    using reference = typename grid_point_traits::view_type;

    /// Type acting like a read-only reference to a grid point
    using const_reference = typename grid_point_traits::const_view_type;

    // -------------------------------------------------------------------------
    // -- Ctors
    // -------------------------------------------------------------------------

    /** @brief Creates and empty grid object.
     *
     * This ctor creates an empty Grid object.
     *
     * @throw None no throw guarantee
     */
    Grid() = default;

    /** @brief Creates a Grid by wrapping already-populated weight and
     *         coordinate buffers.
     *
     *  This is the ctor that all other (non-default) ctors ultimately
     *  dispatch to; it is the only ctor whose implementation is not
     *  templated on the caller's iterator/range types.
     *
     *  @param[in] weights A buffer holding the weight of each grid point
     *                     (size N).
     *  @param[in] coords A buffer holding the flattened Cartesian
     *                    coordinates of each grid point (size 3N, i.e.,
     *                    x0, y0, z0, x1, y1, z1, ...).
     *
     *  @throw std::invalid_argument if the size of @p coords is not exactly
     *                               three times the size of @p weights.
     *                               Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    Grid(wtf::buffer::FloatBuffer weights, wtf::buffer::FloatBuffer coords);

    /** @brief Creates a Grid from a range of GridPoint objects.
     *
     *  @tparam BeingItr The type of the iterator pointing to the first grid
     *                   point.
     *  @tparam EndItr The type of the iterator pointing to just past the last
     *                 grid point.
     *
     *  This ctor will create a new Grid by copying the grid points in the
     *  range [begin, end). It simply flattens the range into a pair of
     *  buffers and then dispatches to the (non-templated) buffer ctor to do
     *  the actual work.
     *
     *  @param[in,out] begin An iterator pointing to the first grid point that
     *                       should be in the Grid. If @p begin was passed by
     *                       reference then after the call @p begin will compare
     *                       equal to @p end
     *  @param[in] end An iterator pointing to just past the last grid point
     *                 that should be included in the newly created grid.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        *this. Strong throw guarantee.
     */
    template<typename BeginItr, typename EndItr>
    Grid(BeginItr&& begin, EndItr&& end) :
      Grid(from_range_(std::forward<BeginItr>(begin),
                       std::forward<EndItr>(end))) {}

private:
    /// Allows the base to access the implementations of at_ and size_
    friend base_type;

    /** @brief Flattens a range of GridPoint objects into a Grid.
     *
     *  This is the only piece of *this's logic which must remain templated
     *  (it needs to work with arbitrary iterator types). As soon as the two
     *  buffers are populated it hands off to the non-templated buffer ctor.
     */
    template<typename BeginItr, typename EndItr>
    static Grid from_range_(BeginItr begin, EndItr end) {
        wtf::buffer::FloatBuffer weights;
        wtf::buffer::FloatBuffer coords;
        for(; begin != end; ++begin) {
            weights.push_back(begin->get_weight());
            coords.push_back(begin->get_x());
            coords.push_back(begin->get_y());
            coords.push_back(begin->get_z());
        }
        return Grid(std::move(weights), std::move(coords));
    }

    /// Implements getting a mutable reference to the i-th grid point
    reference at_(size_type i);

    /// Implements getting a read-only reference to the i-th grid point
    const_reference at_(size_type i) const;

    /// Implements determining the number of grid points in *this
    size_type size_() const noexcept;

    /// The number of grid points in *this
    size_type m_size_ = 0;

    /// Holds the weights of the grid points (shape (N))
    buffer_type m_weights_;

    /// Holds the Cartesian coordinates of the grid points (shape (N, 3))
    buffer_type m_points_;
};

} // namespace chemist
