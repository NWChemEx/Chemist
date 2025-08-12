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
#include <chemist/quantum_mechanics/operator/detail_/operator_impl.hpp>

namespace chemist::qm_operator {

/** @brief Describes the kinetic energy of a particle of type @p ParticleType.
 *
 *  This class is essentially a strong type to signify that a one-particle
 *  operator describes the kinetic energy of the particle.
 *
 *  @tparam ParticleType The type of the particle. Assumed to be Electron,
 *                       Nucleus or a type representing an indeterminate number
 *                       of such particles.
 */
template<typename ParticleType>
class Kinetic
  : public detail_::OperatorImpl<Kinetic<ParticleType>, ParticleType> {
private:
    /// Type of *this
    using my_type = Kinetic<ParticleType>;

    /// Type *this directly derives from.
    using base_type = detail_::OperatorImpl<my_type, ParticleType>;

public:
    /// Make base class's types visible and conventional
    ///@{
    using value_type = typename base_type::template particle_type<0>;
    using reference  = typename base_type::template particle_reference<0>;
    using const_reference =
      typename base_type::template const_particle_reference<0>;
    ///@}

    /** @brief Creates a new Kinetic object describing the kinetic energy of
     *         @p particle.
     *
     *  The resulting object will describe the kinetic energy of @p particle.
     *
     *  @param[in] particle The particle whose kinetic energy is described by
     *                      *this.
     *
     *  @throw None No throw guarantee.
     */
    explicit Kinetic(value_type particle = value_type{}) noexcept;

    /// All are implemented by OperatorImpl. This exposes them to the user.
    ///@{
    Kinetic(const Kinetic&)                    = default;
    Kinetic(Kinetic&& rhs) noexcept            = default;
    Kinetic& operator=(const Kinetic&)         = default;
    Kinetic& operator=(Kinetic&& rhs) noexcept = default;
    ///@}

    /// Defaulted, nothrow dtor
    ~Kinetic() noexcept = default;

    /** @brief Returns the particle
     *
     *  The kinetic energy is a single particle operator. This method can be
     *  used to retrieve the particle whose kinetic energy is described by
     *  *this.
     *
     *  This is a convenience function for calling the base class's at member.
     *
     *  @return A mutable reference to the particle.
     *
     *  @throw None No throw guarantee.
     */
    reference particle() { return base_type::template at<0>(); }

    /** @brief Returns a read-only reference to the particle.
     *
     *  This method is the same as the non-const version except that the result
     *  is read-only.
     *
     *  @return A read-only reference to the particle.
     *
     *  @throw None No throw guarantee.
     */
    const_reference particle() const { return base_type::template at<0>(); }
};

extern template class Kinetic<Electron>;
extern template class Kinetic<ManyElectrons>;
extern template class Kinetic<Nucleus>;
extern template class Kinetic<Nuclei>;
} // namespace chemist::qm_operator
