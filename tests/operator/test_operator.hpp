#pragma once
#include "libchemist/operator/operator.hpp"
#include <catch2/catch.hpp>

namespace testing {

using all_operators = std::tuple<
  libchemist::ElectronElectronSTG, libchemist::ElectronElectronCoulomb,
  libchemist::NuclearNuclearCoulomb, libchemist::ElectronNuclearCoulomb,
  libchemist::ElectronEDensityCoulomb, libchemist::ElectronElectronDelta,
  libchemist::MeanFieldKohnShamExchangeCorrelation,
  libchemist::ElectronEDensityExchange,
  libchemist::ElectronElectronF12Commutator, libchemist::ElectronIdentity,
  libchemist::ElectronKinetic,
  // libchemist::ElectricDipole,
  // libchemist::ElectricQuadrupole, libchemist::ElectricOctupole,
  libchemist::ElectronElectronYukawa>;

// using derived_operators = std::tuple<FockOperator, Hamiltonian>;

} // namespace testing
