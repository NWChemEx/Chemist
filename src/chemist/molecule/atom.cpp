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

#include "chemist/molecule/atom.hpp"

namespace chemist {

using size_type  = typename Atom::size_type;
using coord_type = typename Atom::coord_type;
using name_type  = typename Atom::name_type;
using mass_type  = typename Atom::mass_type;

bool operator==(const Atom& lhs, const Atom& rhs) noexcept {
    using charge_type       = typename Atom::charge_type;
    using point_charge_type = PointCharge<charge_type>;

    const point_charge_type& lhs_point_charge = lhs;
    const point_charge_type& rhs_point_charge = rhs;

    return (lhs.name() == rhs.name()) && (lhs.Z() == rhs.Z()) &&
           (lhs.mass() == rhs.mass()) && (lhs_point_charge == rhs_point_charge);
}

std::ostream& operator<<(std::ostream& os, const Atom& ai) {
    os << ai.name() << std::fixed << std::setprecision(15);
    for(auto c = 0; c < 3; ++c) os << " " << ai.coord(c);
    return os;
}
} // namespace chemist
