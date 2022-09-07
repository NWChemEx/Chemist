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

#include "chemist/wavefunction/nonrelativistic.hpp"

namespace chemist::wavefunction {

#define NONRELATIVISTIC Nonrelativistic<BasisType>

template<typename BasisType>
NONRELATIVISTIC::Nonrelativistic(basis_set_type ref, spin_type spin) :
  Nonrelativistic(std::make_shared<BasisType>(std::move(ref)), spin) {}

template<typename BasisType>
NONRELATIVISTIC::Nonrelativistic(const_basis_set_pointer pref, spin_type spin) :
  m_pbasis_(pref), m_spin_(spin) {}

template<typename BasisType>
typename NONRELATIVISTIC::const_basis_set_reference NONRELATIVISTIC::basis_set()
  const {
    if(m_pbasis_) return *m_pbasis_;
    throw std::runtime_error("Basis set has not been set.");
}

template<typename BasisType>
bool NONRELATIVISTIC::operator==(const Nonrelativistic& rhs) const {
    if(m_spin_ != rhs.m_spin_) return false;
    if(m_pbasis_) {
        if(!rhs.m_pbasis_) return false;
        return basis_set() == rhs.basis_set();
    } else {
        if(rhs.m_pbasis_) return false;
    }
    return true;
}

#undef NONRELATIVISTIC

template class Nonrelativistic<Determinant>;
template class Nonrelativistic<DerivedActiveSpace>;
template class Nonrelativistic<CanonicalDeterminant>;
template class Nonrelativistic<SparseDeterminant>;
template class Nonrelativistic<SparseToTDeterminant>;

} // namespace chemist::wavefunction
