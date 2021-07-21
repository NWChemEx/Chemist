#pragma once

#include "libchemist/wavefunction/determinant_space.hpp"
#include "libchemist/wavefunction/nonrelativistic.hpp"
#include "libchemist/wavefunction/perturbative.hpp"
#include "libchemist/wavefunction/type_traits.hpp"

namespace libchemist::wavefunction {

/// Type of a determinant space which uses derived orbitals
using DeterminantD = DeterminantSpace<orbital_space::DerivedSpaceD>;

/// Type of a determinant space which uses canonical MOs
using CanonicalDeterminantD = DeterminantSpace<orbital_space::CanonicalSpaceD>;

/// Type of a determinant space which uses local orbitals
using LocalDeterminantD = DeterminantSpace<orbital_space::IndDerivedSpaceD,
                                           orbital_space::DepDerivedSpaceD>;

/// Type of a determinant space which uses quasicanonical local orbitals
using CanonicalLocalDeterminantD =
  DeterminantSpace<orbital_space::CanonicalIndSpaceD,
                   orbital_space::CanonicalDepSpaceD>;

/// Type of a wavefunction built from MOs
using Reference = Nonrelativistic<DeterminantD>;

/// Type of a wavefunction built from canonical MOs
using CanonicalReference = Nonrelativistic<CanonicalDeterminantD>;

/// Type of a wavefunction built from local MOs
using LocalReference = Nonrelativistic<LocalDeterminantD>;

/// Type of a wavefunction built frorm quasi-canonical local MOs
using CanonicalLocalReference = Nonrelativistic<CanonicalLocalDeterminantD>;

/// Type of a perturbative correlated wavefunction, using MOs
using Perturbative = Perturbative<Reference>;

/// Type of a perturbative correlated wavefunction, using canonical MOs
using CanonicalPerturbative = Perturbative<CanonicalReference>;

/// Type of a perturbative correlated wavefunction built from local MOs
using LocalPerturbative = Perturbative<LocalReference>;

/// Type of a wavefunction built frorm quasi-canonical local MOs
using CanonicalLocalPertrubative = Perturbative<CanonicalLocalReference>;

} // namespace libchemist::wavefunction
