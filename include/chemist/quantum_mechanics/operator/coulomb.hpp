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

template<typename LHSParticle, typename RHSParticle>
class Coulomb : public detail_::OperatorImpl<Coulomb<LHSParticle, RHSParticle>,
                                             LHSParticle, RHSParticle> {
private:
    /// Type of *this
    using my_type = Coulomb<LHSParticle, RHSParticle>;

    /// Type *this directly derives from
    using base_type = detail_::OperatorImpl<my_type, LHSParticle, RHSParticle>;

public:
    /// Pull's base class's types into view and makes them easier to reference
    ///@{
    using lhs_value_type = typename base_type::template particle_type<0>;
    using lhs_reference  = typename base_type::template particle_reference<0>;
    using const_lhs_reference =
      typename base_type::template const_particle_reference<0>;
    using rhs_value_type = typename base_type::template particle_type<1>;
    using rhs_reference  = typename base_type::template particle_reference<1>;
    using const_rhs_reference =
      typename base_type::template const_particle_reference<1>;
    ///@}

    explicit Coulomb(lhs_value_type p0 = lhs_value_type{},
                     rhs_value_type p1 = rhs_value_type{}) noexcept;

    /// All implemented by OperatorImpl. This exposes them to the user.
    ///@{
    Coulomb(const Coulomb&)                    = default;
    Coulomb(Coulomb&& rhs) noexcept            = default;
    Coulomb& operator=(const Coulomb&)         = default;
    Coulomb& operator=(Coulomb&& rhs) noexcept = default;
    ///@}

    /// Defaulted, nothrow dtor
    ~Coulomb() noexcept = default;

    /** @brief Returns the lhs particle
     *
     *  The Coulomb energy is a two particle operator. We term the first
     *  particle the lhs particle (short for left hand side). This method can be
     *  used to retrieve the lhs particle.
     *
     *  This is a convenience function for calling the base class's at member.
     *
     *  @return A mutable reference to the lhs particle.
     *
     *  @throw None No throw guarantee.
     */
    lhs_reference lhs_particle() { return base_type::template at<0>(); }

    /** @brief Returns a read-only reference to the lhs particle.
     *
     *  This method is the same as the non-const version except that the result
     *  is read-only.
     *
     *  @return A read-only reference to the lhs particle.
     *
     *  @throw None No throw guarantee.
     */
    const_lhs_reference lhs_particle() const {
        return base_type::template at<0>();
    }

    /** @brief Returns the rhs particle
     *
     *  The Coulomb energy is a two particle operator. We term the second
     *  particle the rhs particle (short for right hand side). This method can
     *  be used to retrieve the rhs particle.
     *
     *  This is a convenience function for calling the base class's at member.
     *
     *  @return A mutable reference to the rhs particle.
     *
     *  @throw None No throw guarantee.
     */
    rhs_reference rhs_particle() { return base_type::template at<1>(); }

    /** @brief Returns a read-only reference to the rhs particle.
     *
     *  This method is the same as the non-const version except that the result
     *  is read-only.
     *
     *  @return A read-only reference to the rhs particle.
     *
     *  @throw None No throw guarantee.
     */
    const_rhs_reference rhs_particle() const {
        return base_type::template at<1>();
    }
};

extern template class Coulomb<Electron, Electron>;
extern template class Coulomb<ManyElectrons, ManyElectrons>;
extern template class Coulomb<Electron, chemist::Density<Electron>>;
extern template class Coulomb<ManyElectrons, chemist::Density<Electron>>;
extern template class Coulomb<Electron, DecomposableDensity<Electron>>;
extern template class Coulomb<ManyElectrons, DecomposableDensity<Electron>>;
extern template class Coulomb<Electron, Nuclei>;
extern template class Coulomb<ManyElectrons, Nuclei>;
extern template class Coulomb<Nuclei, Nuclei>;

} // namespace chemist::qm_operator