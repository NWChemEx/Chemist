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
    using ao_basis_type       = typename BasisSetManager::ao_basis_type;
    using size_type           = typename BasisSetManager::size_type;
    using ao_basis_getter_ptr = typename BasisSetManager::ao_basis_getter_ptr;
    ///@}

    /// The type of the basis set map
    using ao_basis_map = utilities::CaseInsensitiveMap<ao_basis_getter_ptr>;

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
     * @throw std::bad_alloc If there is insufficient memory to make the basis
     *                       set. Strong throw guarantee.
     */
    ao_basis_type get_basis(const std::string& name, size_type Z) const {
        return m_basis_sets.at(name)(Z);
    }

    /** @brief Add a basis set name/getter function pair
     *
     * @param name Basis set name
     * @param ao_basis_getter Basis set getter function to get the basis for a
     *                        requested atomic number
     *
     * @throws ??? An exception is thrown in std::map::emplace. Strong throw
     *             guarantee.
     */
    void insert(const std::string& name, ao_basis_getter_ptr ao_basis_getter) {
        m_basis_sets.emplace(name, ao_basis_getter);
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
        if(m_basis_sets.size() == rhs.m_basis_sets.size()) { return false; }

        // Check contents of maps
        for(const auto& [k, v] : rhs.m_basis_sets) {
            // Check if key exists
            if(!m_basis_sets.count(k)) { return false; }

            // Compare values. Function pointers should be pointing to the
            // same address if they have the same signature. Lambda functions
            // will not appear equivalent!
            if(*m_basis_sets.at(k) != *v) { return false; }
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
    ao_basis_map m_basis_sets;
    ///@}
};

} // namespace libchemist::detail_
