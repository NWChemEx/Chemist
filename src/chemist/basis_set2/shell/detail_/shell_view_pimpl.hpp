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
#include <chemist/basis_set2/shell/shell_traits.hpp>
#include <chemist/basis_set2/shell/shell_view.hpp>

namespace chemist::basis_set::detail_ {

template<typename ShellType>
class ShellViewPIMPL {
public:
    /// Type *this is implementing
    using parent_type = ShellView<ShellType>;

    /// Pull in types from the parent class
    using shell_traits = ShellTraits<parent_type>;

    // With provided purity, angular momentum, and contracted gaussian
    ShellViewPIMPL(typename shell_traits::pure_reference pure,
                   typename shell_traits::angular_momentum_reference l,
                   typename shell_traits::cg_reference cg) :
      m_pure(pure), m_l(l), m_cg(std::move(cg)) {}

    /// Compare *this and another shell for equality
    bool operator==(const ShellViewPIMPL& rhs) const noexcept {
        auto lstate = std::tie(m_pure, m_l, m_cg);
        auto rstate = std::tie(rhs.m_pure, rhs.m_l, rhs.m_cg);
        return lstate == rstate;
    }

    /// Whether the shell is pure or not
    typename shell_traits::pure_reference m_pure;

    /// The total angular momentum of this shell
    typename shell_traits::angular_momentum_reference m_l;

    /// The contracted gaussian associated with this shell
    typename shell_traits::cg_reference m_cg;
};

} // namespace chemist::basis_set::detail_