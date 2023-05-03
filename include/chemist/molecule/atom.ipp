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

// WARNING: This file meant only for inclusion from atom.hpp

namespace chemist {

// -- Constructors -------------------------------------------------------------

inline Atom::Atom(name_type s, atomic_number_type Z, mass_type m, coord_type x,
                  coord_type y, coord_type z) :
  m_nuke_(s, Z, m, x, y, z) {}

inline Atom::Atom(name_type s, atomic_number_type Z, mass_type m, coord_type x,
                  coord_type y, coord_type z, charge_type q) :
  m_nuke_(s, Z, m, x, y, z, q) {}

// -- Accessors ----------------------------------------------------------------

inline typename Atom::const_coord_reference Atom::coord(size_type q) const {
    return nucleus().coord(q);
}

inline typename Atom::const_charge_reference Atom::charge() const noexcept {
    return nucleus().charge();
}

// -- Serialization ------------------------------------------------------------

template<typename Archive>
void Atom::save(Archive& ar) const {
    ar& m_nuke_;
}

template<typename Archive>
void Atom::load(Archive& ar) {
    ar& m_nuke_;
}
} // namespace chemist
