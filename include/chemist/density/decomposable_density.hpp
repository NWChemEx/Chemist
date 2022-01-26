#pragma once
#include "chemist/density/density.hpp"
#include "chemist/orbital_space/derived_space.hpp"

namespace chemist {

/** @brief Class describing a decomposable density.
 *
 *  This class ties a density to the derived space it was formed from.
 *
 *  @tparam Particles The types of the particles described by this density. At
 *                    the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 */
template<typename... Particles>
class DecomposableDensity : public Density<Particles...> {
public:
    /// Type of base Density
    using base_type = Density<Particles...>;

    /// Type of the derived space
    using space_type = orbital_space::DerivedSpaceD;

    /// Type used to represent the numeric form of the density
    using typename base_type::value_type;

    /// Type of the basis set
    using typename base_type::aos_type;

    /// Default ctor
    explicit DecomposableDensity() = default;

    /// Ctor with non-default values
    DecomposableDensity(value_type rho, space_type space) :
      base_type(std::move(rho)), m_space_(std::move(space)) {}

    /** @brief Provides read-only access to the derived space.
     *
     *  @return The derived space of the density.
     *
     *  @throw None No throw guarantee.
     */
    const space_type& space() const { return m_space_; }

protected:
    /** @brief Implementation of basis_set()
     *
     *  @return The orbital basis the density is formed in.
     *
     *  @throw std::runtime_error if derived space does not contain a from
     *                            space. Strong throw guarantee.
     */
    const aos_type& basis_set_() const { return m_space_.from_space(); }

    /** @brief Implementation of hash().
     *
     *  @throw None No throw guarantee.
     */
    void hash_(detail_::Hasher& h) const { h(m_space_, this->value()); }

private:
    /// The orbital space used to make the density
    space_type m_space_;
};

/** @brief Compares two DecomposableDensity instances for equality.
 *
 *  @relates DecomposableDensity
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
bool operator==(const DecomposableDensity<LHSParticles...>& lhs,
                const DecomposableDensity<RHSParticles...>& rhs) {
    if constexpr(std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return std::tie(lhs.space(), lhs.value()) ==
               std::tie(rhs.space(), rhs.value());
    } else {
        return false;
    }
}

/** @brief Determines if two DecomposableDensity instances are different.
 *
 *  @relates DecomposableDensity
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
bool operator!=(const DecomposableDensity<LHSParticles...>& lhs,
                const DecomposableDensity<RHSParticles...>& rhs) {
    return !(lhs == rhs);
}

/// Type of the one-Electron density
using Decomposable1EDensity = DecomposableDensity<Electron>;

} // namespace chemist