#pragma once

/**
 * @file LibChemist.hpp
 *
 * This is a convenience header defining the public API of the LibChemist
 * library.  It should NOT be included in any other LibChemist header file,
 * source file, or test (the exception being TestLibChemist, which tests the
 * functions defined in this header file).
 */
#include "LibChemist/BasisSetManager.hpp" //Brings in AOBasisSet and AOShell
#include "LibChemist/MoleculeManager.hpp" //Brings in Molecule and Atom
#include "LibChemist/PeriodicTable.hpp"

/**
 * @brief The primary namespace for the LibChemist library
 *
 * All functionality of the LibChemist library intended for end user consumption
 * is defined within this namespace.
 *
 */
namespace LibChemist {

/**
 * @brief Convenience function for applying a basis set to a molecule
 *
 * @params[in] name The name of the basis set to apply
 * @param[in] mol The molecule instance to apply the basis set to.
 * @param[in] man The BasisSetManager instance to read the basis set from.
 *            Defaults to a default constructed variant.
 * @return The basis set resulting from applying it to @p mol.
 * @throw std::bad_alloc if there is insufficient memory to create the basis
 *        set.  Strong throw guarantee.
 */
inline AOBasisSet apply_basis(const std::string& name, const Molecule& mol,
                              const BasisSetManager& man = BasisSetManager()) {
    AOBasisSet rv;
    for(const auto& ai : mol) {
        for(auto si : man.get_basis(name, ai.Z())) {
            si.center() = ai.coords();
            rv.push_back(si);
        }
    }
    return rv;
}

} // namespace LibChemist
