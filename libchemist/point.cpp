#include "libchemist/detail_/point_pimpl.hpp"

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
    const Point<T>& const_this = *this;
    return const_cast<reference>(const_this.coord(i));
}

template<typename T>
typename Point<T>::const_reference Point<T>::coord(size_type i) const {
    if(i > 2) {
        std::string msg = "Requested coordinate ";
        msg += std::to_string(i);
        msg += " is not in the range [0, 2).";
        throw std::out_of_range(msg);
    }
    return i == 0 ? m_pimpl_->m_x_.value() :
                    i == 1 ? m_pimpl_->m_y_.value() : m_pimpl_->m_z_.value();
}

template<typename T>
Point<T>::Point(std::unique_ptr<pimpl_type<T>> pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

template<typename T>
pimpl_type<T>& Point<T>::pimpl() noexcept {
    return *m_pimpl_;
}

template<typename T>
const pimpl_type<T>& Point<T>::pimpl() const noexcept {
    return *m_pimpl_;
}

template class Point<double>;
template class Point<float>;

} // namespace libchemist
