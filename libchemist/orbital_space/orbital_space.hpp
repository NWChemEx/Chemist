#pragma once
/** @file orbital_space.hpp
 *
 *  This file is a convenience header for using the OrbitalSpace class and its
 *  related classes/functions. Inclusion of this header will include all of the
 *  classes/functions and should only be used by downstream projects. Within
 *  libchemist please only include the minimal header set required for
 *  compilation.
 */
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/canonical_mos.hpp"
#include "libchemist/orbital_space/dependent_orbital_space.hpp"
#include "libchemist/orbital_space/orbital_space_class.hpp"
#include "libchemist/orbital_space/orthogonal_space.hpp"

namespace libchemist {

template<typename T>
using DependentOrbitals =
  DependentOrbitalSpace<OrbitalSpace<T, type::tensor_of_tensors<T>>>;

template<typename T>
using DependentOrthogonalOrbitals =
  DependentOrbitalSpace<OrthogonalSpace<T, type::tensor_of_tensors<T>>>;

template<typename T>
using DependentCanonicalOrbitals =
  DependentOrbitalSpace<CanonicalMO<T, type::tensor_of_tensors<T>>>;

}
