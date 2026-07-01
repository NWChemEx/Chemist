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
#include <chemist/electron/electron.hpp>
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <tensorwrapper/tensorwrapper.hpp>

namespace chemist {

/** @brief Class describing a density.
 *
 *  In Electronic structure theory we place a lot of emphasis on the
 *  one-Electron density; however, there are many other densities. This class
 *  is templated on the types and number of particles that the density
 *  describes. In turn this allows for this class to model densities involving
 *  an arbitrary number of arbitrary particles.
 *
 *  @note At the moment this class is basically a strong type which wraps a
 *        tensor and an AOs object.
 *
 *  @note If we want to support Particles... != Electron... we will need to add
 *        TMP to work out the basis type.
 *
 *  @tparam Particles The types of the particles described by this density. At
 *                    the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 */
template<typename... Particles>
class Density {
public:
    /// Type used to represent the numeric form of the density
    using value_type = tensorwrapper::Tensor;

    /// Type of a mutable reference to the density matrix
    using reference = value_type&;

    /// Type of a read-only reference to the density matrix
    using const_reference = const value_type&;

    /// Type of the basis set for value_type
    using basis_type = wavefunction::AOs;

    /// Type of a mutable reference to an object of type basis_type
    using basis_reference = basis_type&;

    /// Type of a read-only reference to an object of basis_type
    using const_basis_reference = const basis_type&;

    /// Default ctor
    explicit Density() = default;

    /** @brief Creates a new density object with the provided values.
     *
     *  @param[in] rho The tensor representation of the density matrix.
     *  @param[in] orbs The basis set the density matrix is expressed in.
     *
     *  @throw None No throw guarantee.
     */
    Density(value_type rho, basis_type orbs) :
      m_density_(std::move(rho)), m_orbs_(std::move(orbs)) {}

    /** @brief Standard defaulted dtor
     *
     *  This dtor will free up the memory associated with the PIMPL. As a
     *  result, all references to the Point's coordinates will be invalidated.
     *
     *  @throw none No throw guarantee.
     */
    virtual ~Density() noexcept = default;

    /** @brief Returns a mutable reference to the density matrix.
     *
     *  @return A mutable reference to the tensor representation of the density.
     *
     *  @throw None No throw guarantee.
     */
    reference value() noexcept { return m_density_; }

    /** @brief Provides read-only access to the density matrix.
     *
     *  Same as the non-const version except that the return is read-only. See
     *  description of non-const version for more details.
     *
     *  @return The density value.
     *
     *  @throw None No throw guarantee.
     */
    const_reference value() const noexcept { return m_density_; }

    /** @brief Returns a mutable reference to the orbital basis set for the
     *         density.
     *
     *  @return A mutable reference to the basis set for *this.
     *
     *  @throw None No throw guarantee.
     */
    basis_reference basis_set() noexcept { return m_orbs_; }

    /** @brief Provides read-only access to the orbital basis set used to define
     *         the density matrix.
     *
     *  Same as the non-const version except that the return is read-only. See
     *  description of the non-const version for more details.
     *
     *  @return The orbital basis the density is formed in.
     *
     *  @throw ??? Throws if the derived class's implementation of `basis_set_`
     *             throws. Same throw guarantee.
     */
    const_basis_reference basis_set() const { return basis_set_(); }

protected:
    /// The density tensor
    value_type m_density_;

    /// For assigning density value in derived types
    explicit Density(value_type rho) : m_density_(rho) {}

    /** @brief To be overridden by the derived class to implement basis_set().
     *
     *  The derived class is responsible for ensuring this function returns the
     *  basis set.
     *
     *  @return The orbital basis the density is formed in.
     *
     *  @throw None No throw guarantee.
     */
    virtual const_basis_reference basis_set_() const { return m_orbs_; }

private:
    /// The orbital space used to make the density
    basis_type m_orbs_;
};

/** @brief Compares two Density instances for equality.
 *
 *  @relates Density
 *
 *  Two Density instances are equal if they describe the same particles and they
 *  have the same numeric value.
 *
 *  @tparam LHSParticles The particle types for @p lhs.
 *  @tparam RHSParticles The particle types for @p rhs.
 *
 *  @param[in] lhs The density on the left side of the equality operator.
 *  @param[in] rhs The density on the right side of the equality operator.
 *
 *  @return True if @p lhs has the same type and numeric value as @p rhs. False
 *          otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename... LHSParticles, typename... RHSParticles>
bool operator==(const Density<LHSParticles...>& lhs,
                const Density<RHSParticles...>& rhs) {
    if constexpr(std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return std::tie(lhs.basis_set(), lhs.value()) ==
               std::tie(rhs.basis_set(), rhs.value());
    } else {
        return false;
    }
}

/** @brief Determines if two Density instances are different.
 *
 *  @relates Density
 *
 *  Two Densities are different if the equality comparison returns false. This
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
bool operator!=(const Density<LHSParticles...>& lhs,
                const Density<RHSParticles...>& rhs) {
    return !(lhs == rhs);
}

extern template class Density<Electron>;

} // namespace chemist
