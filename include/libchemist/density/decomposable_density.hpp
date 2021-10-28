#pragma once
#include "libchemist/density/density_base.hpp"
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/orbital_space/derived_space.hpp"

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
class DecomposableDensity : public DensityBase<Particles...>{
public:
    /// Typedef of the base type
    using base_type = DensityBase<Particles...>;

    /// Typedef for a derived space
    using space_type = orbital_space::DerivedSpaceD;

    /// Typedefs from the base type
    using typename base_type::aos_type;
    using typename base_type::value_type;

    /// Default ctor
    DecomposableDensity() = default;

    /// Ctor with non-default values
    DecomposableDensity(value_type rho, space_type space) : base_type(rho),
      m_space_(std::move(space)) {}

    /// Other ctors
    DecomposableDensity(const DecomposableDensity&) = default;
    DecomposableDensity(DecomposableDensity&&)  noexcept = default;
    DecomposableDensity& operator=(const DecomposableDensity&) = default;
    DecomposableDensity& operator=(DecomposableDensity&&)  noexcept = default;

    /// Default dtor
    ~DecomposableDensity() = default;

    /** @brief Provides read-only access to the derived space.
     *
     *  @return The derived space of the density.
     *
     *  @throw None No throw guarantee.
     */
    const space_type& space() const { return m_space_; }

protected:
    /** @brief Returns the space that the density matrix is defined in.
     *
     *  @return The orbital space that this density is defined in terms of in a
     *          read-only manner.
     *
     *  @throw std::runtime_error if the space instance does not contain a from
     *                            space. Strong throw guarantee.
     */
    const aos_type& basis_set_impl() const override {
        return m_space_.from_space();
    }

    /// Override the hash function
    void hash_impl(pluginplay::Hasher& h) const override{
        h(m_space_, this->value());
    }

private:
    /// The AO space of the density
    space_type m_space_;
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
bool operator==(const DecomposableDensity<LHSParticles...>& lhs,
                const DecomposableDensity<RHSParticles...>& rhs) {
    if constexpr(std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return std::tie(lhs.space(), lhs.value()) ==
               std::tie(rhs.space(), rhs.value());
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
bool operator!=(const DecomposableDensity<LHSParticles...>& lhs,
                const DecomposableDensity<RHSParticles...>& rhs) {
    return !(lhs == rhs);
}

/// Type of the one-Electron density
using Decomposable1EDensity = DecomposableDensity<Electron>;

} // namespace libchemist
