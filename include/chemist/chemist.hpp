#pragma once

/**
 * @file chemist.hpp
 *
 * This is a convenience header defining the public API of the chemist
 * library.  It should NOT be included in any other chemist header file,
 * source file, or test (the exception being Testchemist, which tests the
 * functions defined in this header file).
 */

#include "chemist/basis_set/basis_set.hpp"
#include "chemist/chemical_system/chemical_system.hpp"
#include "chemist/managers/managers.hpp"
#include "chemist/molecule/molecule.hpp"
#include "chemist/operators/operators.hpp"
#include "chemist/orbital_space/orbital_space.hpp"
#include "chemist/topology/topology.hpp"
#include "chemist/wavefunction/wavefunction.hpp"

/**
 * @brief The primary namespace for the chemist library
 *
 * All functionality of the chemist library intended for end user consumption
 * is defined within this namespace.
 *
 */
namespace chemist {} // namespace chemist
