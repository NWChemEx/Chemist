#pragma once
#include "../test_libchemist.hpp"
#include "libchemist/operator/operator.hpp"
#include <catch2/catch.hpp>
#include <utilities/type_traits/tuple/tuple_cat.hpp>

namespace testing {

template<typename T>
auto non_default_parameter() {
    if constexpr(std::is_same_v<T, libchemist::Electron>) {
        return libchemist::Electron{};
    } else if constexpr(std::is_same_v<T, libchemist::STG>) {
        return libchemist::STG(1.2, 2.3);
    } else if constexpr(std::is_same_v<T, libchemist::ManyElectrons>) {
        return libchemist::ManyElectrons{2};
    } else if constexpr(std::is_same_v<T, libchemist::OneElectronDensity>) {
        auto a_tensor = testing::generate_tensor(2);
        return libchemist::OneElectronDensity(a_tensor);
    } else if constexpr(std::is_same_v<T, libchemist::Nuclei>) {
        libchemist::Atom H(1ul);
        return libchemist::Nuclei{H, H};
    } else {
        static_assert(std::is_same_v<T, libchemist::Electron>,
                      "Particle not coded");
    }
}

using all_coulomb = std::tuple<
  libchemist::ElectronRepulsion, libchemist::NuclearRepulsion,
  libchemist::ElectronNuclearAttraction, libchemist::ElectronEDensityCoulomb,
  libchemist::NElectronRepulsion, libchemist::NElectronNuclearAttraction>;

using all_operator_impls = std::tuple<
  libchemist::ElectronElectronSTG, libchemist::ElectronElectronDelta,
  libchemist::ExchangeCorrelation, libchemist::ElectronEDensityExchange,
  libchemist::ElectronElectronF12Commutator, libchemist::ElectronIdentity,
  libchemist::ElectronKinetic, libchemist::ElectronElectronYukawa>;

using all_multipoles =
  std::tuple<libchemist::ElectricDipole, libchemist::ElectricQuadrupole,
             libchemist::ElectricOctupole>;

using all_operators =
  utilities::type_traits::tuple::tuple_cat_t<all_operator_impls, all_coulomb,
                                             all_multipoles>;

// using derived_operators = std::tuple<FockOperator, Hamiltonian>;

} // namespace testing
