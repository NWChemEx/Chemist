#pragma once
#include "libchemist/density/density_base.hpp"
#include "libchemist/electrons/electrons.hpp"

namespace libchemist {

/** @brief Class describing a density with its space.
 *
 *  In Electronic structure theory we place a lot of emphasis on the
 *  one-Electron density; however, there are many other densities. This class
 *  is templated on the types and number of particles that the density
 *  describes. In turn this allows for this class to model densities involving
 *  an arbitrary number of arbitrary particles.
 *
 *  @tparam Particles The types of the particles described by this density. At
 *                    the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 */
template<typename... Particles>
class Density : public DensityBase<Particles...> {
public:
    /// Typedef of the base type
    using base_type = DensityBase<Particles...>;

    /// Typedefs from the base type
    using typename base_type::aos_type;
    using typename base_type::value_type;

    /// Default ctor
    Density() = default;

    /// Ctor with non-default values
    Density(value_type rho, aos_type aos) :
      base_type(rho), m_orbs_(std::move(aos)) {}

    /// Other ctors
    Density(const Density&)     = default;
    Density(Density&&) noexcept = default;
    Density& operator=(const Density&) = default;
    Density& operator=(Density&&) noexcept = default;

    /// Default dtor
    ~Density() = default;

protected:
    /// Override virtual and return the basis space
    const aos_type& basis_set_impl() const override { return m_orbs_; }

    /// Override the hash function
    void hash_impl(pluginplay::Hasher& h) const override {
        h(m_orbs_, this->value());
    }

private:
    /// The AO space of the density
    aos_type m_orbs_;
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

/// Type of the one-Electron density
using OneElectronDensity = Density<Electron>;

} // namespace libchemist
