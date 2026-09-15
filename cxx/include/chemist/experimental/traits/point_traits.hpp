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
#include <chemist/traits/chemist_class_traits.hpp>
#include <wtf/wtf.hpp>

namespace chemist::experimental {

class Point;

template<typename PointType>
class PointView;

class PointSet;

template<typename PointSetType>
class PointSetView;

} // namespace chemist::experimental

namespace chemist {

/** @brief Traits for a mutable experimental::Point.
 *
 *  The experimental point component deliberately keeps the concrete
 *  floating-point type out of its API. Coordinates are therefore modeled by
 *  the type-erased wtf::fp::Float class and handed out as wtf::fp::FloatView
 *  objects. Whether the resulting view is mutable or read-only is decided
 *  here, by which specialization of this struct is selected.
 */
template<>
struct ChemistClassTraits<experimental::Point> {
    /// Type of the point *this describes
    using value_type = experimental::Point;

    /// Type of a mutable reference to a point
    using reference = value_type&;

    /// Type of a read-only reference to a point
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a point
    using view_type = experimental::PointView<value_type>;

    /// Type acting like a read-only reference to a point
    using const_view_type = experimental::PointView<const value_type>;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = wtf::fp::Float;

    /// Type of a mutable, aliasing view of a coordinate
    using coord_reference = wtf::fp::FloatView<coord_type>;

    /// Type of a read-only, aliasing view of a coordinate
    using const_coord_reference = wtf::fp::FloatView<const coord_type>;
};

/** @brief Traits for a read-only experimental::Point.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through PointView.
 */
template<>
struct ChemistClassTraits<const experimental::Point> {
    /// Type of the point *this describes
    using value_type = experimental::Point;

    /// Type of a reference to a point (read-only because *this is const)
    using reference = const value_type&;

    /// Type of a read-only reference to a point
    using const_reference = const value_type&;

    /// Type acting like a reference to a point (read-only because *this is
    /// const)
    using view_type = experimental::PointView<const value_type>;

    /// Type acting like a read-only reference to a point
    using const_view_type = experimental::PointView<const value_type>;

    /// Type *this uses to model a single Cartesian coordinate
    using coord_type = wtf::fp::Float;

    /// Type of an aliasing view of a coordinate (read-only because *this is
    /// const)
    using coord_reference = wtf::fp::FloatView<const coord_type>;

    /// Type of a read-only, aliasing view of a coordinate
    using const_coord_reference = wtf::fp::FloatView<const coord_type>;
};

/** @brief Traits for a mutable experimental::PointSet.
 *
 *  A PointSet stores one contiguous, type-erased array per Cartesian
 *  direction. Those arrays are its storage and, handed out as BufferView
 *  objects, are also the escape hatch consumers which need a raw pointer go
 *  through.
 */
template<>
struct ChemistClassTraits<experimental::PointSet> {
    /// Type of the set *this describes
    using value_type = experimental::PointSet;

    /// Type of a mutable reference to a set
    using reference = value_type&;

    /// Type of a read-only reference to a set
    using const_reference = const value_type&;

    /// Type acting like a mutable reference to a set
    using view_type = experimental::PointSetView<value_type>;

    /// Type acting like a read-only reference to a set
    using const_view_type = experimental::PointSetView<const value_type>;

    /// Type of the points in the set
    using point_type = experimental::Point;

    /// Traits of the points in the set
    using point_traits = ChemistClassTraits<point_type>;

    /// Type used to own one Cartesian direction's worth of coordinates
    using buffer_type = wtf::buffer::FloatBuffer;

    /// Type of a mutable, aliasing view of one coordinate array
    using buffer_reference = wtf::buffer::BufferView<wtf::fp::Float>;

    /// Type of a read-only, aliasing view of one coordinate array
    using const_buffer_reference =
      wtf::buffer::BufferView<const wtf::fp::Float>;
};

/** @brief Traits for a read-only experimental::PointSet.
 *
 *  Identical to the mutable specialization except that every reference type
 *  has been const-qualified. This is what propagates const through
 *  PointSetView, all the way down to the PointView objects it hands out.
 */
template<>
struct ChemistClassTraits<const experimental::PointSet> {
    /// Type of the set *this describes
    using value_type = experimental::PointSet;

    /// Type of a reference to a set (read-only because *this is const)
    using reference = const value_type&;

    /// Type of a read-only reference to a set
    using const_reference = const value_type&;

    /// Type acting like a reference to a set (read-only because *this is
    /// const)
    using view_type = experimental::PointSetView<const value_type>;

    /// Type acting like a read-only reference to a set
    using const_view_type = experimental::PointSetView<const value_type>;

    /// Type of the points in the set
    using point_type = experimental::Point;

    /// Traits of the points in the set (read-only because *this is const)
    using point_traits = ChemistClassTraits<const point_type>;

    /// Type used to own one Cartesian direction's worth of coordinates
    using buffer_type = wtf::buffer::FloatBuffer;

    /// Type of an aliasing view of one coordinate array (read-only because
    /// *this is const)
    using buffer_reference = wtf::buffer::BufferView<const wtf::fp::Float>;

    /// Type of a read-only, aliasing view of one coordinate array
    using const_buffer_reference =
      wtf::buffer::BufferView<const wtf::fp::Float>;
};

} // namespace chemist
