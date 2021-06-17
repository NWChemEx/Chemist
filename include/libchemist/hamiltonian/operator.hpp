#pragma once
#include <cstddef>
#include <typeindex>
#include <sde/detail_/memoization.hpp>

namespace libchemist {

template <std::size_t N>
struct Operator {
  static_assert( (N > 0), "Operator must have NBODY >= 1" );
  inline static constexpr std::size_t n_body = N;

  /// Hash function
  inline void hash(sde::Hasher& h) const { hash_impl(h); }

  template <typename OpType>
  inline bool compare( const OpType& other ) const noexcept {
    if constexpr (OpType::n_body != N) return false;
    else return compare_impl( typeid(OpType) );
  }

  virtual ~Operator() noexcept = default;

protected:

  virtual void hash_impl( sde::Hasher& h ) const = 0;
  virtual bool compare_impl( std::type_index index ) const noexcept = 0;

};

template <std::size_t N>
bool operator==( const Operator<N>& lhs, const Operator<N>& rhs ) {
  return lhs.compare(rhs);
}

template <std::size_t N, std::size_t M>
constexpr std::enable_if_t<N!=M,bool> 
  operator==( const Operator<N>& lhs, const Operator<M>& rhs) {
  return false;
}


template <std::size_t N>
struct DensityDependentOperator : public Operator<N> { };


#define REGISTER_OPERATOR(NAME,OpType)                 \
  struct NAME : public OpType {                        \
    void hash_impl( sde::Hasher& h )                   \
      const override { return h(*this); }              \
    bool compare_impl( std::type_index index )         \
      const noexcept override {                        \
      return index == std::type_index(typeid(NAME));   \
    }                                                  \
  };

REGISTER_OPERATOR( ElectronKinetic,         Operator<1> );
REGISTER_OPERATOR( ElectronNuclearCoulomb,  Operator<1> );
REGISTER_OPERATOR( ElectronElectronCoulomb, Operator<2> );


REGISTER_OPERATOR( MeanFieldElectronCoulomb,       DensityDependentOperator<1> );
REGISTER_OPERATOR( MeanFieldElectronExactExchange, DensityDependentOperator<1> );
REGISTER_OPERATOR( KohnShamExchangeCorrelation,    DensityDependentOperator<1> );

#undef REGISTER_OPERATOR
}
