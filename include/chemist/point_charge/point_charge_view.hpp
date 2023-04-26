#pragma once
#include <chemist/detail_/view/traits.hpp>
#include <chemist/point/point_view2.hpp>
#include <chemist/point_charge/point_charge.hpp>
#include <type_traits>

namespace chemist {

template<typename ChargeType>
class PointChargeView
  : public PointView2<typename detail_::ViewTraits<ChargeType>::apply_const<
      typename ChargeType::point_type>> {
private:
    using traits_type = detail_::ViewTraits<ChargeType>;

public:
    /// Type of an un-qualified PointCharge object
    using point_charge_type = typename traits_type::type;

    /// Type of an un-qualified Point object
    using point_type = typename point_charge_type::point_type;

    /// Type used to store the point charge's charge
    using charge_type = typename point_charge_type::charge_type;

    /// Type used of a reference to the charge
    using charge_reference = typename traits_type::apply_const_ref<charge_type>;

    /// Type of *this if passed as a PointView
    using point_reference =
      PointView2<typename traits_type::apply_const<point_type>>;

    /// Type of a reference to one of the base's coordinates
    using coord_reference = typename point_reference::coord_reference;

    PointChargeView(charge_reference q, point_reference r) :
      point_reference(r), m_pq_(&q) {}

    PointChargeView(charge_reference q, coord_reference x, coord_reference y,
                    coord_reference z) :
      point_reference(x, y, z), m_pq_(&q) {}

private:
    /// The type of a pointer used to alias the charge
    using internal_pointer = typename traits_type::apply_const_ptr<charge_type>;

    internal_pointer m_pq_;
};

} // namespace chemist
