#pragma once
#include "libchemist/operator/operator.hpp"
#include <catch2/catch.hpp>
#include <utilities/type_traits/tuple/tuple_cat.hpp>

namespace testing {

using all_operator_impls = std::tuple<
  libchemist::ElectronElectronSTG, libchemist::ElectronElectronCoulomb,
  libchemist::NuclearNuclearCoulomb, libchemist::ElectronNuclearCoulomb,
  libchemist::ElectronEDensityCoulomb, libchemist::ElectronElectronDelta,
  libchemist::MeanFieldKohnShamExchangeCorrelation,
  libchemist::ElectronEDensityExchange,
  libchemist::ElectronElectronF12Commutator, libchemist::ElectronIdentity,
  libchemist::ElectronKinetic, libchemist::ElectronElectronYukawa>;

using all_multipoles =
  std::tuple<libchemist::ElectricDipole, libchemist::ElectricQuadrupole,
             libchemist::ElectricOctupole>;

using all_operators =
  utilities::type_traits::tuple::tuple_cat_t<all_operator_impls,
                                             all_multipoles>;

// using derived_operators = std::tuple<FockOperator, Hamiltonian>;

} // namespace testing
