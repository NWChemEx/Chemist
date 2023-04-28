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
#include <chemist/nucleus/nuclei.hpp>
#include <vector>

namespace chemist::detail_ {

/** @brief Implements the Nuclei class.
 *
 *  Nuclei is a set of Nucleus objects. Each Nucleus is an atomic number, mass,
 *  and a point charge. This class unpacks the atomic number and mass of each
 *  Nucleus into a contiguous array, and then unpacks the point charge part
 *  into a Charges.
 */
class NucleiPIMPL {
private:
    /// Type of *this
    using my_type = NucleiPIMPL;

public:
    /// Type we are implementing
    using parent_type = Nuclei;

    /// Reuse parent class's types
    ///@{
    using pimpl_pointer   = typename parent_type::pimpl_pointer;
    using value_type      = typename parent_type::value_type;
    using reference       = typename parent_type::reference;
    using const_reference = typename parent_type::const_reference;
    using charge_set_type = typename parent_type::charge_set_type;
    using const_charge_set_reference =
      typename parent_type::const_charge_set_reference;
    using size_type = typename parent_type::size_type;
    ///@}

    /// Reuse Nucleus types
    ///@{
    using atomic_number_type = typename value_type::atomic_number_type;
    using mass_type          = typename value_type::mass_type;
    ///@}

    /// Implements push_back
    void push_back(value_type q) {
        m_Zs_.push_back(q.Z());
        m_mass_.push_back(q.mass());
        m_charges_.push_back(q);
    }

    /// Implements retrieving a reference to a charge
    reference operator[](size_type i) {
        return reference(m_Zs_[i], m_mass_[i], m_charges_[i]);
    }

    /// Implements retrieving a read-only reference to a charge
    const_reference operator[](size_type i) const {
        return const_reference(m_Zs_[i], m_mass_[i], m_charges_[i]);
    }

    /// Implements determining the number of point Nuclei
    size_type size() const noexcept { return m_charges_.size(); }

    /// Allows "upcasting" to a Charges
    const_charge_set_reference as_charges() { return m_charges_; }

    /// Implements comparisons for Nuclei
    bool operator==(const NucleiPIMPL& rhs) const {
        return std::tie(m_charges_, m_Zs_, m_mass_) ==
               std::tie(rhs.m_charges_, rhs.m_Zs_, rhs.m_mass_);
    }

private:
    /// The state associated with being a Charges<T>
    charge_set_type m_charges_;

    /// The atomic number of each nucleus
    std::vector<atomic_number_type> m_Zs_;

    /// The mass of each nucleus
    std::vector<mass_type> m_mass_;
};

} // namespace chemist::detail_
