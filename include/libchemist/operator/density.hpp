#pragma once
#include "libchemist/operator/electron.hpp"
#include "libchemist/types.hpp"

namespace libchemist {

/** @brief Class describing a density.
 *
 *  @todo Should the density be its own libchemist component?
 */
template<typename... Particles>
class Density {
public:
    /// Type used to represent the numeric form of the density
    using density_type = type::tensor;

private:
    /// The density tensor
    type::tensor m_density_;
};

/// Type of the one-electron density
using OneElectronDensity = Density<Electron>;

} // namespace libchemist
