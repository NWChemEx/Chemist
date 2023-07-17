/*
 * Copyright 2023 NWChemEx-Project
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

#pragma once
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_traits.hpp>

namespace chemist::basis_set {

/** @brief Propagates types associated with a Shell of type @p ShellType.
 *
 *  @tparam ShellType expected to be either an instantiation of Shell or
 *                    ShellView.
 */
template<typename ShellType>
struct ShellTraits
  : public ContractedGaussianTraits<typename ShellType::reference> {
    using cg_type = typename ShellType::value_type;

    using cg_reference = typename ShellType::reference;

    using const_cg_reference = typename ShellType::const_reference;

    using angular_momentum_type = typename ShellType::angular_momentum_type;

    using angular_momentum_reference =
      typename ShellType::angular_momentum_reference;

    using const_angular_momentum_reference =
      typename ShellType::const_angular_momentum_reference;

    using pure_type = typename ShellType::pure_type;

    using pure_reference = typename ShellType::pure_reference;

    using const_pure_reference = typename ShellType::const_pure_reference;
};

} // namespace chemist::basis_set