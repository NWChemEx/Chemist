#pragma once
#include <cstddef>
#include <typeinfo>

namespace libchemist {

template <std::size_t N>
struct Operator {
  static_assert( (N > 0), "Operator must have NBODY >= 1" );
  inline static constexpr std::size_t n_body = N;

  inline std::size_t type_hash() { return type_hash_; } 
  inline explicit Operator(std::size_t hash) : type_hash_(hash){}
  inline Operator() : Operator(0){}
protected:

  std::size_t type_hash_ = 0;
};

template <std::size_t N>
struct DensityDependentOperator : public Operator<N> {
  inline static constexpr std::size_t n_body = Operator<N>::n_body;

  inline explicit DensityDependentOperator(std::size_t hash) :
    Operator<N>(hash) {}
  inline DensityDependentOperator() : Operator<N>() {}

  // TODO: this needs to reference a density in some way 
};


#define REGISTER_OPERATOR(NAME,OpType)           \
  struct NAME : public OpType {                  \
    NAME() : OpType(typeid(NAME).hash_code()) {} \
  };

REGISTER_OPERATOR( ElectronKinetic,         Operator<1> );
REGISTER_OPERATOR( ElectronNuclearCoulomb,  Operator<1> );
REGISTER_OPERATOR( ElectronElectronCoulomb, Operator<2> );


REGISTER_OPERATOR( MeanFieldElectronCoulomb,       DensityDependentOperator<1> );
REGISTER_OPERATOR( MeanFieldElectronExactExchange, DensityDependentOperator<1> );
REGISTER_OPERATOR( KohnShamExchangeCorrelation,    DensityDependentOperator<1> );

#undef REGISTER_OPERATOR
}
