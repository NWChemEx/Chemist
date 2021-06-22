#pragma once
#include <cstddef>
#include <typeindex>
#include <sde/detail_/memoization.hpp>

namespace libchemist {

struct Operator {
  /// Hash function
  inline void hash(sde::Hasher& h) const { hash_impl(h); }

  template <typename OpType>
  inline bool compare( const OpType& other ) const noexcept {
    return compare_impl( typeid(OpType) );
  }

  virtual ~Operator() noexcept = default;

protected:

  virtual void hash_impl( sde::Hasher& h ) const = 0;
  virtual bool compare_impl( std::type_index index ) const noexcept = 0;

};

inline bool operator==( const Operator& lhs, const Operator& rhs ) {
  return lhs.compare(rhs);
}


struct DensityDependentOperator : public Operator { };



struct Electron{};
struct Nucleus{};
struct PointCharge{};
struct PointNucleus : public Nucleus, public PointCharge {};

namespace detail_ {

  template <typename ParticleType, typename = void>
  struct is_nucleus_particle : public std::false_type {};
  template <typename ParticleType>
  struct is_nucleus_particle< ParticleType,
    std::enable_if_t<std::is_base_of_v<Nucleus,ParticleType>> > :
    public std::true_type {};


  template <typename ParticleType>
  inline static constexpr bool is_nucleus_particle_v =
    is_nucleus_particle<ParticleType>::value;

  template <typename ParticleType, typename = void>
  struct NElectrons : public std::integral_constant<std::size_t,0> {};
  template <typename ParticleType, typename = void>
  struct NNuclei : public std::integral_constant<std::size_t,0> {};

  template <>
  struct NElectrons<Electron> : public std::integral_constant<std::size_t,1> {};
  template <typename ParticleType> 
  struct NNuclei< ParticleType,
    std::enable_if_t< is_nucleus_particle_v<ParticleType>> > : 
    public std::integral_constant<std::size_t,1> {};


  template <typename Head, typename... Tail>
  struct CountElectrons : public std::integral_constant< std::size_t, 
    NElectrons<Head>::value + CountElectrons<Tail...>::value > {};
  template <typename ParticleType>
  struct CountElectrons<ParticleType> : public std::integral_constant< std::size_t,
    NElectrons<ParticleType>::value > {};

  template <typename Head, typename... Tail>
  struct CountNuclei : public std::integral_constant< std::size_t, 
    NNuclei<Head>::value + CountNuclei<Tail...>::value > {};
  template <typename ParticleType>
  struct CountNuclei<ParticleType> : public std::integral_constant< std::size_t,
    NNuclei<ParticleType>::value > {};


  template <typename... Particles>
  inline static constexpr auto n_electrons_v =
    CountElectrons<Particles...>::value;
  template <typename... Particles>
  inline static constexpr auto n_nuclei_v =
    CountNuclei<Particles...>::value;
}



#define REGISTER_OPERATOR(NAME,OpType,...)             \
  struct NAME : public OpType {                        \
    inline static constexpr std::size_t n_electrons =  \
      detail_::n_electrons_v<__VA_ARGS__>;             \
    inline static constexpr std::size_t n_nuclei =     \
      detail_::n_nuclei_v<__VA_ARGS__>;                \
    inline void hash_impl( sde::Hasher& h )            \
      const override { return h(*this); }              \
    inline bool compare_impl( std::type_index index )  \
      const noexcept override {                        \
      return index == std::type_index(typeid(NAME));   \
    }                                                  \
  };


template <typename ParticleType>
REGISTER_OPERATOR( Kinetic, Operator, ParticleType );
template <typename P1, typename P2>
REGISTER_OPERATOR( CoulombInteraction, Operator, P1, P2 );

using ElectronKinetic         = Kinetic<Electron>;
using ElectronNuclearCoulomb  = CoulombInteraction< Electron, PointNucleus >;
using ElectronElectronCoulomb = CoulombInteraction< Electron, Electron >;


REGISTER_OPERATOR( MeanFieldElectronCoulomb,       DensityDependentOperator, Electron );
REGISTER_OPERATOR( MeanFieldElectronExactExchange, DensityDependentOperator, Electron );
REGISTER_OPERATOR( KohnShamExchangeCorrelation,    DensityDependentOperator, Electron );

#undef REGISTER_OPERATOR
}
