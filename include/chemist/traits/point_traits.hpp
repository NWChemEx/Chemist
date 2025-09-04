/*
 * Copyright 2024 NWChemEx-Project
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

namespace chemist {
template<typename T>
class Point;
template<typename PointType>
class PointView;
template<typename T>
class PointSet;
template<typename PointSetType>
class PointSetView;

template<typename T>
struct ChemistClassTraits<Point<T>> {
    using value_type            = Point<T>;
    using reference             = value_type&;
    using const_reference       = const value_type&;
    using view_type             = PointView<value_type>;
    using const_view_type       = PointView<const value_type>;
    using coord_type            = T;
    using coord_reference       = coord_type&;
    using const_coord_reference = const coord_type&;
    using coord_pointer         = coord_type*;
    using const_coord_pointer   = const coord_type*;
};

template<typename T>
struct ChemistClassTraits<const Point<T>> {
    using value_type            = Point<T>;
    using reference             = const value_type&;
    using const_reference       = const value_type&;
    using view_type             = PointView<const value_type>;
    using const_view_type       = PointView<const value_type>;
    using coord_type            = T;
    using coord_reference       = const coord_type&;
    using const_coord_reference = const coord_type&;
    using coord_pointer         = const coord_type*;
    using const_coord_pointer   = const coord_type*;
};

template<typename T>
struct ChemistClassTraits<PointSet<T>> {
    using value_type      = PointSet<T>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = PointSetView<value_type>;
    using const_view_type = PointSetView<const value_type>;
    using point_type      = Point<T>;
    using point_traits    = ChemistClassTraits<point_type>;
};

template<typename T>
struct ChemistClassTraits<const PointSet<T>> {
    using value_type      = PointSet<T>;
    using reference       = const value_type&;
    using const_reference = const value_type&;
    using view_type       = PointSetView<const value_type>;
    using const_view_type = PointSetView<const value_type>;
    using point_type      = Point<T>;
    using point_traits    = ChemistClassTraits<const point_type>;
};

} // namespace chemist
