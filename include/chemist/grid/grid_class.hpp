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
#include <chemist/point/point_set.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

/** @brief Models an ordered list of GridPoint objects.
 *
 *  A grid is an ordered set of grid points, where each grid point has
 *  coordinates (presently described in Cartesian coordinates) and a weight.
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

    /// Struct defining the types for a PointSet
    using point_set_traits = typename traits_type::point_set_traits;

public:
    /// Type storing the Cartesian coordinates of each grid point
    using point_set_type = typename traits_type::point_set_type;

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
     * This ctor creates an empty Grid object. GridPoints can be added using
     * push_back.
     *
     * @throw None no throw guarantee
     */
    Grid() = default;

    /** @brief Creates a Grid from a range of GridPoint objects.
     *
     *  @tparam BeingItr The type of the iterator pointing to the first grid
     *                   point.
     *  @tparam EndItr The type of the iterator pointing to just past the last
     *                 grid point.
     *
     *  This ctor will create a new Grid by copying the grid points in the
     *  range [begin, end).
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
    Grid(BeginItr&& begin, EndItr&& end) {
        while(begin != end) {
            m_weights_.push_back(begin->weight());
            m_points_.push_back(begin->point());
            ++begin;
        }
    }

private:
    /// Type used to store the weight of a grid point
    using weight_type = typename value_type::weight_type;

    /// Allows the base to access the implementations of at_ and size_
    friend base_type;

    /// Implements getting a mutable reference to the i-th grid point
    reference at_(size_type i) {
        return reference(m_weights_[i], m_points_[i]);
    }

    /// Implements getting a read-only reference to the i-th grid point
    const_reference at_(size_type i) const {
        return const_reference(m_weights_[i], m_points_[i]);
    }

    /// Implements determining the number of grid points in *this
    size_type size_() const noexcept { return m_weights_.size(); }

    /// Holds the weights of the grid points
    std::vector<weight_type> m_weights_;

    /// Holds the Cartesian coordinates of the grid points.
    point_set_type m_points_;
};

} // namespace chemist
