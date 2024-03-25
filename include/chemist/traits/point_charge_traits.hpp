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
    using reference              = PointChargeView<value_type>;
    using const_reference        = PointChargeView<const value_type>;
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
    using reference              = PointChargeView<const value_type>;
    using const_reference        = PointChargeView<const value_type>;
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
    using reference           = ChargesView<value_type>;
    using const_reference     = ChargesView<const value_type>;
    using point_charge_traits = ChemistClassTraits<PointCharge<T>>;
    using point_set_traits    = ChemistClassTraits<PointSet<T>>;
};

template<typename T>
struct ChemistClassTraits<const Charges<T>> {
    using value_type          = Charges<T>;
    using reference           = ChargesView<const value_type>;
    using const_reference     = ChargesView<const value_type>;
    using point_charge_traits = ChemistClassTraits<const PointCharge<T>>;
    using point_set_traits    = ChemistClassTraits<const PointSet<T>>;
};

} // namespace chemist
