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
