#pragma once

/**
 * @file libchemist.hpp
 *
 * This is a convenience header defining the public API of the libchemist
 * library.  It should NOT be included in any other libchemist header file,
 * source file, or test (the exception being Testlibchemist, which tests the
 * functions defined in this header file).
 */

#include "libchemist/basis_set/basis_set.hpp"
#include "libchemist/chemical_system/chemical_system.hpp"
#include "libchemist/managers/managers.hpp"
#include "libchemist/molecule/molecule.hpp"
#include "libchemist/operators/operators.hpp"
#include "libchemist/orbital_space/orbital_space.hpp"
#include "libchemist/tensor/tensor.hpp"
#include "libchemist/topology/topology.hpp"
#include "libchemist/wavefunction/wavefunction.hpp"

/**
 * @brief The primary namespace for the libchemist library
 *
 * All functionality of the libchemist library intended for end user consumption
 * is defined within this namespace.
 *
 */
namespace libchemist {

} // namespace libchemist
