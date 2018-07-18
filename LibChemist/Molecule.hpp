#pragma once
#include "LibChemist/Atom.hpp"

namespace LibChemist {
namespace detail_ {
class MolPIMPL;
} // namespace detail_

/**
 * @brief Class for modeling the atoms as a whole.
 *
 * For a typical computational chemistry calculation the Molecule class
 * represents the second tier of input.  By this we mean that the Molecule class
 * describes how the first tier of input (the Atoms) come together.  In addition
 * to simply listing the atoms, the Molecule also stores information on how the
 * atoms couple such as the overall spin/charge
 */
class Molecule {

};

} // end namespace LibChemist
