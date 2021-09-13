#pragma once
#include "libchemist/managers/basis_set_manager.hpp"
#include "utilities/containers/case_insensitive_map.hpp"

namespace libchemist::detail_ {

/** @brief PIMPL to hide details of the BasisSetManager state
 *
 * Basis sets should be added to the BasisSetManager through the API defined
 * by the BasisSetManager class. That API redirects calls to this struct.
 */
struct BasisSetManagerPIMPL {
    /// Forward typedefs from main class
    ///@{
    using ao_basis_type = typename BasisSetManager::ao_basis_type;
    using size_type     = typename BasisSetManager::size_type;
    using ao_basis_map  = typename BasisSetManager::ao_basis_map;
    ///@}

    /// The type of the basis set map, mapping names to AO basis maps
    using basis_set_map = utilities::CaseInsensitiveMap<ao_basis_map>;

    /** @name BasisSetManagerPIMPL Public API
     *
     * @brief Functions comprising the public API of the PIMPL
     */
    ///@{
    /// Deep copy function
    std::unique_ptr<BasisSetManagerPIMPL> clone() const {
        return std::make_unique<BasisSetManagerPIMPL>(*this);
    }

    /** @brief Gets the basis for the given atomic number from a basis set
     *
     * @param name Name of the basis set
     * @param Z Atomic number
     *
     * @returns Basis for the requested atomic number from the basis set
     *
     * @throws std::out_of_range @p name is not a valid basis set name.
     *                           Strong throw guarantee.
     */
    ao_basis_type get_basis(const std::string& name, size_type Z) const {
        return m_basis_sets.at(name).at(Z);
    }

    /** @brief Add a basis set name/getter function pair
     *
     * @param name Basis set name
     * @param ao_basis AO basis object
     *
     * @throws ??? An exception is thrown in std::map::emplace. Strong throw
     *             guarantee.
     */
    void insert(const std::string& name, ao_basis_map ao_basis) {
        m_basis_sets.emplace(name, ao_basis);
    }
    ///@}

    ///@{
    /** @name Comparison Operators
     *
     * @param rhs BasisSetManagerPIMPL on the right-hand side of the operator
     * @returns
     */
    bool operator==(const BasisSetManagerPIMPL& rhs) const {
        // Quick size check
        if(m_basis_sets.size() != rhs.m_basis_sets.size()) { return false; }

        // Check contents of each basis set map
        for(const auto& [name, basis_map] : rhs.m_basis_sets) {
            // Check if key exists
            if(m_basis_sets.count(name) == 0) { return false; }

            // Compare the AO basis maps
            // Start with another quick size check
            if(m_basis_sets.at(name).size() != basis_map.size()) {
                return false;
            }

            // Compare the AO bases in the map
            for(const auto& [Z, ao_basis] : basis_map) {
                // Check if key exists
                if(m_basis_sets.at(name).count(Z) == 0) { return false; }

                // Compare the AO bases themselves
                if(m_basis_sets.at(name).at(Z) != ao_basis) { return false; }
            }
        }

        return true;
    }

    bool operator!=(const BasisSetManagerPIMPL& rhs) const {
        return !((*this) == rhs);
    }
    ///@}

    /** @name BasisSetManager State
     *
     * @brief This section defines the state of the BasisSetManager class.
     */
    ///@{
    /// Map of basis set names to the getter function for each atom supported
    /// by the basis set
    basis_set_map m_basis_sets;
    ///@}
};

} // namespace libchemist::detail_
