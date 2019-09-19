#include "libchemist/detail_/point_pimpl.hpp"
#include "libchemist/point.hpp"

namespace libchemist {

template<typename T>
using pimpl_type = detail_::PointPIMPL<T>;

template<typename T, typename... Args>
static auto make_pimpl(Args&&... args) {
    return std::make_unique<pimpl_type<T>>(std::forward<Args>(args)...);
}

template<typename T>
Point<T>::Point() : m_pimpl_(make_pimpl<T>()) {}

template<typename T>
Point<T>::Point(T x, T y, T z) : m_pimpl_(make_pimpl<T>(x, y, z)) {}

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

template class Point<double>;
template class Point<float>;

} // namespace libchemist
