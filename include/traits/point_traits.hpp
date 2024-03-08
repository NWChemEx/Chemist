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
class Point;
template<typename PointType>
class PointView;
class PointSet;
template<typename PointSetType>
class PointSetView;

template<>
struct ChemistClassTraits<Point> {
    using value_type      = Point;
    using reference       = PointView<Point>;
    using const_reference = PointView<const Point>;
};

template<>
struct ChemistClassTraits<const Point> {
    using value_type      = Point;
    using reference       = PointView<const Point>;
    using const_reference = PointView<const CLASS_NAME>;
};

template<>
struct ChemistClassTraits<PointSet> {
    using value_type      = PointSet;
    using reference       = PointSetView<PointSet>;
    using const_reference = PointSetView<const PointSet>;
    using point_traits    = ChemistClassTraits<Point>;
};

template<>
struct ChemistClassTraits<const PointSet> {
    using value_type      = PointSet;
    using reference       = PointSetView<const PointSet>;
    using const_reference = PointSetView<const PointSet>;
    using point_traits    = ChemistClassTraits<const Point>;
};

} // namespace chemist

#undef CHEMIST_CLASS_TRAITS
