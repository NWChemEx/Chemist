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

// Warning! This file meant only for inclusion from nucleus.hpp

namespace chemist {

// -- Constructors -------------------------------------------------------------

inline Nucleus::Nucleus(name_type name, atomic_number_type Z, mass_type m) :
  Nucleus(std::move(name), Z, m, 0.0, 0.0, 0.0) {}

inline Nucleus::Nucleus(name_type name, atomic_number_type Z, mass_type m,
                        coord_type x, coord_type y, coord_type z) :
  Nucleus(std::move(name), Z, m, x, y, z, (charge_type)Z) {}

inline Nucleus::Nucleus(name_type name, atomic_number_type Z, mass_type m,
                        coord_type x, coord_type y, coord_type z,
                        charge_type q) :
  point_charge_type(q, x, y, z),
  m_name_(std::move(name)),
  m_Z_(Z),
  m_mass_(m) {}

// -- Comparisons --------------------------------------------------------------

inline bool Nucleus::operator==(const Nucleus& rhs) const noexcept {
    if(static_cast<const point_charge_type&>(*this) != rhs) return false;
    return std::tie(m_name_, m_Z_, m_mass_) ==
           std::tie(rhs.m_name_, rhs.m_Z_, rhs.m_mass_);
}

inline bool Nucleus::operator!=(const Nucleus& rhs) const noexcept {
    return !(*this == rhs);
}

// -- Serialization ------------------------------------------------------------

template<typename Archive>
void Nucleus::save(Archive& ar) const {
    point_charge_type::save(ar);
    ar& m_name_;
    ar& m_Z_;
    ar& m_mass_;
}

template<typename Archive>
void Nucleus::load(Archive& ar) {
    point_charge_type::load(ar);
    ar& m_name_;
    ar& m_Z_;
    ar& m_mass_;
}

} // namespace chemist
