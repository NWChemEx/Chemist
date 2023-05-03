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

#include "../../point/detail_/point_pimpl.hpp"
#include "cgto_pimpl.hpp"
#include "chemist/basis_set/contracted_gaussian_view.hpp"

namespace chemist {

template<typename T>
ContractedGaussianView<T>::ContractedGaussianView() :
  ContractedGaussianView(ContractedGaussian<std::remove_cv_t<T>>(
    std::make_unique<detail_::CGTOPIMPL<std::remove_cv_t<T>>>(),
    std::make_unique<detail_::PointPIMPL<std::remove_cv_t<T>>>(nullptr, nullptr,
                                                               nullptr))) {}

template class ContractedGaussianView<double>;
template class ContractedGaussianView<const double>;
template class ContractedGaussianView<float>;
template class ContractedGaussianView<const float>;

} // namespace chemist
