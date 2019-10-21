#include "libchemist/point/detail_/point_pimpl.hpp"
#include "libchemist/point/point.hpp"

namespace libchemist {

template<typename T>
using pimpl_type = detail_::PointPIMPL<T>;

template<typename T, typename... Args>
static auto make_pimpl(Args&&... args) {
    return std::make_unique<pimpl_type<T>>(std::forward<Args>(args)...);
}

template<typename T>
Point<T>::Point() : Point(make_pimpl<T>()) {}

template<typename T>
Point<T>::Point(Point<T>&& rhs) noexcept = default;

template<typename T>
Point<T>& Point<T>::operator=(const Point<T>& rhs) noexcept {
    x() = rhs.x();
    y() = rhs.y();
    z() = rhs.z();
    return *this;
}

template<typename T>
Point<T>& Point<T>::operator=(Point<T>&& rhs) noexcept = default;

template<typename T>
Point<T>::Point(T x, T y, T z) : Point(make_pimpl<T>(x, y, z)) {}

template<typename T>
Point<T>::~Point<T>() noexcept = default;

template<typename T>
typename Point<T>::reference Point<T>::coord(size_type i) {
    return m_pimpl_->coord(i);
}

template<typename T>
typename Point<T>::const_reference Point<T>::coord(size_type i) const {
    return m_pimpl_->coord(i);
}

template<typename T>
Point<T>::Point(std::unique_ptr<detail_::PointPIMPL<T>> pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

template<typename T>
std::unique_ptr<detail_::PointPIMPL<T>> Point<T>::point_alias() const {
    return std::make_unique<detail_::PointPIMPL<T>>(
      const_cast<T*>(&x()), const_cast<T*>(&y()), const_cast<T*>(&z()));
}

template class Point<double>;
template class Point<float>;

} // namespace libchemist
