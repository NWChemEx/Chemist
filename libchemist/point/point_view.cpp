#include "libchemist/point/detail_/point_pimpl.hpp"
#include "libchemist/point/point_view.hpp"

namespace libchemist {

template<typename T>
using pimpl_type = detail_::PointPIMPL<T>;

/* Note that in these implementations const_cast is fine b/c PointView enforces
 * const based off of whether T is const or not.
 */

template<typename T>
PointView<T>::PointView(parent_type& point) :
  PointView(&point.x(), &point.y(), &point.z()) {}

template<typename T>
PointView<T>::PointView(const PointView<T>& rhs) :
  PointView(&const_cast<reference>(rhs.x()), &const_cast<reference>(rhs.y()),
            &const_cast<reference>(rhs.z())) {}

template<typename T>
PointView<T>& PointView<T>::operator=(const PointView<T>& rhs) {
    return *this = std::move(PointView<T>(rhs));
}

template<typename T>
PointView<T>::PointView(T* x, T* y, T* z) :
  m_pimpl_(std::make_unique<pimpl_type<value_type>>(
    const_cast<value_type*>(x), const_cast<value_type*>(y),
    const_cast<value_type*>(z))) {}

template class PointView<double>;
template class PointView<const double>;
template class PointView<float>;
template class PointView<const float>;

} // namespace libchemist
