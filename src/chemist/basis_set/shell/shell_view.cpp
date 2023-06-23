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
#include "chemist/basis_set/shell_view.hpp"
#include "shell_pimpl.hpp"

namespace chemist {

template<typename T>
ShellView<T>::ShellView(shell_reference shell2alias) :
  ShellView(shell2alias.contracted_gaussian(), shell2alias.l(),
            shell2alias.pure()) {}

template<typename T>
ShellView<T>::ShellView(contracted_gaussian_reference cg,
                        angular_momentum_reference l, pure_reference ao_type) :
  m_cg_(&cg), m_l_(&l), m_pure_(&ao_type) {}

template<typename T>
bool ShellView<T>::operator==(const ShellView& rhs) const noexcept {
    auto my_state  = std::tie(*m_cg_, *m_l_, &m_pure_);
    auto rhs_state = std::tie(*rhs.m_cg_, *rhs.m_l_, *rhs.m_pure_);

    return my_state == rhs_state;
}

template class ShellView<double>;
template class ShellView<const double>;
template class ShellView<float>;
template class ShellView<const float>;

} // namespace chemist
