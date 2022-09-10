/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
