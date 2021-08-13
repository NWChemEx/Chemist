#pragma once
#include "libchemist/density/density.hpp"
#include "libchemist/electrons/electrons.hpp"
#include "libchemist/molecule/molecule.hpp"
#include "libchemist/operators/detail_/operator_impl.hpp"

namespace libchemist::operators {

/** @brief Describes the electrostatic interaction among particles.
 *
 *  The CoulombInteraction describes the electrostatic interaction among charged
 *  particles.
 *
 *  @tparam Particles The type or types of the entities participating in the
 *                    interaction. Can be a single type if the type entails more
 *                    than one particle (e.g. Nuclei)
 *
 */
template<typename... Particles>
class CoulombInteraction
  : public detail_::OperatorImpl<CoulombInteraction, Particles...> {
private:
    /// Type of the base class
    using base_type = detail_::OperatorImpl<CoulombInteraction, Particles...>;

public:
    CoulombInteraction()                          = default;
    CoulombInteraction(const CoulombInteraction&) = default;
    CoulombInteraction(CoulombInteraction&&)      = default;
    explicit CoulombInteraction(Particles... ps) :
      base_type(std::move(ps)...) {}

    CoulombInteraction& operator=(const CoulombInteraction&) = default;
    CoulombInteraction& operator=(CoulombInteraction&&) = default;

protected:
    /// Gives a more traditional representation than the default implementation
    std::string as_string_impl() const override { return "(r\u0302₁₂)⁻¹"; }
};

/// Type of the two-electron repulsion operator
using ElectronRepulsion =
  CoulombInteraction<libchemist::Electron, libchemist::Electron>;

/// Type of the nuclei-nuclei repulsion operator
using NuclearRepulsion = CoulombInteraction<libchemist::Nuclei>;

/// Type of the electron-nuclei attraction operator
using ElectronNuclearAttraction =
  CoulombInteraction<libchemist::Electron, libchemist::Nuclei>;

/// Type of a one-electron Coulomb interaction with a one-electron density
using ElectronEDensityCoulomb =
  CoulombInteraction<libchemist::Electron, libchemist::OneElectronDensity>;

/// Type of the many-electron repulsion operator
using NElectronRepulsion = CoulombInteraction<libchemist::ManyElectrons>;

/// Type of the many-electron, nuclei attraction operator
using NElectronNuclearAttraction =
  CoulombInteraction<libchemist::ManyElectrons, libchemist::Nuclei>;

} // namespace libchemist::operators
