#include "../../point/point_pimpl.hpp"
#include "chemist/basis_set/primitive_view.hpp"
#include "primitive_pimpl.hpp"

namespace chemist {

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
  PointView<T, Primitive<std::remove_cv_t<T>>>(Primitive(
    std::make_unique<prim_pimpl_t<std::remove_cv_t<T>>>(
      const_cast<std::remove_cv_t<T>*>(coef),
      const_cast<std::remove_cv_t<T>*>(exp)),
    std::make_unique<point_pimpl_t<std::remove_cv_t<T>>>(
      const_cast<std::remove_cv_t<T>*>(x), const_cast<std::remove_cv_t<T>*>(y),
      const_cast<std::remove_cv_t<T>*>(z)))) {}

template class PrimitiveView<double>;
template class PrimitiveView<const double>;
template class PrimitiveView<float>;
template class PrimitiveView<const float>;

} // namespace chemist
