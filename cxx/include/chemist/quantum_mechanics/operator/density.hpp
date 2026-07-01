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
#include <chemist/quantum_mechanics/wavefunction/determinant.hpp>

namespace chemist::qm_operator {

/** @brief  An operator for the probability density of a series of particles.
 *
 *  @tparam OrbitalsType The type of the vector space spanned by the one-
 *                       particle basis functions.
 *  @tparam Particles The particles described by this operator.
 *
 *  In quantum mechanics, the density operator is a projection operator that
 *  projects onto the state of the system. For the state @f$\ket{\Psi}@f$ this
 *  has the form:
 *  @f[
 *     \widehat{\rho} = \ket{\Psi}\bra{\Psi}.
 *  @f]
 *  In the more general case when @f$\ket{Psi}@f$ is an ensemble of @f$m@f$
 *  states this becomes:
 *  @f[
 *      \widehat{\rho} = \sum_{i=1}^m w_i\ket{\psi_i}\bra{\psi_i},
 *  @f]
 *  where @f$w_i@f$ is the weight of the @f$i@f$-th state, @$\bra{\psi_i}@f$, in
 *  the ensemble.
 *
 */
template<typename OrbitalsType, typename... Particles>
class Density
  : public detail_::OperatorImpl<Density<OrbitalsType, Particles...>,
                                 Particles...> {
private:
    /// Type of *this
    using my_type = Density<OrbitalsType, Particles...>;

    /// Type of the implementation
    using impl_type = detail_::OperatorImpl<my_type, Particles...>;

public:
    /// Pull in base class's types
    ///@{
    using typename impl_type::base_pointer;
    using typename impl_type::const_base_reference;
    using typename impl_type::visitor_reference;
    ///@}

    /// The type used to store the weight of each state
    using weight_type = double;

    /// The type used to store the weights of each state
    using weight_vector_type = std::vector<weight_type>;

    /// Type acting like a mutable reference to an object of type weight_vector
    using weight_vector_reference = weight_vector_type&;

    /// Type acting lik a read-only reference to an object of type weight_vector
    using const_weight_vector_reference = const weight_vector_type&;

    /// The type of one-particle functions comprising *this
    using orbitals_type = OrbitalsType;

    /// Type acting like a reference to a mutable orbitals_type object
    using orbitals_reference = orbitals_type&;

    /// Type acting like a reference to a read-only orbitals_type object
    using const_orbitals_reference = const orbitals_type&;

    /// Type used for indexing and offsets
    using size_type = typename weight_vector_type::size_type;

    /** @brief Creates the density of an empty system.
     *
     *  Objects created by this ctor are usually placeholders and wil have their
     *  state assigned to them later.
     *
     *  @throw None No throw guarantee.
     */
    Density() noexcept = default;

    /** @brief Creates a density operator with the provided ensemble.
     *
     *  This ctor creates a density operator where the @f$i@f$-th projection is
     *  `weights[i] * orbitals[i] * orbitals[i].adjoint()`.
     *
     *  @param[in] orbitals The basis set the density operator is expressed in.
     *  @param[in] weights The weight of each basis function in the ensemble.
     *
     *  @throw std::runtime_error if the size of orbitals is not the same as
     *                            the size of weights.
     */
    Density(orbitals_type orbitals, weight_vector_type weights) :
      m_weights_(std::move(weights)), m_orbitals_(std::move(orbitals)) {
        if(m_weights_.size() != m_orbitals_.size())
            throw std::runtime_error("Must provide one weight per orbital");
    }

    /** @brief Creates a density operator with the provided ensemble
     *
     *  @tparam T The input type of the weights
     *
     *  In many cases the weights being used to initialize *this are not stored
     *  as `weight_type` objects (e.g., when weights map to occupations they are
     *  often integers). This ctor facilitates converting non-`weight_type`
     *  weights into `weight_type` weights. This ctor converts @p weights and
     *  then dispatches to the value ctor.
     *
     *  @param[in] orbitals The one-electron wavefunctions *this is based on.
     *  @param[in] weights The weights of each orbital in the ensemble.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        the converted weights. Strong throw guarantee.
     */
    template<typename T>
    Density(orbitals_type orbitals, std::vector<T> weights) :
      Density(std::move(orbitals),
              weight_vector_type(weights.begin(), weights.end())) {}

    /** @brief Forms the density from the many-particle wavefunction.
     *
     *  The `m` particle wavefunction can be converted to an
     *  `n=sizeof...(Particles)` particle density by taking the inner product
     *  over the `m-n` particles that are not part of the density. This ctor
     *  initializes *this by contracting over the `m-n` particles.
     *
     *  @param[in] wf The many-particle wavefunction of the system.
     *
     *  @throw std::bad_alloc if there is a problem copying the state from
     *                        @p wf. Strong throw guarantee.
     */
    explicit Density(const wavefunction::Determinant<orbitals_type>& wf) :
      Density(wf.orbitals(), wf.occupations()) {
        // I think what we have only works for the one-particle density operator
        static_assert(sizeof...(Particles) == 1);
    }

    // -------------------------------------------------------------------------
    // -- Getters and Setters
    // -------------------------------------------------------------------------

    /** @brief Provides mutable access to the orbitals.
     *
     *  This method can be used to inspect and modify the orbitals forming
     *  *this. If you add/remove an orbital you are expected to add/remove the
     *  corresponding weight as well. This method will NOT check that this is
     *  the case.
     *
     *  @return The orbitals forming *this.
     *
     *  @throw None No throw guarantee.
     */
    orbitals_reference orbitals() noexcept { return m_orbitals_; }

    /** @brief Provides read-only access to the orbitals.
     *
     *  This method is the same as the non-const version except that the result
     *  is immutable.
     *
     *  @return The orbitals forming *this.
     *
     *  @throw None No throw guarantee.
     */
    const_orbitals_reference orbitals() const noexcept { return m_orbitals_; }

    /** @brief Provides mutable access to the weights of the ensemble.
     *
     *  This method provides mutable access to the weights of the ensemble. If
     *  you add/remove a weight you are expected to also add/remove the
     *  corresponding orbital.
     *
     *  @return The weights of the states forming *this.
     *
     *  @throw None No throw guarantee.
     */
    weight_vector_reference weights() noexcept { return m_weights_; }

    /** @brief Provides read-only access to the weights of the ensemble.
     *
     *  This method is the same as the non-const version except that the
     *  resulting weights are immutable.
     *
     *  @return The weights of the ensemble forming *this.
     *
     *  @throw None No throw guarantee.
     */
    const_weight_vector_reference weights() const noexcept {
        return m_weights_;
    }

    /** @brief Returns the number of states in *this.
     *
     *  This method is used to determine the number of states forming this
     *  density operator.
     *
     *  @return The number of states comprising the ensemble.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_orbitals_.size(); }

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two density operators are value equal if they contain the same number
     *  of states in the ensemble, if the @f$i@f$-th weight in *this is value
     *  equal to the @f$i@f$-ith weight in @p rhs, and if the @f$i@f$-th state
     *  in *this is value equal to the @f$i@f$-th state in *this.
     *
     *  @param[in] rhs The operator to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Density& rhs) const noexcept {
        return std::tie(m_weights_, m_orbitals_) ==
               std::tie(rhs.m_weights_, rhs.m_orbitals_);
    }

    /** @brief Determines if *this is different from @p rhs.
     *
     *  This class defines different as "not value equal." See the description
     *  for operator== for the definition of value equality.
     *
     *  @param[in] rhs The density operator to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Density& rhs) const noexcept {
        return !((*this) == rhs);
    }

private:
    /// Ensemble weights of the orbitals
    weight_vector_type m_weights_;

    /// The orbitals
    orbitals_type m_orbitals_;
};

} // namespace chemist::qm_operator
