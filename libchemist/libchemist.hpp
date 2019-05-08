#pragma once

/**
 * @file libchemist.hpp
 *
 * This is a convenience header defining the public API of the libchemist
 * library.  It should NOT be included in any other libchemist header file,
 * source file, or test (the exception being Testlibchemist, which tests the
 * functions defined in this header file).
 */
#include "libchemist/basis_set_manager.hpp" //Brings in AOBasisSet and AOShell
#include "libchemist/molecule_manager.hpp"  //Brings in Molecule and Atom
#include "libchemist/periodic_table.hpp"

/**
 * @brief The primary namespace for the libchemist library
 *
 * All functionality of the libchemist library intended for end user consumption
 * is defined within this namespace.
 *
 */
namespace libchemist {

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

} // namespace libchemist
