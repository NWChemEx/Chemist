#pragma once
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>
#include <chemist/traits/electron_traits.hpp>
#include <tuple>
#include <type_traits>

namespace chemist::qm_operator::detail_ {

/** @brief Operator code factorization requiring particle types.
 *
 *  The OperatorBase class provides code factorization and interfaces for when
 *  the particles described by the operator do not need to be known. There is
 *  additional code factorization that is possible when you know the types (for
 *  example determining if something is a one or two electron operator). That's
 *  the responsibility of this class.
 *
 *  @note While we refer to each type in @p Particles as "a particle" the
 *        reality is it can actually be an indeterminate number of particles
 *        (e.g., `ManyElectrons`) or a non-particle object (e.g.,
 *         `Density<Electron>`). We opted for "particle" because it's mostly
 *        right in most cases and "object" seemed too generic.
 *
 *  @tparam Particles A list of the interacting particles the operator
 *                    describes.
 */
template<typename DerivedType, typename... Particles>
class OperatorImpl : public OperatorBase {
private:
    /// Type of *this
    using my_type = OperatorImpl<DerivedType, Particles...>;

    /// Helper to determine if @p T is an Electron
    template<typename T>
    static constexpr auto is_electron_v = std::is_same_v<T, Electron>;

    /// Qualified type of the i-th Particle
    template<std::size_t i>
    using i_type = std::tuple_element_t<i, std::tuple<Particles...>>;

    /// Traits type for a particle of type @p T
    template<typename T>
    using traits_type = ChemistClassTraits<T>;

    /// Traits for the i-th Particle type
    template<std::size_t i>
    using i_traits_type = traits_type<i_type<i>>;

    /// Unqualified type of a particle of type @p T
    template<typename T>
    using particle_type_ = typename traits_type<T>::value_type;

    template<typename T>
    using disable_if_me_t = std::enable_if_t<!std::is_same_v<T, my_type>>;

public:
    /// Unqualified type of the i-th particle
    template<std::size_t i>
    using particle_type = typename i_traits_type<i>::value_type;

    /// Type acting like a reference to the i-th particle
    template<std::size_t i>
    using particle_reference = typename i_traits_type<i>::view_type;

    /// Type acting like a read-only reference to the i-th particle.
    template<std::size_t i>
    using const_particle_reference = typename i_traits_type<i>::const_view_type;

    /** @brief The determinable number of electrons in *this
     *
     *  Some operators, like the one-electron kinetic energy operator, or the
     *  two-electron electron-electron repulsion operator involve a fixed number
     *  of electrons. This function determines the fixed number of electrons in
     *  *this.
     *
     *  @return The number of
     */
    static constexpr auto n_electrons() {
        return (is_electron_v<particle_type_<Particles>> + ...);
    }

    template<std::size_t i>
    particle_reference<i> at() {
        return std::get<i>(m_particles_);
    }

    template<std::size_t i>
    const_particle_reference<i> at() const {
        return std::get<i>(m_particles_);
    }

    bool operator==(const OperatorImpl& rhs) const noexcept {
        return m_particles_ == rhs.m_particles_;
    }

    bool operator!=(const OperatorImpl& rhs) const noexcept {
        return !(*this == rhs);
    }

protected:
    /// Type *this derives from
    using base_type = OperatorBase;

    /// Pull in base types.
    using typename base_type::base_pointer;
    using typename base_type::const_base_reference;

    template<typename... ParticlesIn,
             typename = std::tuple<disable_if_me_t<ParticlesIn>...>>
    OperatorImpl(ParticlesIn&&... particles) noexcept :
      m_particles_(std::forward<ParticlesIn>(particles)...) {}

    OperatorImpl(const OperatorImpl& other) = default;

    /// Implements clone by calling derived class's copy ctor
    typename base_type::base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements visit by passing the derived class to visitor.run
    //@{
    void visit_(visitor_reference visitor) override {
        visitor.run(downcast_());
    }
    void visit_(visitor_reference visitor) const override {
        visitor.run(downcast_());
    }
    //@}

    bool are_equal_(const_base_reference other) {
        auto pother = dynamic_cast<const my_type*>(&other);
        if(pother == nullptr) return false; // Different types
        return downcast_() == pother->downcast_();
    }

private:
    /// Wraps the process of getting a mutable instance of the derived class
    auto& downcast_() { return static_cast<DerivedType&>(*this); }

    /// Wraps the process of getting a read-only instance of the derived class
    const auto& downcast_() const {
        return static_cast<const DerivedType&>(*this);
    }
    /// These are the particles the operator describes
    std::tuple<Particles...> m_particles_;
};

} // namespace chemist::qm_operator::detail_