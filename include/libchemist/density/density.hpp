#pragma once
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/types.hpp"

namespace libchemist {

/** @brief Class describing a density.
 *
 *  In Electronic structure theory we place a lot of emphasis on the
 *  one-Electron density; however, there are many other densities. This class
 *  is templated on the types and number of particles that the density
 *  describes. In turn this allows for this class to model densities involving
 *  an arbitrary number of arbitrary particles.
 *
 *  @note At the moment this class is basically a strong type which wraps a
 *        tensor.
 *
 *  @tparam SpaceType The type of the space from which the density derives.
 *  @tparam Particles The types of the particles described by this density. At
 *                    the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 */
template<typename SpaceType, typename... Particles>
class Density {
public:
    /// Type used to represent the numeric form of the density
    using value_type = type::tensor;

    /// Type of the basis set
    using space_type = SpaceType;

    explicit Density() = default;

    Density(value_type rho, space_type aos) :
      m_orbs_(std::move(aos)), m_density_(std::move(rho)) {}

    /** @brief Provides read-only access to the orbital basis set used to define
     *         the density matrix.
     *
     *  @return The orbital basis the density is formed in.
     *
     *  @throw None No throw guarantee.
     */
    const auto& basis_set() const { return m_orbs_; }

    const auto& value() const noexcept { return m_density_; }

    void hash(pluginplay::Hasher& h) const { h(m_orbs_, m_density_); }

private:
    /// The orbital space used to make the density
    space_type m_orbs_;

    /// The density tensor
    value_type m_density_;
};

/** @brief Compares two Density instances for equality.
 *
 *  @relates Density
 *
 *  Two Density instances are equal if they describe the same particles and they
 *  have the same numeric value.
 *
 *  @tparam LHSSpaceType The space type for @p lhs.
 *  @tparam LHSParticles The particle types for @p lhs.
 *  @tparam RHSSpaceType The space type for @p rhs.
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
template<typename LHSSpaceType, typename... LHSParticles, typename RHSSpaceType,
         typename... RHSParticles>
bool operator==(const Density<LHSSpaceType, LHSParticles...>& lhs,
                const Density<RHSSpaceType, RHSParticles...>& rhs) {
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
 *  @tparam LHSSpaceType The space type for @p lhs.
 *  @tparam LHSParticles The particle types for @p lhs.
 *  @tparam RHSSpaceType The space type for @p rhs.
 *  @tparam RHSParticles The particle types for @p rhs.
 *
 *  @param[in] lhs The density on the left side of the not-equal operator.
 *  @param[in] rhs The density on the right side of the not-equal operator.
 *
 *  @return False if @p lhs and @p rhs are equal and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSSpaceType, typename... LHSParticles, typename RHSSpaceType,
         typename... RHSParticles>
bool operator!=(const Density<LHSSpaceType, LHSParticles...>& lhs,
                const Density<RHSSpaceType, RHSParticles...>& rhs) {
    return !(lhs == rhs);
}

/// Type of the one-Electron density
using OneElectronDensity    = Density<orbital_space::AOSpaceD, Electron>;
using Decomposable1EDensity = Density<orbital_space::DerivedSpaceD, Electron>;

} // namespace libchemist
