#include "../../point/point_pimpl.hpp"
#include "chemist/basis_set/primitive.hpp"
#include "primitive_pimpl.hpp"
#include <cassert>

namespace chemist {

#define PRIM_TYPE Primitive<T>

template<typename T>
PRIM_TYPE::Primitive() : Primitive(0.0, 0.0, 0.0, 0.0, 0.0) {}

template<typename T>
PRIM_TYPE::Primitive(T coef, T exp, T x, T y, T z) :
  m_pimpl_(std::make_unique<my_pimpl>(coef, exp)), Point<T>(x, y, z) {}

template<typename T>
PRIM_TYPE::Primitive(const PRIM_TYPE& rhs) :
  Primitive(rhs.coefficient(), rhs.exponent(), rhs.x(), rhs.y(), rhs.z()) {}

template<typename T>
PRIM_TYPE::Primitive(PRIM_TYPE&& rhs) noexcept = default;

template<typename T>
PRIM_TYPE& PRIM_TYPE::operator=(const PRIM_TYPE& rhs) noexcept {
    Point<T>::operator=(rhs);
    m_pimpl_->coef()  = rhs.coefficient();
    m_pimpl_->exp()   = rhs.exponent();
    return *this;
}

template<typename T>
PRIM_TYPE& PRIM_TYPE::operator=(PRIM_TYPE&& rhs) noexcept = default;

template<typename T>
PRIM_TYPE::Primitive(my_pimpl_ptr my_pimpl, base_pimpl_ptr point_pimpl) noexcept
  :
  m_pimpl_(std::move(my_pimpl)), Point<T>(std::move(point_pimpl)) {}

template<typename T>
PRIM_TYPE::~Primitive() noexcept = default;

template<typename T>
typename PRIM_TYPE::reference PRIM_TYPE::coefficient() noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->coef();
}

template<typename T>
typename PRIM_TYPE::const_reference PRIM_TYPE::exponent() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->exp();
}

template<typename T>
typename PRIM_TYPE::reference PRIM_TYPE::exponent() noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->exp();
}

template<typename T>
typename PRIM_TYPE::const_reference PRIM_TYPE::coefficient() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->coef();
}

#undef PRIM_TYPE

template class Primitive<double>;
template class Primitive<float>;

} // namespace chemist
