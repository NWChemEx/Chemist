#pragma once

namespace chemist::fragmenting::detail_ {

/** @brief Implements Fragmented<FragmentedType>
 * 
 *  @tparam TypeToFragment Chemist object which has been broken into subsets.
 *                         Expected to be Nuclei, Molecule, or ChemicalSystem.
 * 
 * The Fragmented<T> class is implemented by a FragmentedPIMPL<T> object. Each
 * FragmentedPIMPL<T> object contains the state unique to T, but exposes it
 * through a common API. This in general means that each FragmentedPIMPL<T>
 * object contains a different implementation, thus we do not define a primary
 * template and instead require developers to provide implementations for each
 * T they want to support.
 * 
 * FWIW, this design avoids having user-facing classes like FragmentedNuclei, 
 * FragmentedMolecule, etc. If we had such classes, then the user would need to
 * use template meta-programming techniques to unify treatments. Instead, by
 * having the user-facing classes behave like a single templated class we can
 * hide the TMP under the hood of the Fragmented<T> class, avoiding exposing it
 * to the user. The trade-off is the developers need to maintain different
 * PIMPLs for each scenario (essentially the PIMPLs become the FragmentedNuclei,
 * FragmentedMolecule, etc.).
*/
template<typename TypeToFragment>
class FragmentedPIMPL;

}

// Explicit specializations should be included here so user can use this file
// like it actually defines classes
#include "fragmented_pimpl_nuclei.hpp"