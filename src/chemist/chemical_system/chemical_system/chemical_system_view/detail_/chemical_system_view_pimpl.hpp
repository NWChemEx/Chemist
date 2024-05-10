/*
 * Copyright 2024 NWChemEx-Project
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
#include <chemist/chemical_system/chemical_system/chemical_system_view/chemical_system_view.hpp>

namespace chemist::detail_ {

/** @brief Defines the API for implementing ChemicalSystemView
 *
 *  @tparam ChemicalSystemType The type of the ChemicalSystem *this aliases.
 *                             Assumed to be either `ChemicalSystem` or
 *                             `const ChemicalSystem`.
 *
 *  As of right now there is only a single PIMPL for the ChemicalSystemView,
 *  but if others are needed this class will become a base class that defines
 *  the API all PIMPLs must implement.
 *
 */
template<typename ChemicalSystemType>
class ChemicalSystemViewPIMPL {
private:
    /// Type of *this
    using my_type = ChemicalSystemViewPIMPL<ChemicalSystemType>;

public:
    /// Type *this aliases
    using parent_type = ChemicalSystemView<ChemicalSystemType>;

    /// Pull in parent's types
    ///@{
    using pimpl_pointer      = typename parent_type::pimpl_pointer;
    using molecule_reference = typename parent_type::molecule_reference;
    using const_molecule_reference =
      typename parent_type::const_molecule_reference;
    ///@}

    explicit ChemicalSystemViewPIMPL(molecule_reference molecule) :
      m_molecule_(std::move(molecule)) {}

    /** @brief Initializes *this to a copy of @p rhs such that *this willl
     *         alias the same ChemicalSystem
     *
     *  @param[in] rhs The object to copy.
     *
     *  @throw std::bad_alloc if there is a problem copying @p rhs. Strong
     *                        throw guarantee.
     *
     */
    ChemicalSystemViewPIMPL(const ChemicalSystemViewPIMPL& rhs) = default;

    /// Will be implemented generically by the main class
    ///@{
    ChemicalSystemViewPIMPL(ChemicalSystemViewPIMPL&&) noexcept = delete;
    ChemicalSystemViewPIMPL& operator=(const ChemicalSystemViewPIMPL&) = delete;
    ChemicalSystemViewPIMPL& operator=(ChemicalSystemViewPIMPL&&) = delete;
    ///@}

    /// Mutable reference to the Molecule stored in *this
    molecule_reference molecule() { return m_molecule_; }

    /// Read-only reference to the Molecule stored in *this
    const_molecule_reference molecule() const { return m_molecule_; }

    /// Makes a (possibly eventually polymorphic) deep copy of *this
    pimpl_pointer clone() const { return std::make_unique<my_type>(*this); }

    /// Compares the Molecule in *thi to the one in @p rhs
    bool operator==(const ChemicalSystemViewPIMPL& rhs) const noexcept {
        return m_molecule_ == rhs.m_molecule_;
    }

private:
    /// An alias of the Molecule piece of *this
    molecule_reference m_molecule_;
};

} // namespace chemist::detail_
