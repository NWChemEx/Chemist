#pragma once
#include "libchemist/operator/electron.hpp"
#include "libchemist/types.hpp"

namespace libchemist {

/** @brief Class describing a density.
 *
 *  In Electronic structure theory we place a lot of emphasis on the
 *  one-Electron density; however, there are many other densities. This class
 * is templated on the types and number of particles that the density describes.
 *  In turn this allows for this class to model densities involving an
 *  arbitrary number of arbitrary particles.
 *
 *  @note At the moment this class is basically a strong type which wraps a
 *        tensor.
 *
 *  @tparam Particles The types of the particles described by this density. At
 *                    the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 *  @todo Should the density be its own libchemist component?
 */
template<typename... Particles>
class Density {
public:
    /// Type used to represent the numeric form of the density
    using value_type = type::tensor;

    explicit Density(value_type rho = {}) : m_density_(std::move(rho)) {}

    const auto& value() const noexcept { return m_density_; }

    void hash(pluginplay::Hasher& h) const { h(m_density_); }

private:
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
        return lhs.value() == rhs.value();
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

/// Type of the one-Electron density
using OneElectronDensity = Density<Electron>;

} // namespace libchemist
