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
#include <chemist/chemical_system/molecule/molecule.hpp>

namespace chemist::detail_ {

class MolPIMPL {
public:
    using parent_type = Molecule;

    /// Pull in parent types
    ///@{
    using nuclei_type            = typename parent_type::nuclei_type;
    using nuclei_reference       = typename parent_type::nuclei_reference;
    using const_nuclei_reference = typename parent_type::const_nuclei_reference;
    using size_type              = typename parent_type::size_type;
    using charge_type            = typename parent_type::charge_type;
    ///@}

    nuclei_reference nuclei() { return m_nuclei_; }
    const_nuclei_reference nuclei() const { return m_nuclei_; }

    /// Charge
    charge_type m_charge = 0;

    /// Multiplicity
    size_type m_mult = 1;

private:
    /// The nuclei
    nuclei_type m_nuclei_;
};

} // namespace chemist::detail_
