#pragma once
#include <chemist/grid/grid_point.hpp>
#include <chemist/grid/grid_point_view.hpp>
#include <chemist/point/point_set.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

class Grid : utilities::IndexableContainerBase<Grid> {
private:
    using base_type         = utilities::IndexableContainerBase<Grid>;
    using traits_type       = ChemistClassTraits<Grid>;
    using grid_point_traits = typename traits_type::grid_point_traits;
    using point_set_traits  = typename traits_type::point_set_traits;

public:
    using point_set_type = typename traits_type::point_set_type;

    using value_type      = typename traits_type::grid_point_type;
    using reference       = typename grid_point_traits::view_type;
    using const_reference = typename grid_point_traits::const_view_type;
    using weight_type     = typename value_type::weight_type;

    template<typename BeginItr, typename EndItr>
    Grid(BeginItr&& begin, EndItr&& end) {
        while(begin != end) {
            m_weights_.push_back(begin->weight());
            m_points_.push_back(begin->point());
        }
    }

private:
    friend base_type;

    reference at_(size_type i);
    const_reference at_(size_type i) const;
    size_type size_() const noexcept { return m_weights_.size(); }

    std::vector<weight_type> m_weights_;

    point_set_type m_points_;
};

} // namespace chemist