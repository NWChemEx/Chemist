#pragma once
#include "../test_libchemist.hpp"
#include "libchemist/operator/operator.hpp"
#include <catch2/catch.hpp>
#include <utilities/type_traits/tuple/tuple_cat.hpp>

namespace testing {

template<typename T>
auto non_default_parameter() {
    if constexpr(std::is_same_v<T, libchemist::operators::Electron>) {
        return libchemist::operators::Electron{};
    } else if constexpr(std::is_same_v<T, libchemist::operators::STG>) {
        return libchemist::operators::STG(1.2, 2.3);
    } else if constexpr(std::is_same_v<T,
                                       libchemist::operators::ManyElectrons>) {
        return libchemist::operators::ManyElectrons{2};
    } else if constexpr(std::is_same_v<
                          T, libchemist::operators::OneElectronDensity>) {
        auto a_tensor = testing::generate_tensor(2);
        return libchemist::operators::OneElectronDensity(a_tensor);
    } else if constexpr(std::is_same_v<T, libchemist::Nuclei>) {
        libchemist::Atom H(1ul);
        return libchemist::Nuclei{H, H};
    } else {
        static_assert(std::is_same_v<T, libchemist::operators::Electron>,
                      "Particle not coded");
    }
}

// Tuple containing the known densities
using density_types = std::tuple<libchemist::operators::OneElectronDensity>;

// Tuple containing known Slater-type geminal types
using stg_types = std::tuple<libchemist::operators::STG>;

using correlation_factor_types =
  std::tuple<libchemist::operators::ElectronElectronSTG>;

using all_coulomb =
  std::tuple<libchemist::operators::ElectronRepulsion,
             libchemist::operators::NuclearRepulsion,
             libchemist::operators::ElectronNuclearAttraction,
             libchemist::operators::ElectronEDensityCoulomb,
             libchemist::operators::NElectronRepulsion,
             libchemist::operators::NElectronNuclearAttraction>;

using all_operator_impls =
  std::tuple<libchemist::operators::ElectronElectronDelta,
             libchemist::operators::ExchangeCorrelation,
             libchemist::operators::ElectronEDensityExchange,
             libchemist::operators::ElectronElectronF12Commutator,
             libchemist::operators::ElectronIdentity,
             libchemist::operators::ElectronKinetic,
             libchemist::operators::ElectronElectronYukawa>;

using all_multipoles = std::tuple<libchemist::operators::ElectricDipole,
                                  libchemist::operators::ElectricQuadrupole,
                                  libchemist::operators::ElectricOctupole>;

using all_operators = utilities::type_traits::tuple::tuple_cat_t<
  correlation_factor_types, all_operator_impls, all_coulomb, all_multipoles>;

// using derived_operators = std::tuple<FockOperator, Hamiltonian>;

} // namespace testing
