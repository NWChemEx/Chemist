#pragma once
#include <cstddef>
#include <typeindex>
#include <sde/detail_/memoization.hpp>
#include <utilities/type_traits/parameter_pack_traits.hpp>

namespace libchemist {

/** @brief An abstract base class for Operator types
 *
 *  This class provides the basic API specifications for 
 *  Operator types to satisfy `SDEAny` type requirements
 *  and to allow for their identification and manipulation
 *  in a type-erased manner
 *
 *  All Operator types are to be derived from this class.
 */
struct Operator {
    /// Hash function
    inline void hash(sde::Hasher& h) const { hash_impl(h); }

    /// Compare this operator instance with another.
    template <typename OpType>
    inline bool compare( const OpType& other ) const noexcept {
      return compare_impl( typeid(OpType) );
    }

    /// Defaulted no-throw dtor
    virtual ~Operator() noexcept = default;

protected:
    /// Derived implementation of Hash function.
    virtual void hash_impl( sde::Hasher& h ) const = 0;
    /// Derived implementation of comparison function.
    virtual bool compare_impl( std::type_index index ) const noexcept = 0;
};

/** @brief Check equality of two type-erased Operator types
 *  
 *  @param[in] lhs First operator to compare
 *  @param[in] rhs Second operator to compare
 *
 *  @return `true` if the two operators are identical, `false` otherwise.
 */
inline bool operator==( const Operator& lhs, const Operator& rhs ) {
    return lhs.compare(rhs);
}


/** @brief A class to represent density-dependent operators
 *
 *  This class allows for the identification of an operator
 *  as a density dependent operator (e.g. mean-field operators).
 *  Derived from `Operator`.
 *
 *  All density dependent operator types are to be derived from 
 *  this class.
 */
struct DensityDependentOperator : public Operator { };


/// Strong type representing an electron.
struct Electron {};
/// Strong type representing a nucleus.
struct Nucleus {};
/// Strong type representing a point charge.
struct PointCharge {};
/// Strong type representing a nucleus with a point charge distribution.
struct PointNucleus : public Nucleus, public PointCharge {};
/// Strong type representing a dipole field.
struct DipoleField {};
/// Strong type representing a quadrupole field.
struct QuadrupoleField {};
/// Strong type representing a octupole field.
struct OctupoleField {};

namespace detail_ {
  template <typename... Particles>
  inline static constexpr std::size_t n_electrons_v =
    utilities::type_traits::parameter_pack_count_derived_type_v<Electron,Particles...>;
  template <typename... Particles>
  inline static constexpr std::size_t n_nuclei_v =
    utilities::type_traits::parameter_pack_count_derived_type_v<Nucleus,Particles...>;
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


/// Kinetic energy operator type
template <typename ParticleType>
REGISTER_OPERATOR( Kinetic, Operator, ParticleType );

/// Coulomb interaction operator type
template <typename P1, typename P2>
REGISTER_OPERATOR( CoulombInteraction, Operator, P1, P2 );

/// Classical Electron-Field interaction operator type
template <typename FieldType>
REGISTER_OPERATOR( ClassicalFieldInteraction, Operator, Electron, FieldType );

/// Electron-Kinetic energy operator type
using ElectronKinetic         = Kinetic<Electron>;
/// Electron-Nuclear Coulomb attraction operator type
using ElectronNuclearCoulomb  = CoulombInteraction< Electron, PointNucleus >;
/// Electron-Electron Coulomb repulsion operator type
using ElectronElectronCoulomb = CoulombInteraction< Electron, Electron >;

/// Classical Electron-Dipole field interaction operator type
using ClassicalElectronDipoleField     = ClassicalFieldInteraction<DipoleField>;
/// Classical Electron-Quadrupole field interaction operator type
using ClassicalElectronQuadrupoleField = ClassicalFieldInteraction<QuadrupoleField>;
/// Classical Electron-Octupole field interaction operator type
using ClassicalElectronOctupoleField   = ClassicalFieldInteraction<OctupoleField>;


/// Mean-field Coulomb operator type
REGISTER_OPERATOR( MeanFieldElectronCoulomb,       DensityDependentOperator, Electron );
/// Mean-field exact (Hartree-Fock) exchange operator type
REGISTER_OPERATOR( MeanFieldElectronExactExchange, DensityDependentOperator, Electron );
/// Kohn-Sham exchange correlation (XC) operator type
REGISTER_OPERATOR( KohnShamExchangeCorrelation,    DensityDependentOperator, Electron );

#undef REGISTER_OPERATOR
}
