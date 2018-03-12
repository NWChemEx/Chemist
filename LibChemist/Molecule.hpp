#pragma once
#include "LibChemist/Atom.hpp"
#include "LibChemist/BasisSet.hpp"
#include <map>
#include <vector>
#include <string>

namespace LibChemist {

/** @brief This is the main class for storing the atoms in our system.
 *
 *  In electronic structure theory one typically thinks of the molecular system
 *  as a set of nuclei and a set of electrons such that the latter is smeared
 *  out over the former.  The Atom class models the nuclei of the atoms (as
 *  well as related concepts like point charges and ghost atoms).  The Molecule
 *  class stores these Atom instances as well as properties relating to the
 *  electrons that move about among the Atom instances.
 */
struct Molecule {
    /// The possible properties a Molecule instance may have
    enum class Property {
        multiplicity, ///< 2S + 1, where S is the total spin of the molecule
        nalpha, ///< The number of alpha electrons
        nbeta, ///< The number of beta electrons
    };

    /// The type of a Molecule property label is enum-like
    using property_key = Property;

    /// The type of a property value is POD floating-point type.
    using property_value = double;

    /// The type of a container associating a property with its value.
    using property_map = std::map<property_key, property_value>;

    /// The type of a container of atoms, satisfies random-access container
    using atom_container = std::vector<Atom>;

    /// The list of the molecule's properties and their values
    property_map properties;

    /// The list of atoms in the class
    atom_container atoms;

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
    double nelectrons() const {
        return properties.at(Property::nalpha) + properties.at(Property::nbeta);
    }

    /**
     * @brief Computes and returns the charge of the molecule
     *
     * The charge of the molecule is assumed to be the sum of charges of each
     * atom less the number of electrons.
     *
     * @note This function will account for the charge of point charges too.
     *
     * @return The total computed charge (in a.u.).
     * @throw std::out_of_range if an atom doesn't
     */
    double charge() const ;

    /**
     *  @brief Returns the selected basis set.
     *
     *  For a particular Molecule instance the basis shells reside on the
     *  atoms.  This function will aggregate those shells into a new BasisSet
     *  instance.  If the user desires, the general contractions can be
     *  retained.
     *
     * @param[in] name The basis set to get.  @p name is case insensitive.
     * @param[in] ungeneralize If true general contractions will be expanded out
     *            into a series of shells, rather than treated as one shell.
     *            Default is true.
     * @param[in] atoms The SetOfAtoms instance to obtain the basis set from.
     *
     * @returns A deep copy of the requested basis set on the atoms.
     *
     */
    BasisSet get_basis(const std::string& name, bool ungeneralize = true) const;
};

} // end namespace LibChemist
