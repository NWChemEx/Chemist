#pragma once
#include <chemist/traits/grid_traits.hpp>

namespace chemist {

template<typename GridPointType>
class GridPointView {
private:
    using grid_point_traits = ChemistClassTraits<GridPointType>;
    using point_traits      = typename grid_point_traits::point_traits;

    /** @brief Determines if @p T is the same as GridPointType
     *
     *  @tparam T The type to compare to GridPointType
     *
     *  This variable will be set to true if @p T is the same type as
     *  GridPointType and if GridPointType is not cv-qualified (if it is
     *  cv-qualified, std::decay_t is not able to match it).
     */
    template<typename T>
    static constexpr bool is_grid_point_type =
      std::is_same_v<std::decay_t<T>, GridPointType>;

    template<typename T>
    using enable_if_assign_from_point = std::enable_if_t<is_grid_point_type<T>>;

public:
    /// Type of a non-CV qualified GridPoint
    using grid_point_type = typename grid_point_traits::value_type;

    /// Type of a reference to a possibly cv-qualified grid_point_type object
    using grid_point_reference = typename grid_point_traits::reference;

    /// Type of a reference to a const-qualified grid_point_type
    using const_grid_point_reference =
      typename grid_point_traits::const_reference;

    /// Type of a reference to a possibly cv-qualified grid point's weight
    using weight_reference = typename grid_point_traits::weight_reference;

    /// Type of a read-only reference to a grid point's weight
    using const_weight_reference =
      typename grid_point_traits::const_weight_reference;

    /// Type of a view to a possibly cv-qualified point_type object
    using point_view = typename point_traits::view_type;

    /// Type of a read-only view to a point_type object
    using const_point_view = typename point_traits::const_view_type;

    GridPointView(grid_point_reference other) :
      GridPointView(other.weight(), other.point()) {}

    GridPointView(weight_reference weight, point_view point) :
      m_pweight_(&weight), m_point_(std::move(point)) {}

    template<typename RHSType, typename = enable_if_assign_from_point<RHSType>>
    GridPointView& operator=(RHSType&& rhs) {
        m_pweight_ = &rhs.weight();
        m_point_   = rhs.point();
        return *this;
    }

    weight_reference weight() { return *m_pweight_; }

    const_weight_reference weight() const { return *m_pweight_; }

    point_view point() { return m_point_; }

    const_point_view point() const { return m_point_; }

    template<typename RHSType>
    bool operator==(const GridPointView<RHSType>& rhs) const noexcept {
        return std::tie(weight(), point()) ==
               std::tie(rhs.weight(), rhs.point());
    }

    template<typename RHSType>
    bool operator!=(const GridPointView<RHSType>& rhs) const noexcept {
        return !(*this == rhs);
    }

public:
    using weight_pointer = typename grid_point_traits::weight_pointer;

    weight_pointer m_pweight_;

    point_view m_point_;
};

template<typename T>
bool operator==(const GridPoint& lhs, const GridPointView<T>& rhs) {
    return rhs == lhs;
}

template<typename T>
bool operator!=(const GridPoint& lhs, const GridPointView<T>& rhs) {
    return rhs != lhs;
}

} // namespace chemist