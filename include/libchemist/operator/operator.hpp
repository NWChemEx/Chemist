#pragma once
#include "libchemist/potentials/electrostatic.hpp"
#include "libchemist/basis_set/shell_type.hpp"
#include "libchemist/point/point.hpp"

#include <cstddef>
#include <sde/hasher.hpp>
#include <typeindex>
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

    /// Polymorphic comparison of this Operator instance with another
    template<typename OpType>
    inline bool is_equal(const OpType& other) const noexcept {
        return is_equal_impl(static_cast<const Operator&>(other)) and
               other.is_equal_impl(*this);
    }

    /** @brief Non-polymorphic equality comparison of Operator instances
     *
     *  @param[in] other Operator instance to compare
     *  @return    true (stateless base class)
     */
    inline bool operator==(const Operator& other) const { return true; }

    /** @brief Non-polymorphic inequality comparison of Operator instances
     *
     *  @param[in] other Operator instance to compare
     *  @return    false (stateless base class)
     */
    inline bool operator!=(const Operator& other) const { return false; }

    /// Defaulted no-throw dtor
    virtual ~Operator() noexcept = default;

    /// Defaulted default noexcept ctor
    Operator() noexcept = default;
    /// Defaulted copy ctor
    Operator(const Operator&) noexcept = default;
    /// Defaulted move ctor
    Operator(Operator&&) noexcept = default;

protected:
    /// Derived implementation of Hash function.
    virtual void hash_impl(sde::Hasher& h) const = 0;
    /// Derived implementation of comparison function.
    // virtual bool is_equal_impl( std::type_index index ) const noexcept = 0;
    virtual bool is_equal_impl(const Operator&) const noexcept = 0;
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
struct DensityDependentOperator : public Operator {};

/// Strong type representing an electron.
struct Electron {};
/// Strong type representing a nucleus.
struct Nucleus {};

namespace detail_ {
template<typename... Particles>
inline static constexpr std::size_t n_electrons_v =
  utilities::type_traits::parameter_pack_count_derived_type_v<Electron,
                                                              Particles...>;
template<typename... Particles>
inline static constexpr std::size_t n_nuclei_v =
  utilities::type_traits::parameter_pack_count_derived_type_v<Nucleus,
                                                              Particles...>;
} // namespace detail_

#define REGISTER_BASE_OPERATOR(NAME, BASE, ...)                            \
    struct NAME : public BASE {                                            \
        friend struct Operator;                                            \
        static constexpr auto n_electrons =                                \
          detail_::n_electrons_v<__VA_ARGS__>;                             \
        static constexpr auto n_nuclei = detail_::n_nuclei_v<__VA_ARGS__>; \
        /* Base operators are stateless */                                 \
        inline bool operator==(const NAME& other) const { return true; }   \
        inline bool operator!=(const NAME& other) const { return false; }  \
        NAME() noexcept            = default;                              \
        NAME(const NAME&) noexcept = default;                              \
        NAME(NAME&&) noexcept      = default;                              \
        virtual ~NAME() noexcept    = default;                             \
                                                                           \
    protected:                                                             \
        virtual inline void hash_impl(sde::Hasher& h) const override {     \
            return h(*this);                                               \
        }                                                                  \
        virtual inline bool is_equal_impl(                                 \
          const Operator& other) const noexcept override {                 \
            auto ptr = dynamic_cast<const NAME*>(&other);                  \
            if(!ptr) return false;                                         \
            return *this == *ptr;                                          \
        }                                                                  \
    };


/// Identity operator
REGISTER_BASE_OPERATOR(Identity, Operator);

/// Kinetic energy operator type
template<typename P>
REGISTER_BASE_OPERATOR(Kinetic, Operator, P);

template<typename P, typename Q>
std::enable_if_t<!std::is_same_v<P, Q>, bool> operator==(
  const Kinetic<P>& lhs, const Kinetic<Q>& rhs) {
    return false;
}

/// Coulomb interaction operator type
template<typename P1, typename P2>
REGISTER_BASE_OPERATOR(CoulombInteraction, Operator, P1, P2);

template<typename P1, typename P2, typename Q1, typename Q2>
std::enable_if_t<!std::is_same_v<P1, P2> or !std::is_same_v<P2, Q2>, bool>
operator==(const CoulombInteraction<P1, P2>& lhs,
           const CoulombInteraction<Q1, Q2>& rhs) {
    return false;
}

/// Mean-field Coulomb operator type
REGISTER_BASE_OPERATOR(MeanFieldElectronCoulomb, DensityDependentOperator,
                       Electron);
/// Mean-field exact (Hartree-Fock) exchange operator type
REGISTER_BASE_OPERATOR(MeanFieldElectronExactExchange, DensityDependentOperator,
                       Electron);
/// Kohn-Sham exchange correlation (XC) operator type
REGISTER_BASE_OPERATOR(KohnShamExchangeCorrelation, DensityDependentOperator,
                       Electron);

#undef REGISTER_BASE_OPERATOR

// Typedef'd stateless operators

/// Electron-Kinetic energy operator type
using ElectronKinetic = Kinetic<Electron>;
/// Electron-Electron Coulomb repulsion operator type
using ElectronElectronCoulomb = CoulombInteraction<Electron, Electron>;

// Stateful operators

#if 0
/// Electron-Nuclear Coulomb accraction operator type
class ElectronNuclearCoulomb : public CoulombInteraction<Electron, Nucleus> {
    using base_type = CoulombInteraction<Electron, Nucleus>;
    friend struct Operator;

public:
    static constexpr std::size_t n_electrons = 1;
    static constexpr std::size_t n_nuclei    = 1;

    inline bool operator==(const ElectronNuclearCoulomb& other) const {
        const auto& this_as_coul = static_cast<const base_type&>(*this);
        const auto& othr_as_coul = static_cast<const base_type&>(other);
        return (this_as_coul == othr_as_coul) and
               (potential_ == other.potential_);
    }
    inline bool operator!=(const ElectronNuclearCoulomb& other) const {
        return not((*this) == other);
    }

    inline const auto& potential() const { return potential_; }
    inline auto& potential() { return potential_; }

    template<typename... Args>
    inline void add_charge(Args&&... args) {
        potential_.add_charge(
          potentials::Electrostatic::charge_type(std::forward<Args>(args)...));
    }

    inline ElectronNuclearCoulomb() = default;

    inline ElectronNuclearCoulomb& operator=(
      const ElectronNuclearCoulomb& other) {
        potential_ = other.potential_;
        return *this;
    }

    inline ElectronNuclearCoulomb& operator=(
      ElectronNuclearCoulomb&& other) noexcept {
        potential_ = std::move(other.potential_);
        return *this;
    }

    inline ElectronNuclearCoulomb(const ElectronNuclearCoulomb& other) {
        *this = other;
    }

    inline ElectronNuclearCoulomb(ElectronNuclearCoulomb&& other) noexcept {
        *this = std::move(other);
    }

protected:
    inline void hash_impl(sde::Hasher& h) const override {
        return h(potential_);
    }

    inline bool is_equal_impl(const Operator& other) const noexcept override {
        auto ptr = dynamic_cast<const ElectronNuclearCoulomb*>(&other);
        if(!ptr) return false;
        return *this == *ptr;
    }

private:
    /// Electrostatic potential induced by the nuclei
    potentials::Electrostatic potential_;
    // TODO: Need to generalize Electrostatic to take differing charge models
};
#endif





/// Enumerate gauge types
enum class GaugeType {
    length   = 0,
    velocity = 1
};

/// Multipole operator
template <std::size_t L, typename ParticleType, 
  ShellType Basis = ShellType::cartesian, 
  GaugeType Gauge = GaugeType::length>
class Multipole : public Operator {
    friend struct Operator;

public:
    using point_type = Point<double>;

    static constexpr auto n_electrons = detail_::n_electrons_v<ParticleType>;
    static constexpr auto n_nuclei    = detail_::n_nuclei_v<ParticleType>;

    //static constexpr std:size_t op_size =
    //  Basis == ShellType::cartesian ? ((L+1)*(L+2)/2) : (2*L+1);

    inline bool operator==( const Multipole& other ) const { 
        return gauge_origin_ == other.gauge_origin_;
    }
    inline bool operator!=( const Multipole& other ) const {
        return not(*this == other);
    }

    Multipole( point_type p ) : gauge_origin_(p) { }
    Multipole() : Multipole( point_type(0.,0.,0.) ) { }

    Multipole( const Multipole& other ) :
      gauge_origin_(other.gauge_origin_) {}
    Multipole( Multipole&& other ) noexcept :
      gauge_origin_(std::move(other.gauge_origin_)) {}

    ~Multipole() noexcept = default;

    inline const auto& gauge_origin() const { return gauge_origin_; }
    inline auto& gauge_origin() { return gauge_origin_; }

protected:
    inline void hash_impl( sde::Hasher& h ) const override {
        return h(gauge_origin);
    }

    inline bool is_equal_impl( const Operator& other ) const noexcept override {
        auto ptr = dynamic_cast<const Multipole*>(&other);
        if(!ptr) return false;
        return *this == *ptr;
    }

private:
    /// Gauge origin for the multipole operator
    point_type gauge_origin_;
};

/// Electric dipole operator type
using ElectricDipoleOperator     = Multipole<1,Electron>;
/// Electric quadrupole operator type
using ElectricQuadrupoleOperator = Multipole<2,Electron>;
/// Electric octupole operator type
using ElectricOctupoleOperator   = Multipole<3,Electron>;





template <typename ParamType,typename P1, typename P2>
class ParameterizedCoulombInteraction : public CoulombInteraction<P1,P2> {
    using base_type = CoulombInteraction<P1,P2>;
    friend struct Operator;

public:
    static constexpr auto n_electrons = detail_::n_electrons_v<P1,P2>;
    static constexpr auto n_nuclei    = detail_::n_nuclei_v<P1,P2>;

    inline bool operator==(const ParameterizedCoulombInteraction& other) const {
        const auto& this_as_coul = static_cast<const base_type&>(*this);
        const auto& othr_as_coul = static_cast<const base_type&>(other);
        return (this_as_coul == othr_as_coul) and (params_ == other.params_);
    }
    inline bool operator!=(const ParameterizedCoulombInteraction& other) const {
        return not((*this) == other);
    }

    inline const auto& params() const { return params_; }
    inline auto& params() { return params_; }

    ParameterizedCoulombInteraction() = default;
    virtual ~ParameterizedCoulombInteraction() noexcept = default;
    
    ParameterizedCoulombInteraction& operator=(
      const ParameterizedCoulombInteraction& other) {
        params_ = other.params_;
        return *this;
    }

    ParameterizedCoulombInteraction& operator=(
      ParameterizedCoulombInteraction&& other) noexcept {
        params_ = std::move(other.params_);
        return *this;
    }


    ParameterizedCoulombInteraction( const ParameterizedCoulombInteraction& other ) {
        *this = other;
    }
    ParameterizedCoulombInteraction( ParameterizedCoulombInteraction&& other ) noexcept {
        *this = std::move(other);
    }

protected:
    virtual void hash_impl( sde::Hasher& h ) const override {
        return h(params_);
    }

    /// Parameters used to define and modulate the Coulomb Interaction
    ParamType params_;
};


#define REGISTER_PARAM_COU_OPERATOR(OP_NAME,PARAM_TYPE,PARAM_NAME,...)      \
struct OP_NAME :                                                            \
  public ParameterizedCoulombInteraction<PARAM_TYPE,__VA_ARGS__> {          \
private:                                                                    \
    using base_type =                                                       \
      ParameterizedCoulombInteraction<PARAM_TYPE,__VA_ARGS__>;              \
    friend struct Operator;                                                 \
                                                                            \
public:                                                                     \
    static const auto n_electrons = base_type::n_electrons;                 \
    static const auto n_nuclei    = base_type::n_nuclei;                    \
                                                                            \
    inline bool operator==(const OP_NAME& other) const {                    \
        const auto& this_as_base = static_cast<const base_type&>(*this);    \
        const auto& othr_as_base = static_cast<const base_type&>(other);    \
        return (this_as_base == othr_as_base);                              \
    }                                                                       \
                                                                            \
    inline bool operator!=(const OP_NAME& other) const {                    \
        return not((*this) == other);                                       \
    }                                                                       \
                                                                            \
    inline const auto& PARAM_NAME() const { return this->params_; }         \
    inline auto& PARAM_NAME() { return this->params_; }                     \
                                                                            \
    OP_NAME() = default;                                                    \
    virtual ~OP_NAME() noexcept = default;                                  \
    OP_NAME( const OP_NAME& ) = default;                                    \
    OP_NAME( OP_NAME&& ) noexcept = default;                                \
                                                                            \
    inline OP_NAME& operator=( const OP_NAME& other ) {                     \
        base_type::operator=(other);                                        \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline OP_NAME& operator=( OP_NAME&& other ) noexcept {                 \
        base_type::operator=(std::move(other));                             \
        return *this;                                                       \
    }                                                                       \
                                                                            \
protected:                                                                  \
    virtual                                                                 \
    inline                                                                  \
    bool is_equal_impl(const Operator& other) const noexcept override {     \
        auto ptr = dynamic_cast<const OP_NAME*>(&other);                    \
        if(!ptr) return false;                                              \
        return *this == *ptr;                                               \
    }                                                                       \
};


REGISTER_PARAM_COU_OPERATOR(ElectronNuclearCoulomb,potentials::Electrostatic,
  potential,Electron,Nucleus);

REGISTER_PARAM_COU_OPERATOR(SlaterTypeGeminal,double,exponent,Electron,Electron);
REGISTER_PARAM_COU_OPERATOR(YukawaPotential,  double,exponent,Electron,Electron);

#undef REGISTER_PARAM_COU_OPERATOR


} // namespace libchemist
