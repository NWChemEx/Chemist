#pragma once
#include "LibChemist/Atom.hpp"
#include "LibChemist/BasisSet.hpp"
#include <map>
#include <vector>

namespace LibChemist {

/** \brief This is the main class for storing the atoms in our system.
 *
 *  A molecule is basically a list of atoms and a set of properties relating to
 *  that collection of atoms.  When possible properties are computed via a core
 *  set of properties (e.g. the number of electrons is computed as number of
 *  alpha electrons plus the number of beta electrons)
 */
struct Molecule {
    /// The possible properties a Molecule instance may have
    enum class Properties {
        multiplicity, ///< 2S + 1, where S is the total spin of the molecule
        nalpha, ///< The number of alpha electrons
        nbeta, ///< The number of beta electrons
    };

    /// The type of a Molecule property label
    using key_type = Properties;

    /// The type of a property value
    using property_value_type = double;

    /// The type of an object holding the properties of a molecule instance
    using property_map = std::map<key_type, property_value_type>;

    /// The type of array storing the atoms
    using atom_list_type = std::vector<Atom>;

    /// The list of the molecule's properties and their values
    property_map properties;

    atom_list_type atoms;

    /**
     * @brief Computes the number of electrons in this molecule.
     *
     * We assume that the number of electrons in the molecule is equal to the
     * number of alpha electrons plus the number of beta electrons.
     *
     * @return The sum of the number of alpha and the number of beta electrons.
     *
     * @throw std::out_of_range if either the number of alpha or the number of
     * beta electrons is not set.  Strong throw guarantee.
     *
     * @threading The number of alpha and the number of beta electrons is
     * accessed and a data race may result if those properties are changed
     * concurrently.
     */
    double nelectrons()const{
        return properties.at(Property::nalpha) + properties.at(Property::nbeta);
    }

    BasisSet get_basis(const std::string& name)const;
};

/** \relates SetOfAtoms
 *  \brief Pulls the basis set off of a SetOfAtoms instances and places it
 *  in a more integrals-friendly container.  General contractions will be
 *  expanded out (i.e. exponents will be copied for each shell.
 *
 * \note This function will un-generalize the basis set.  If you want the
 *       basis set to remain general, use get_general_basis instead.
 *
 * \param[in] name The basis set key to get.
 * \param[in] atoms The SetOfAtoms instance to obtain the basis set from.
 *
 * \returns A deep copy of the requested basis set on the atoms.
 *
 */
BasisSet get_basis(const std::string& name, const SetOfAtoms& atoms);

/** \relates SetOfAtoms
 *  \brief Pulls the basis set off of a SetOfAtoms instances and places it
 *  in a more integrals-friendly container.  The resulting basis set still
 *  contains general contractions (if it had any).
 *
 * \note This function will un-generalize the basis set.  If you want the
 *       basis set to remain general, use get_general_basis instead.
 *
 * \param[in] name The basis set key to get.
 * \param[in] atoms The SetOfAtoms instance to obtain the basis set from.
 *
 * \returns A deep copy of the requested basis set on the atoms.
 *
 */
BasisSet get_general_basis(const std::string& name, const SetOfAtoms& atoms);

} // namespace LibChemist
