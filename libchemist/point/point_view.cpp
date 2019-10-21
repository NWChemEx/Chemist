#include "libchemist/point/detail_/point_pimpl.hpp"
#include "libchemist/point/point_view.hpp"

namespace libchemist {

template class PointViewBase<double, Point<double>>;
template class PointViewBase<const double, Point<double>>;
template class PointViewBase<float, Point<float>>;
template class PointViewBase<const float, Point<float>>;

} // namespace libchemist
