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
#include "chemist/managers/molecule_manager.hpp"
#include "chemist/managers/periodic_table.hpp"
#include "utilities/containers/case_insensitive_map.hpp"

namespace chemist::detail_ {

/**
 * @brief PIMPL to hide details of the MoleculeManager state
 *
 * Molecules should be added to the MoleculeManager through the API defined
 * by the MoleculeManager class. That API redirects calls to this struct.
 */
struct MoleculeManagerPIMPL {
public:
    /// Forward typedefs
    ///@{
    using key_type   = typename MoleculeManager::key_type;
    using value_type = typename MoleculeManager::value_type;
    ///@}

    /// The type of a molecule map, mapping names to Molecules
    using molecule_map = utilities::CaseInsensitiveMap<Molecule>;

    /** @name MoleculeManagerPIMPL Public API
     *
     * @brief Functions comprising the public API of the PIMPL
     */
    ///@{
    /// Deep copy function
    std::unique_ptr<MoleculeManagerPIMPL> clone() const {
        return std::make_unique<MoleculeManagerPIMPL>(*this);
    }

    /**
     * @brief Get the molecule with the specified name
     *
     * @param name Name of the molecule to retrieve
     *
     * @returns Molecule specified by name given
     *
     * @throws std::out_of_range @p name is not an existing molecule name.
     *                           Strong throw guarantee.
     */
    value_type at(const key_type& name) const;

    /**
     * @brief Adds a new molecule
     *
     * @param name Name of molecule to add
     * @param molecule Molecule to add
     *
     * @throw std::runtime_error if molecule with the given name already
     *                           exists. Strong throw guarantee.
     * @throws ??? An exception is thrown in std::map::emplace. Strong throw
     *             guarantee.
     */
    void insert(const key_type& name, const value_type& molecule);
    ///@}

    /**
     * @name Comparison Operators
     *
     * @param rhs MoleculeManagerPIMPL on the right-hand side of the operator
     * @return Truth of comparison operator
     */
    ///@{
    bool operator==(const MoleculeManagerPIMPL& rhs) const;
    bool operator!=(const MoleculeManagerPIMPL& rhs) const;
    ///@}

private:
    molecule_map m_molecule_map;
};

inline typename MoleculeManagerPIMPL::value_type MoleculeManagerPIMPL::at(
  const key_type& name) const {
    if(!m_molecule_map.count(name))
        throw std::out_of_range("No molecule named " + name);

    return m_molecule_map.at(name);
}

inline void MoleculeManagerPIMPL::insert(const key_type& name,
                                         const value_type& molecule) {
    if(m_molecule_map.count(name))
        throw std::runtime_error("Molecule already exists named " + name);

    m_molecule_map.emplace(name, std::move(molecule));
}

inline bool MoleculeManagerPIMPL::operator==(
  const MoleculeManagerPIMPL& rhs) const {
    return m_molecule_map == rhs.m_molecule_map;
}

inline bool MoleculeManagerPIMPL::operator!=(
  const MoleculeManagerPIMPL& rhs) const {
    return !(*this == rhs);
}

} // namespace chemist::detail_
