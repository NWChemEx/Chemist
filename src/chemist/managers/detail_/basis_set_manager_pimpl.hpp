#pragma once
#include "chemist/managers/basis_set_manager.hpp"
#include "utilities/containers/case_insensitive_map.hpp"

namespace chemist::detail_ {

/**
 * @brief PIMPL to hide details of the BasisSetManager state
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

    /**
     * @name BasisSetManagerPIMPL Public API
     *
     * @brief Functions comprising the public API of the PIMPL
     */
    ///@{
    /// Deep copy function
    std::unique_ptr<BasisSetManagerPIMPL> clone() const {
        return std::make_unique<BasisSetManagerPIMPL>(*this);
    }

    /**
     * @brief Gets the basis for the given atomic number from a basis set
     *
     * @param name Name of the basis set
     * @param Z Atomic number
     *
     * @return Basis for the requested atomic number from the basis set
     *
     * @throw std::out_of_range @p name is not a valid basis set name.
     *                          Strong throw guarantee.
     */
    ao_basis_type get_basis(const std::string& name, size_type Z) const;

    /**
     * @brief Add a basis set name/getter function pair
     *
     * @param name Basis set name
     * @param ao_basis AO basis object
     *
     * @throw std::runtime_error if basis set with the given name already
     *                           exists. Strong throw guarantee.
     * @throw ??? An exception is thrown in std::map::emplace. Strong throw
     *            guarantee.
     */
    void insert(const std::string& name, const ao_basis_map& ao_basis);
    ///@}

    /**
     * @name Comparison Operators
     *
     * @param rhs BasisSetManagerPIMPL on the right-hand side of the operator
     * @return Truth of comparison operator
     */
    ///@{
    bool operator==(const BasisSetManagerPIMPL& rhs) const {
        return m_basis_sets == rhs.m_basis_sets;
    }

    bool operator!=(const BasisSetManagerPIMPL& rhs) const {
        return !((*this) == rhs);
    }
    ///@}

    /**
     * @name BasisSetManager State
     *
     * @brief This section defines the state of the BasisSetManager class.
     */
    ///@{
    /// Map of basis set names to the getter function for each atom supported
    /// by the basis set
    basis_set_map m_basis_sets;
    ///@}
};

inline typename BasisSetManagerPIMPL::ao_basis_type
BasisSetManagerPIMPL::get_basis(const std::string& name, size_type Z) const {
    if(!m_basis_sets.count(name))
        throw std::out_of_range("No basis set named " + name);

    if(!m_basis_sets.at(name).count(Z))
        throw std::out_of_range("Basis set " + name +
                                " does not contain AOs for atomic number " +
                                std::to_string(Z));

    return m_basis_sets.at(name).at(Z);
}

inline void BasisSetManagerPIMPL::insert(const std::string& name,
                                         const ao_basis_map& ao_basis) {
    if(m_basis_sets.count(name))
        throw std::runtime_error("Basis set already exists named " + name);

    m_basis_sets.emplace(name, std::move(ao_basis));
}

} // namespace chemist::detail_
