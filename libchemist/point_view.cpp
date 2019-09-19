#include "libchemist/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
using pimpl_type = detail_::PointPIMPL<T>;

template<typename T, typename... Args>
static auto make_pimpl(Args&&... args) {
    return std::make_unique<pimpl_type<T>>(std::forward<Args>(args)...);
}

template<typename T>
PointView<T>::PointView() : m_pimpl_(make_pimpl<T>()) {}

template<typename T>
PointView<T>::PointView(T* x, T* y, T* z) : m_pimpl_(make_pimpl<T>(x, y, z)) {}

template<typename T>
PointView<T>::~PointView<T>() noexcept = default;

template class PointView<double>;
template class PointView<float>;

} // namespace libchemist
