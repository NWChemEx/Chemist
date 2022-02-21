#pragma once
#include "../test_chemist.hpp"
#include "chemist/electrons/electrons.hpp"
#include "chemist/operators/operators.hpp"
#include <catch2/catch.hpp>
#include <utilities/type_traits/tuple/tuple_cat.hpp>

namespace testing {

template<typename T>
auto non_default_parameter() {
    if constexpr(std::is_same_v<T, chemist::Electron>) {
        return chemist::Electron{};
    } else if constexpr(std::is_same_v<T, chemist::operators::STG>) {
        return chemist::operators::STG(1.2, 2.3);
    } else if constexpr(std::is_same_v<T, chemist::ManyElectrons>) {
        return chemist::ManyElectrons{2};
    } else if constexpr(std::is_same_v<T, chemist::OneElectronDensity>) {
        auto a_tensor = testing::generate_tensor(2);
        return chemist::OneElectronDensity(a_tensor,
                                           chemist::orbital_space::AOSpaceD{});
    } else if constexpr(std::is_same_v<T, chemist::Decomposable1EDensity>) {
        auto a_tensor = testing::generate_tensor(2);
        return chemist::Decomposable1EDensity(
          a_tensor, chemist::orbital_space::DerivedSpaceD{});
    } else if constexpr(std::is_same_v<T, chemist::Nuclei>) {
        chemist::Atom H(1ul);
        return chemist::Nuclei{H, H};
    } else if constexpr(std::is_same_v<T, chemist::Point<double>>) {
        return chemist::Point<double>(1., 2., 3.);
    } else {
        static_assert(std::is_same_v<T, chemist::Electron>,
                      "Particle not coded");
    }
}

// Tuple containing known Slater-type geminal types
using stg_types = std::tuple<chemist::operators::STG>;

using correlation_factor_types =
  std::tuple<chemist::operators::ElectronElectronSTG>;

using all_coulomb = std::tuple<chemist::operators::ElectronRepulsion,
                               chemist::operators::NuclearRepulsion,
                               chemist::operators::ElectronNuclearAttraction,
                               chemist::operators::ElectronEDensityCoulomb,
                               chemist::operators::NElectronRepulsion,
                               chemist::operators::NElectronNuclearAttraction>;

using all_operator_impls =
  std::tuple<chemist::operators::ElectronElectronDelta,
             chemist::operators::ExchangeCorrelation,
             chemist::operators::ElectronEDensityExchange,
             chemist::operators::ElectronDecomposableEDensity,
             chemist::operators::ElectronElectronF12Commutator,
             chemist::operators::ElectronIdentity,
             chemist::operators::ElectronKinetic,
             chemist::operators::ElectronElectronYukawa,
             chemist::operators::ElectronIdentity_Nuclear>;

using all_multipoles = std::tuple<chemist::operators::ElectricDipole,
                                  chemist::operators::ElectricQuadrupole,
                                  chemist::operators::ElectricOctupole>;

using all_operators = utilities::type_traits::tuple::tuple_cat_t<
  correlation_factor_types, all_operator_impls, all_coulomb, all_multipoles>;

// using derived_operators = std::tuple<FockOperator, Hamiltonian>;

} // namespace testing
