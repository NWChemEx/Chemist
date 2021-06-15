#pragma once
#include <cstddef>

namespace libchemist {

template <std::size_t N>
struct Operator {
  static_assert( (N > 0), "Operator must have NBODY >= 1" );
  inline static constexpr std::size_t n_body = N;
};

template <std::size_t N>
struct DensityDependentOperator : public Operator<N> {
  inline static constexpr std::size_t n_body = Operator<N>::n_body;

  // TODO: this needs to reference a density in some way 
};

struct ElectronKinetic         : public Operator<1> {};
struct ElectronNuclearCoulomb  : public Operator<1> {};
struct ElectronElectronCoulomb : public Operator<2> {};

struct MeanFieldElectronCoulomb       : public DensityDependentOperator<1> {};
struct MeanFieldElectronExactExchange : public DensityDependentOperator<1> {};
struct KohnShamExchangeCorrelation    : public DensityDependentOperator<1> {};

}
