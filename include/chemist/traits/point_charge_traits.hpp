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
#include <chemist/traits/point_traits.hpp>

namespace chemist {
template<typename T>
class PointCharge;
template<typename T>
class PointChargeView;
template<typename T>
class Charges;
template<typename T>
class ChargesView;

template<typename T>
struct ChemistClassTraits<PointCharge<T>> {
    using value_type             = PointCharge<T>;
    using view_type              = PointChargeView<value_type>;
    using const_view_type        = PointChargeView<const value_type>;
    using charge_type            = T;
    using charge_reference       = charge_type&;
    using const_charge_reference = const charge_type&;
    using charge_pointer         = T*;
    using const_charge_pointer   = const T*;
    using point_traits           = ChemistClassTraits<Point<double>>;
};

template<typename T>
struct ChemistClassTraits<const PointCharge<T>> {
    using value_type             = PointCharge<T>;
    using view_type              = PointChargeView<const value_type>;
    using const_view_type        = PointChargeView<const value_type>;
    using charge_type            = T;
    using charge_reference       = const charge_type&;
    using const_charge_reference = const charge_type&;
    using charge_pointer         = T*;
    using const_charge_pointer   = const T*;
    using point_traits           = ChemistClassTraits<const Point<double>>;
};

template<typename T>
struct ChemistClassTraits<Charges<T>> {
    using value_type          = Charges<T>;
    using reference           = Charges<T>&;
    using const_reference     = const Charges<T>&;
    using view_type           = ChargesView<value_type>;
    using const_view_type     = ChargesView<const value_type>;
    using point_charge_traits = ChemistClassTraits<PointCharge<T>>;
    using point_set_traits    = ChemistClassTraits<PointSet<T>>;
};

template<typename T>
struct ChemistClassTraits<const Charges<T>> {
    using value_type          = Charges<T>;
    using reference           = const Charges<T>&;
    using const_reference     = const Charges<T>&;
    using view_type           = ChargesView<const value_type>;
    using const_view_type     = ChargesView<const value_type>;
    using point_charge_traits = ChemistClassTraits<const PointCharge<T>>;
    using point_set_traits    = ChemistClassTraits<const PointSet<T>>;
};

} // namespace chemist
