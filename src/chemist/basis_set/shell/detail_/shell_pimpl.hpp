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

#pragma once
#include <chemist/basis_set/shell/shell.hpp>
#include <chemist/basis_set/shell/shell_traits.hpp>

namespace chemist::basis_set::detail_ {

template<typename CGType>
class ShellPIMPL {
public:
    /// Type *this implements
    using parent_type = Shell<CGType>;

    /// Traits class with all the types for a Shell
    using shell_traits = ShellTraits<parent_type>;

    /// Default, no state
    ShellPIMPL() noexcept = default;

    /// With provided purity, angular momentum, and contracted gaussian
    ShellPIMPL(typename shell_traits::pure_type pure,
               typename shell_traits::angular_momentum_type l,
               typename shell_traits::cg_type cg) noexcept :
      m_cg(std::move(cg)), m_l(std::move(l)), m_pure(std::move(pure)) {}

    /// Compare values for two shells for equality
    bool operator==(const ShellPIMPL& rhs) const noexcept {
        auto lstate = std::tie(m_cg, m_l, m_pure);
        auto rstate = std::tie(rhs.m_cg, rhs.m_l, rhs.m_pure);
        return lstate == rstate;
    }

    /// The contracted gaussian associated with this shell
    typename shell_traits::cg_type m_cg;

    /// The total angular momentum of this shell
    typename shell_traits::angular_momentum_type m_l = 0;

    /// Whether the shell is pure or not
    typename shell_traits::pure_type m_pure = shell_traits::pure_type::pure;
};

} // namespace chemist::basis_set::detail_