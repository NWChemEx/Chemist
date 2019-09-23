#include "libchemist/basis_set/primitive/detail_/primitive_pimpl.hpp"
#include "libchemist/basis_set/primitive/primitive.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
Primitive<T>::Primitive() : m_pimpl_(std::make_unique<pimpl_t>()), Point<T>() {}

template<typename T>
Primitive<T>::Primitive(T coef, T exp, T x, T y, T z) :
  m_pimpl_(std::make_unique<pimpl_t>(coef, exp)),
  Point<T>(x, y, z) {}

template<typename T>
Primitive<T>::Primitive(const Primitive<T>& rhs) :
  Primitive(rhs.coefficient(), rhs.exponent(), rhs.x(), rhs.y(), rhs.z()) {}

template<typename T>
Primitive<T>::Primitive(Primitive<T>&& rhs) noexcept = default;

template<typename T>
Primitive<T>& Primitive<T>::operator=(const Primitive<T>& rhs) noexcept {
    Point<T>::operator=(rhs);
    m_pimpl_->coef()  = rhs.coefficient();
    m_pimpl_->exp()   = rhs.exponent();
    return *this;
}

template<typename T>
Primitive<T>& Primitive<T>::operator=(Primitive<T>&& rhs) noexcept = default;

template<typename T>
Primitive<T>::Primitive(
  std::unique_ptr<pimpl_t> my_pimpl,
  std::unique_ptr<detail_::PointPIMPL<T>> point_pimpl) noexcept :
  m_pimpl_(std::move(my_pimpl)),
  Point<T>(std::move(point_pimpl)) {}

template<typename T>
Primitive<T>::~Primitive() noexcept = default;

template<typename T>
typename Primitive<T>::reference Primitive<T>::coefficient() noexcept {
    return m_pimpl_->coef();
}

template<typename T>
typename Primitive<T>::const_reference Primitive<T>::exponent() const noexcept {
    return m_pimpl_->exp();
}

template<typename T>
typename Primitive<T>::reference Primitive<T>::exponent() noexcept {
    return m_pimpl_->exp();
}

template<typename T>
typename Primitive<T>::const_reference Primitive<T>::coefficient() const
  noexcept {
    return m_pimpl_->coef();
}

template class Primitive<double>;
template class Primitive<float>;

} // namespace libchemist
