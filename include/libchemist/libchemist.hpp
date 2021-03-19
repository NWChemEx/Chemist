#pragma once

/**
 * @file libchemist.hpp
 *
 * This is a convenience header defining the public API of the libchemist
 * library.  It should NOT be included in any other libchemist header file,
 * source file, or test (the exception being Testlibchemist, which tests the
 * functions defined in this header file).
 */
#include "libchemist/managers/managers.hpp"
#include "libchemist/orbital_space/orbital_space.hpp"

#include "libchemist/molecule/atom.hpp"
#include "libchemist/molecule/molecule.hpp"
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
inline auto apply_basis(const std::string& name, const Molecule& mol,
                        const BasisSetManager& man = BasisSetManager()) {
    AOBasisSet<double> aos;
    for(const auto& ai : mol) {
        auto ci = man.get_basis(name, ai.Z());
        for(auto i : {0, 1, 2}) ci.coord(i) = ai.coords()[i];
        aos.add_center(ci);
    }
    return orbital_space::AOSpace<double>(aos);
}

} // namespace libchemist
