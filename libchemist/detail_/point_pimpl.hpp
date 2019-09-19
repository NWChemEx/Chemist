#pragma once
#include "libchemist/point.hpp"
#include <utilities/containers/own_or_borrow.hpp>

namespace libchemist::detail_ {

template<typename T>
struct PointPIMPL {
    PointPIMPL() = default;

    template<typename X, typename Y, typename Z>
    PointPIMPL(X&& x, Y&& y, Z&& z);

    utilities::OwnOrBorrow<T> m_x_;
    utilities::OwnOrBorrow<T> m_y_;
    utilities::OwnOrBorrow<T> m_z_;
};

template<typename T>
template<typename X, typename Y, typename Z>
PointPIMPL<T>::PointPIMPL(X&& x, Y&& y, Z&& z) :
  m_x_(std::forward<X>(x)),
  m_y_(std::forward<Y>(y)),
  m_z_(std::forward<Z>(z)) {}

} // namespace libchemist::detail_
