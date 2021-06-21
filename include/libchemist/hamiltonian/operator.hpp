#pragma once
#include <cstddef>
#include <typeindex>
#include <sde/detail_/memoization.hpp>

namespace libchemist {

template <std::size_t NE, std::size_t NN = 0>
struct Operator {
  inline static constexpr std::size_t n_elec = NE;
  inline static constexpr std::size_t n_nuc  = NN;

  /// Hash function
  inline void hash(sde::Hasher& h) const { hash_impl(h); }

  template <typename OpType>
  inline bool compare( const OpType& other ) const noexcept {
    if constexpr (OpType::n_elec != NE or OpType::n_nuc != NN) return false;
    else return compare_impl( typeid(OpType) );
  }

  virtual ~Operator() noexcept = default;

protected:

  virtual void hash_impl( sde::Hasher& h ) const = 0;
  virtual bool compare_impl( std::type_index index ) const noexcept = 0;

};

template <std::size_t NE, std::size_t NN>
bool operator==( const Operator<NE,NN>& lhs, const Operator<NE,NN>& rhs ) {
  return lhs.compare(rhs);
}

template <std::size_t NE, std::size_t NN, std::size_t ME, std::size_t MN>
constexpr std::enable_if_t<NE!=ME or NN!=MN,bool> 
  operator==( const Operator<NE,NN>& lhs, const Operator<ME,MN>& rhs) {
  return false;
}


template <std::size_t NE, std::size_t NN = 0>
struct DensityDependentOperator : public Operator<NE,NN> { };


#define REGISTER_OPERATOR(NAME,OpType,...)             \
  struct NAME : public OpType<__VA_ARGS__> {           \
    void hash_impl( sde::Hasher& h )                   \
      const override { return h(*this); }              \
    bool compare_impl( std::type_index index )         \
      const noexcept override {                        \
      return index == std::type_index(typeid(NAME));   \
    }                                                  \
  };

#if 0
REGISTER_OPERATOR( ElectronKinetic,         Operator, 1,0 );
REGISTER_OPERATOR( ElectronNuclearCoulomb,  Operator, 1,1 );
REGISTER_OPERATOR( ElectronElectronCoulomb, Operator, 2,0 );
#else

struct Electron{};
struct PointCharge{};
struct PointNucleus : public PointCharge{};

namespace detail_ {
  template <typename ParticleType>
  inline constexpr std::size_t n_electrons = 0;

  template <typename ParticleType>
  inline constexpr std::size_t n_nuclei = 0;

  template <>
  inline constexpr std::size_t n_electrons<Electron> = 1;
  template <>
  inline constexpr std::size_t n_nuclei<PointNucleus> = 1;

  template <typename Particle, typename... Particles>
  struct count_electrons {
    inline static constexpr std::size_t value = 
      n_electrons<Particle> + count_electrons<Particles...>::value;
  };
  template <typename Particle>
  struct count_electrons<Particle> {
    inline static constexpr std::size_t value = n_electrons<Particle>;
  };

  template <typename Particle, typename... Particles>
  struct count_nuclei {
    inline static constexpr std::size_t value = 
      n_nuclei<Particle> + count_nuclei<Particles...>::value;
  };
  template <typename Particle>
  struct count_nuclei<Particle> {
    inline static constexpr std::size_t value = n_nuclei<Particle>;
  };
}


template <typename ParticleType>
REGISTER_OPERATOR( Kinetic, Operator, 
  detail_::n_electrons<ParticleType>, 
  detail_::n_nuclei<ParticleType> )

template <typename P1, typename P2>
REGISTER_OPERATOR( CoulombInteraction, Operator, 
  detail_::count_electrons<P1,P2>::value,
  detail_::count_nuclei<P1,P2>::value )


using ElectronKinetic         = Kinetic<Electron>;
using ElectronNuclearCoulomb  = CoulombInteraction< Electron, PointNucleus >;
using ElectronElectronCoulomb = CoulombInteraction< Electron, Electron >;

#endif

REGISTER_OPERATOR( MeanFieldElectronCoulomb,       DensityDependentOperator, 1 );
REGISTER_OPERATOR( MeanFieldElectronExactExchange, DensityDependentOperator, 1 );
REGISTER_OPERATOR( KohnShamExchangeCorrelation,    DensityDependentOperator, 1 );

#undef REGISTER_OPERATOR
}
