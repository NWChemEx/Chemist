#include "libchemist/basis_set/primitive/detail_/primitive_pimpl.hpp"
#include "libchemist/basis_set/primitive/primitive_view.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
using prim_pimpl_t = detail_::PrimitivePIMPL<std::remove_const_t<T>>;

template<typename T>
using point_pimpl_t = detail_::PointPIMPL<std::remove_const_t<T>>;

/* Same comments apply hear that applied to PointView<T>. It is okay to
 * const_cast internally because the public API of the PrimitiveView class takes
 * care of the const-correctness.
 */

template<typename T>
PrimitiveView<T>::PrimitiveView(T* coef, T* exp, T* x, T* y, T* z) :
  m_pimpl_(std::make_unique<prim_pimpl_t<value_type>>(
             const_cast<value_type*>(coef), const_cast<value_type*>(exp)),
           std::make_unique<point_pimpl_t<value_type>>(
             const_cast<value_type*>(x), const_cast<value_type*>(y),
             const_cast<value_type*>(z))) {}

template<typename T>
PrimitiveView<T>::PrimitiveView(const PrimitiveView<T>& rhs) :
  PrimitiveView(const_cast<T*>(&rhs.coefficient()),
                const_cast<T*>(&rhs.exponent()), const_cast<T*>(&rhs.x()),
                const_cast<T*>(&rhs.y()), const_cast<T*>(&rhs.z())) {}

template class PrimitiveView<double>;
template class PrimitiveView<const double>;
template class PrimitiveView<float>;
template class PrimitiveView<const float>;

} // namespace libchemist
