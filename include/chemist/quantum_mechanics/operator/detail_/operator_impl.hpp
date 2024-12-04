/*
 * Copyright 2024 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <chemist/density/density.hpp>
#include <chemist/electron/electron.hpp>
#include <chemist/nucleus/nucleus.hpp>
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>
#include <chemist/traits/electron_traits.hpp>
#include <chemist/traits/nucleus_traits.hpp>
#include <tuple>
#include <type_traits>
#include <utilities/dsl/dsl.hpp>

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
 *  @tparam DerivedType the type *this is implementing. Needed for CRTP.
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

    /// Helper to determine if @p T is a ManyElectrons
    template<typename T>
    static constexpr auto is_many_electrons_v =
      std::is_same_v<T, ManyElectrons>;

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

    /// Disables a method if @p T is the same type as *this
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
     *  *this. Note that an operator like `Kinetic<ManyElectrons>` is considered
     *  a one-electron operator in this sense because it contains one
     *  determinable electron as well as an indeterminable (at compile time)
     *  number of electrons.
     *
     *  @return The number of determinable electrons in *this.
     *
     *  @throw None No throw guarantee.
     */
    static constexpr auto n_electrons() {
        return (is_electron_v<particle_type_<Particles>> + ...) +
               (is_many_electrons_v<particle_type_<Particles>> + ...);
    }

    /** @brief Returns the @p i-th particle in *this.
     *
     *  This method returns a mutable reference to the @p i-th particle involved
     *  in this operator.
     *
     *  @tparam i The offset of the particle. @p i should be in the range 0 to
     *          sizeof...(Particles).
     *
     *  @throw None No throw guarantee.
     */
    template<std::size_t i>
    particle_reference<i> at() {
        return std::get<i>(m_particles_);
    }

    /** @brief REturns the @p i-th particle in *this.
     *
     *  This method is the same as the non-const version except that the
     *  resulting particle is read-only. See the documentation for the non-const
     *  version for more details.
     *
     *  @tparam i The offset of the particle. @p i should be in the range 0 to
     *          sizeof...(Particles).
     *
     *  @throw None No throw guarantee.
     */
    template<std::size_t i>
    const_particle_reference<i> at() const {
        return std::get<i>(m_particles_);
    }

    /** @brief Non-polymorphically determines if *this is value equal to @p rhs.
     *
     *  This method compares the particles in *this to the particles in @p rhs.
     *  The particles must be stored in the same order and
     *  `at<i>() == rhs.at<i>()` must be true for all i.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const OperatorImpl& rhs) const noexcept {
        return m_particles_ == rhs.m_particles_;
    }

    /** @brief Non-polymorphically determines if *this is different than @p rhs.
     *
     *  This class defines "different" as being "not value equal." See the
     *  documentation for operator== for the definition of value equal.
     *
     *  @param[in] rhs The object to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const OperatorImpl& rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Adds @p rhs to *this.
     *
     *  @tparam RHSType The type of @p rhs. Assumed to be the type of an
     *                  operator or the type of a DSL term involving operators.
     *
     *  @param[in] rhs The object to add to *this.
     *
     *  @return A DSL term describing the user's requested operation.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSType>
    auto operator+(const RHSType& rhs) const {
        return utilities::dsl::Add<const DerivedType, const RHSType>(
          downcast_(), rhs);
    }

    /** @brief Subtracts @p rhs from *this.
     *
     *  @tparam RHSType The type of @p rhs. Assumed to be the type of an
     *                  operator or the type of a DSL term involving operators.
     *
     *  @param[in] rhs The object to subtract from *this.
     *
     *  @return A DSL term describing the user's requested operation.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSType>
    auto operator-(const RHSType& rhs) const {
        return utilities::dsl::Subtract<const DerivedType, const RHSType>(
          downcast_(), rhs);
    }

    /** @brief Right multiplies *this by @p rhs.
     *
     *  @tparam RHSType The type of @p rhs. Assumed to be the type of an
     *                  operator or the type of a DSL term involving operators.
     *
     *  @param[in] rhs The object to multiply with *this.
     *
     *  @return A DSL term describing the user's requested operation.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSType>
    auto operator*(const RHSType& rhs) const {
        return utilities::dsl::Multiply<const DerivedType, const RHSType>(
          downcast_(), rhs);
    }

protected:
    /// Type *this derives from
    using base_type = OperatorBase;

    /// Pull in types defined in base
    ///@{
    using typename base_type::base_pointer;
    using typename base_type::const_base_reference;
    ///@}

    /** @brief Creates a new interaction involving the provided particles
     *
     *  @tparam ParticlesIn The qualified types of the input particles. The i-th
     *                      type in @p ParticlesIn must be implicitly
     *                      convertible to the i-th type in @p Particles.
     *  @tparam <anonymous> A template type parameter used to disable this
     *                      method via SFINAE if any of the types in
     *                      @p ParticlesIn is the same type as *this (in which
     *                      case the user probably wanted the copy ctor).
     *
     *  This method is used to create a new operator by either copying or moving
     *  existing particles.
     *
     *  @param[in] particles The values for the particles whose interactions are
     *                       being described by *this.
     *
     *  @throw ??? if we need to copy any of the values in @p particles and if
     *             the copy constructor throws. Same throw guarantee as the
     *             object's copy constructor.
     */
    template<typename... ParticlesIn,
             typename = std::tuple<disable_if_me_t<ParticlesIn>...>>
    OperatorImpl(ParticlesIn&&... particles) noexcept :
      m_particles_(std::forward<ParticlesIn>(particles)...) {}

    /// Defaulted copy ctor, just deep copies the particles in @p other.
    OperatorImpl(const OperatorImpl& other) = default;

    /// Defaulted move ctor, just moves the particles from @p other
    OperatorImpl(OperatorImpl&& other) noexcept = default;

    /// Defaulted copy assignment, just copies the particles from @p rhs
    OperatorImpl& operator=(const OperatorImpl& rhs) = default;

    /// Defaulted move assignment, just moves the particles from @p rhs
    OperatorImpl& operator=(OperatorImpl&& rhs) noexcept = default;

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

    /** @brief Implements are_equal.
     *
     *  This common implementation of are_equal first ensures that @p other can
     *  be downcast to the same OperatorImpl type (which through CRTP also
     *  ensures that the derived types of *this and @p other are the same
     *  type). Then we call the derived class's operator== to do the value
     *  comparison.
     *
     *  @param[in] other The operator to compare to.
     *
     *  @return True if *this and @p other are polymoprhically value equal and
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal_(const_base_reference other) const noexcept override {
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

/** @brief Enables left multiplication by floating point values.
 *
 *  @tparam LHSType Type of the floating point value.
 *  @tparam DerivedType Type OperatorImpl is implementing.
 *  @tparam Particles Types of the particles DerivedType is templated on.
 *  @tparam <anonymous> Used to disable this overload if @p LHSType is not a
 *                      floating point type.
 *
 *  It is conventional to put scalars on the left of an operator. This function
 *  overloads left multiplication of an operator by a scalar so that it works.
 */
template<typename LHSType, typename DerivedType, typename... Particles,
         typename = std::enable_if_t<std::is_floating_point_v<LHSType>>>
auto operator*(const LHSType& lhs,
               const OperatorImpl<DerivedType, Particles...>& rhs) {
    return utilities::dsl::Multiply<const LHSType, const DerivedType>(
      lhs, static_cast<const DerivedType&>(rhs));
}

} // namespace chemist::qm_operator::detail_