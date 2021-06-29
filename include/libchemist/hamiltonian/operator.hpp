#pragma once
#include <cstddef>
#include <typeindex>
#include <sde/detail_/memoization.hpp>
#include <utilities/type_traits/parameter_pack_traits.hpp>
#include "libchemist/potentials/electrostatic.hpp"

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

    /// Polymorphic comparison of this Operator instance with another
    template <typename OpType>
    inline bool is_equal( const OpType& other ) const noexcept {
      return is_equal_impl( static_cast<const Operator&>(other) ) and
             other.is_equal_impl(*this);
    }

    /** @brief Non-polymorphic comparison of Operator instances
     *
     *  @param[in] other Operator instance to compare
     *  @return    true (stateless base class)
     */
    inline bool operator==( const Operator& ) const { return true; }
    inline bool operator!=( const Operator& ) const { return false; }

    /// Defaulted no-throw dtor
    virtual ~Operator() noexcept = default;

protected:
    /// Derived implementation of Hash function.
    virtual void hash_impl( sde::Hasher& h ) const = 0;
    /// Derived implementation of comparison function.
    //virtual bool is_equal_impl( std::type_index index ) const noexcept = 0;
    virtual bool is_equal_impl( const Operator& ) const noexcept = 0;
};



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


#if 0

#define REGISTER_OPERATOR(NAME,OpType,...)             \
  struct NAME : public OpType {                        \
    inline static constexpr std::size_t n_electrons =  \
      detail_::n_electrons_v<__VA_ARGS__>;             \
    inline static constexpr std::size_t n_nuclei =     \
      detail_::n_nuclei_v<__VA_ARGS__>;                \
    inline void hash_impl( sde::Hasher& h )            \
      const override { return h(*this); }              \
    inline bool is_equal_impl( std::type_index index )  \
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

#else


#define REGISTER_BASE_OPERATOR(NAME,BASE,...)                                 \
struct NAME : public BASE {                                                   \
    friend struct Operator;                                                   \
    static constexpr auto n_electrons = detail_::n_electrons_v<__VA_ARGS__>;  \
    static constexpr auto n_nuclei    = detail_::n_nuclei_v<__VA_ARGS__>;     \
    /* Base operators are stateless */                                        \
    inline bool operator==( const NAME& other ) const { return true; }        \
    inline bool operator!=( const NAME& other ) const { return false; }       \
protected:                                                                    \
    virtual inline void hash_impl( sde::Hasher& h ) const override            \
      { return h(*this); }                                                    \
    virtual inline bool is_equal_impl( const Operator& other ) const noexcept \
      override {                                                              \
      auto ptr = dynamic_cast<const NAME*>(&other);                           \
      if( !ptr ) return false;                                                \
      return *this == *ptr;                                                   \
    }                                                                         \
};                                                                            \


/// Kinetic energy operator type
template <typename P>
REGISTER_BASE_OPERATOR( Kinetic, Operator, P );

template <typename P, typename Q>
std::enable_if_t< !std::is_same_v<P,Q>, bool > operator==(
  const Kinetic<P>& lhs, const Kinetic<Q>& rhs
) { return false; }

/// Coulomb interaction operator type
template <typename P1, typename P2>
REGISTER_BASE_OPERATOR( CoulombInteraction, Operator, P1, P2 );

template <typename P1, typename P2, typename Q1, typename Q2>
std::enable_if_t<!std::is_same_v<P1,P2> or !std::is_same_v<P2,Q2>, bool> operator==(
  const CoulombInteraction<P1,P2>& lhs, const CoulombInteraction<Q1,Q2>& rhs
) { return false; }


/// Mean-field Coulomb operator type
REGISTER_BASE_OPERATOR( MeanFieldElectronCoulomb,       DensityDependentOperator, Electron );
/// Mean-field exact (Hartree-Fock) exchange operator type
REGISTER_BASE_OPERATOR( MeanFieldElectronExactExchange, DensityDependentOperator, Electron );
/// Kohn-Sham exchange correlation (XC) operator type
REGISTER_BASE_OPERATOR( KohnShamExchangeCorrelation,    DensityDependentOperator, Electron );

// Typedef'd stateless operators

/// Electron-Kinetic energy operator type
using ElectronKinetic         = Kinetic<Electron>;
/// Electron-Electron Coulomb repulsion operator type
using ElectronElectronCoulomb = CoulombInteraction< Electron, Electron >;




// Stateful operators

/// Electron-Nuclear Coulomb accraction operator type
class ElectronNuclearCoulomb : public CoulombInteraction<Electron,Nucleus> {
    using base_type = CoulombInteraction<Electron,Nucleus>;

    friend struct Operator;
    friend struct CoulombInteraction<Electron,Nucleus>;

public:
    static constexpr std::size_t n_electrons = 1;
    static constexpr std::size_t n_nuclei    = 1;

    inline bool operator==( const ElectronNuclearCoulomb& other ) const {
        const auto& this_as_coul = static_cast<const base_type&>(*this);
        const auto& othr_as_coul = static_cast<const base_type&>(other);
        return (this_as_coul == othr_as_coul) and (potential_ == other.potential_);
    }
    inline bool operator!=( const ElectronNuclearCoulomb& other ) const {
        return not ((*this) == other); 
    }

    inline const auto& potential() const { return potential_; }
    inline auto&       potential()       { return potential_; }

    template <typename ChargeType>
    inline void add_charge( ChargeType q ) {
      potential_.add_charge( std::move(q) );
    }

protected:
    inline void hash_impl( sde::Hasher& h ) const override {
        return h(potential_);
    }

    inline bool is_equal_impl( const Operator& other ) const noexcept override {
        auto ptr = dynamic_cast<const ElectronNuclearCoulomb*>(&other);
        if( !ptr ) return false;                     
        return *this == *ptr;                        
    }

private:
    /// Electrostatic potential induced by the nuclei
    potentials::Electrostatic potential_;
    // TODO: Need to generalize Electrostatic to take differing charge models
};

#endif
}
