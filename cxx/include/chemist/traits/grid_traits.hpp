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
#include <chemist/traits/chemist_class_traits.hpp>
#include <tensorwrapper/tensorwrapper.hpp>
#include <wtf/wtf.hpp>

namespace chemist {

class GridPoint;
template<typename GridPointType>
class GridPointView;
class Grid;
template<typename GridType>
class GridView;

template<>
struct ChemistClassTraits<GridPoint> {
    using value_type             = GridPoint;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using view_type              = GridPointView<value_type>;
    using const_view_type        = GridPointView<const value_type>;
    using weight_type            = wtf::fp::Float;
    using weight_reference       = wtf::fp::FloatView<weight_type>;
    using const_weight_reference = wtf::fp::FloatView<const weight_type>;
    using coord_type             = wtf::fp::Float;
    using coord_reference        = wtf::fp::FloatView<coord_type>;
    using const_coord_reference  = wtf::fp::FloatView<const coord_type>;
};

template<>
struct ChemistClassTraits<const GridPoint> {
    using value_type             = GridPoint;
    using reference              = const value_type&;
    using const_reference        = const value_type&;
    using view_type              = GridPointView<const value_type>;
    using const_view_type        = GridPointView<const value_type>;
    using weight_type            = wtf::fp::Float;
    using weight_reference       = wtf::fp::FloatView<const weight_type>;
    using const_weight_reference = wtf::fp::FloatView<const weight_type>;
    using coord_type             = wtf::fp::Float;
    using coord_reference        = wtf::fp::FloatView<const coord_type>;
    using const_coord_reference  = wtf::fp::FloatView<const coord_type>;
};

template<>
struct ChemistClassTraits<Grid> {
    using value_type        = Grid;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using view_type         = GridView<value_type>;
    using const_view_type   = GridView<const value_type>;
    using grid_point_type   = GridPoint;
    using grid_point_traits = ChemistClassTraits<grid_point_type>;
    using buffer_type       = tensorwrapper::Tensor;
};

template<>
struct ChemistClassTraits<const Grid> {
    using value_type        = Grid;
    using reference         = const value_type&;
    using const_reference   = const value_type&;
    using view_type         = GridView<const value_type>;
    using const_view_type   = GridView<const value_type>;
    using grid_point_type   = GridPoint;
    using grid_point_traits = ChemistClassTraits<const grid_point_type>;
    using buffer_type       = tensorwrapper::Tensor;
};

} // namespace chemist
