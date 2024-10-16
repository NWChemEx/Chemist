/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/quantum_mechanics/wavefunction/transformed.hpp>

namespace chemist {

/** @brief Class describing a decomposable density.
 *
 *  In quantum mechanics, densities can be created from operators like
 *  @f$\ket{\psi}\bra{\psi}@f$ the tensor representation of which in the
 *  @f${\phi}@f$ basis looks like:
 *  @f$\braket{\phi'\mid\psi}\braket{\psi\mid\phi}=CC^\dagger@f$ where
 *  @f$C_{\phi\psi}=\braket{\phi'\mid\psi}@f$. The point being, when the
 *  @f${\phi}\rightarrow{\psi}@f$ transformation is known, the density
 *  decomposition in terms of the @f${\psi}@f$ basis is also known. In many
 *  cases the @f${\psi}@f$ basis is smaller and algorithms can exploit this
 *  fact to reduce the number of FLOPs that must be done by working with the
 *  factors instead of the density. This class contains the factors of the
 *  decomposition.
 *
 *  Following from the above discussion, this class establishes the convention
 *  that the matrix formed from the basis vectors of @f${\psi}@f$ as column
 *  vectors is the left factor and that the matrix with the basis vectors of
 *  @f${\psi}@f$ as the row vectors is the right factor.
 *
 *  @tparam Particles The types of the particles described by this density.
 *                    At the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 */
template<typename... Particles>
class DecomposableDensity : public Density<Particles...> {
private:
    /// Type of base Density
    using base_type = Density<Particles...>;

public:
    /// Pull in base class's types
    ///@{
    using typename base_type::basis_type;
    using typename base_type::const_basis_reference;
    using typename base_type::const_reference;
    using typename base_type::value_type;
    ///@}

    /// Type of a transformation from a space of type basis_type
    using transformed_basis_type = wavefunction::Transformed<basis_type>;

    /// A read-only reference to an object of type transformed_basis_type
    using const_transformed_basis_reference = const transformed_basis_type&;

    /// Default ctor
    explicit DecomposableDensity() = default;

    /** @brief A density which has been factored in the @p space space.
     *
     *  This is a convenience ctor to initialize a factorized density given
     *  the density and the space it can be factorized into.
     *
     *  @param[in] rho The representation of the density in the basis_type
     *                 basis.
     *  @param[in] space A space which is obtained by transforming from the
     *                   basis_type basis.
     *
     *  @throw std::bad_alloc if there is a problem copying the state from
     *                        @p space. Strong throw guarantee.
     */
    DecomposableDensity(value_type rho,
                        const_transformed_basis_reference space) :
      DecomposableDensity(std::move(rho), space.from_space(),
                          space.transform()) {}

    /** @brief Creates a density with left factor @p c.
     *
     *  @param[in] rho The representation of the density in the basis_type
     *                 basis.
     *  @param[in] aos The basis set @p rho is expressed in.
     *  @param[in] c   The left factor when @p rho is decomposed.
     *
     *  @throw None No throw guarantee.
     */
    DecomposableDensity(value_type rho, basis_type aos, value_type c) :
      base_type(std::move(rho), std::move(aos)), m_c_(std::move(c)) {}

    /** @brief Returns the left factor.
     *
     *  See the class description for the definition of the left factor.
     *
     *  @return A read-only reference to the left factor of the decomposition.
     *
     *  @throw None No throw guarantee.
     */
    const_reference left_factor() const { return m_c_; }

private:
    /// The orbital space used to make the density
    value_type m_c_;
};

/** @brief Compares two DecomposableDensity instances for equality.
 *
 *  @relates DecomposableDensity
 *
 *  Two Density instances are equal if they describe the same particles and
 they
 *  have the same numeric value.
 *
 *  @tparam LHSParticles The particle types for @p lhs.
 *  @tparam RHSParticles The particle types for @p rhs.
 *
 *  @param[in] lhs The density on the left side of the equality operator.
 *  @param[in] rhs The density on the right side of the equality operator.
 *
 *  @return True if @p lhs has the same type and numeric value as @p rhs.
 False
 *          otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename... LHSParticles, typename... RHSParticles>
bool operator==(const DecomposableDensity<LHSParticles...>& lhs,
                const DecomposableDensity<RHSParticles...>& rhs) {
    if constexpr(std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return std::tie(lhs.left_factor(), lhs.value()) ==
               std::tie(rhs.left_factor(), rhs.value());
    } else {
        return false;
    }
}

/** @brief Determines if two DecomposableDensity instances are different.
 *
 *  @relates DecomposableDensity
 *
 *  Two Densities are different if the equality comparison returns false.
 This
 *  indicates that the densities either have different types and/or different
 *  numeric values.
 *
 *  @tparam LHSParticles The particle types for @p lhs.
 *  @tparam RHSParticles The particle types for @p rhs.
 *
 *  @param[in] lhs The density on the left side of the not-equal operator.
 *  @param[in] rhs The density on the right side of the not-equal operator.
 *
 *  @return False if @p lhs and @p rhs are equal and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename... LHSParticles, typename... RHSParticles>
bool operator!=(const DecomposableDensity<LHSParticles...>& lhs,
                const DecomposableDensity<RHSParticles...>& rhs) {
    return !(lhs == rhs);
}

/// Type of the one-Electron density
using Decomposable1EDensity = DecomposableDensity<Electron>;

} // namespace chemist
