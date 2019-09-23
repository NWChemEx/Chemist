#include "libchemist/basis_set/primitive/detail_/primitive_pimpl.hpp"
#include "libchemist/basis_set/primitive/primitive_view.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
PrimitiveView<T>::PrimitiveView(T* coef, T* exp, T* x, T* y, T* z) :
  m_pimpl_(std::make_unique < detail_::PrimitivePIMPL<T>(coef, exp),
           std::make_unique < detail_::PointPIMPL<T>(x, y, z)) {}

template class PrimitiveView<double>;
template class PrimitiveView<const double>;
template class PrimitiveView<float>;
template class PrimitiveView<const float>;

} // namespace libchemist
