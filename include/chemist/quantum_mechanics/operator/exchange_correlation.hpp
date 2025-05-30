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

/** @brief List of standardized XC functionals
 *
 *  In theory there are two ways to specify an XC functional:
 *  1. Provide the explicit functional form including parameters.
 *  2. Use one that has been "standardized" and named, and provide the name.
 *
 *  Overwhelmingly people tend to number 2. To aid in this effort we provide a
 *  list of commonly encountered "standardized" functionals. Users can refer to
 *  any of these functionals via the enum. If a user wants to use a functional,
 *  standardized or not, for which we do not have enum they can specify "CUSTOM"
 *  and provide the ExchangeCorrelation class with the functional form
 *  (currently not supported).
 *
 *  @note The quotes around standardized are because they aren't as standardized
 *        as people think they are...
 */
enum class xc_functional {
    NONE,
    CUSTOM,
    SVWN3,
    SVWN5,
    BLYP,
    B3LYP,
    PBE,
    revPBE,
    PBE0
};

/** @brief Operator defining the exchange-correlation (XC) interaction.
 *
 *  @tparam LHSParticle The type of particle interacting with the XC potential.
 *  @tparam RHSParticle The type of particle defining the XC potential.
 */
template<typename LHSParticle, typename RHSParticle>
class ExchangeCorrelation
  : public detail_::OperatorImpl<ExchangeCorrelation<LHSParticle, RHSParticle>,
                                 LHSParticle, RHSParticle> {
private:
    /// Type of *this
    using my_type = ExchangeCorrelation<LHSParticle, RHSParticle>;

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

    /// Type used to specify the form of the XC functional
    using functional_type = xc_functional;

    /** @brief Creates an operator for the XC potential.
     *
     *  @param[in] func_name The name of the standardized XC functional to use.
     *  @param[in] p0 The particle interacting with the potential.
     *  @param[in] p1 The particle defining the potential.
     */
    explicit ExchangeCorrelation(
      functional_type func_name = xc_functional::NONE,
      lhs_value_type p0         = lhs_value_type{},
      rhs_value_type p1         = rhs_value_type{}) noexcept;

    /// All implemented by OperatorImpl. This exposes them to the user.
    ///@{
    ExchangeCorrelation(const ExchangeCorrelation&)         = default;
    ExchangeCorrelation(ExchangeCorrelation&& rhs) noexcept = default;
    ExchangeCorrelation& operator=(const ExchangeCorrelation&) = default;
    ExchangeCorrelation& operator=(ExchangeCorrelation&& rhs) noexcept =
      default;
    ///@}

    /// Defaulted, nothrow dtor
    ~ExchangeCorrelation() noexcept = default;

    /** @brief Returns the lhs particle
     *
     *  The ExchangeCorrelation energy is a two particle operator. We term the
     * first particle the lhs particle (short for left hand side). This method
     * can be used to retrieve the lhs particle.
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
     *  The ExchangeCorrelation energy is a two particle operator. We term the
     * second particle the rhs particle (short for right hand side). This method
     * can be used to retrieve the rhs particle.
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

    /** @brief The name of the XC functional.
     *
     *  This will be something like PBE or BLYP.
     *
     *  @return A mutable reference to the name of the XC functional.
     *
     *  @throw None No throw guarantee.
     */
    auto& functional_name() noexcept { return m_xc_form_; }

    /** @brief The name of the XC functional.
     *
     *  Same as the non-const version, except the result is read-only.
     *
     *  @return The name of the XC functional.
     *
     *  @throw None No throw guarantee.
     */
    auto functional_name() const noexcept { return m_xc_form_; }

private:
    /// The name of the XC functional's form.
    functional_type m_xc_form_;
};

extern template class ExchangeCorrelation<Electron, chemist::Density<Electron>>;
extern template class ExchangeCorrelation<ManyElectrons,
                                          chemist::Density<Electron>>;
extern template class ExchangeCorrelation<Electron,
                                          DecomposableDensity<Electron>>;
extern template class ExchangeCorrelation<ManyElectrons,
                                          DecomposableDensity<Electron>>;

} // namespace chemist::qm_operator