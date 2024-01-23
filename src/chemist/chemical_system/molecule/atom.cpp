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

#include <chemist/chemical_system/molecule/atom.hpp>

namespace chemist {

using size_type  = typename Atom::size_type;
using coord_type = typename Atom::coord_type;
using name_type  = typename Atom::name_type;
using mass_type  = typename Atom::mass_type;

bool operator==(const Atom& lhs, const Atom& rhs) noexcept {
    if(lhs.nucleus() != rhs.nucleus()) return false;
    return lhs.n_electrons() == rhs.n_electrons();
}

std::ostream& operator<<(std::ostream& os, const Atom& ai) {
    os << ai.name() << std::fixed << std::setprecision(15);
    for(auto c = 0; c < 3; ++c) os << " " << ai.coord(c);
    return os;
}
} // namespace chemist
