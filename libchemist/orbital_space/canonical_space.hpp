#pragma once
#include <sde/detail_/memoization.hpp>

namespace libchemist::orbital_space {

/** @brief Models an orbital space in which the Fock matrix is diagonal.
 *
 *  For orbital spaces which diagonalize the Fock matrix we can associate an
 *  energy with each orbital (specifically the eigenvalue of the Fock matrix
 *  associated with that orbital). This class models an orbital space that
 *  diagonalizes the Fock matrix in addition to the properties bestowed on the
 *  space by `BaseType`.
 *
 *  @tparam OrbitalEnergyType Type of the tensor used to hold the orbital
 *                            energies.
 *  @tparam BaseType Type of the class from which this orbital space inherits.
 */
template<typename OrbitalEnergyType, typename BaseType>
class CanonicalSpace_ : public BaseType {
public:
    /// Type of the tensor used to store the orbital energies
    using orbital_energy_type = OrbitalEnergyType;

    /** @brief Creates a CanonicalSpace_ with the provided state.
     *
     *  @tparam Args The types of the arguments which will be forwarded to the
     *               base class.
     *
     *  @param[in] egys The energies of the orbitals. Defaults to a default
     *                  initialized orbital_energy_type instance.
     *
     *  @param[in] args The inputs for the base class.
     *
     *  @throw ??? If moving `egys` or forwarding `args` throws. Same throw
     *             guarantee.
     */
    template<typename... Args>
    CanonicalSpace_(OrbitalEnergyType egys = {}, Args&&... args);

    /** @brief Returns the energies of the orbitals in this orbital space.
     *
     *  @return The tensor containing the orbital energies in a read/write
     *          fashion.
     */
    auto& orbital_energies() { return m_egys_; }

    /** @brief Returns the energies of the orbitals in this orbital space.
     *
     *  @return The tensor containing the orbital energies in a read-only state.
     */
    const auto& orbital_energies() const { return m_egys_; }

protected:
    /// Adds the orbital energies to the hash internal to `h`
    virtual void hash_(sde::Hasher& h) const override;

private:
    /// The energies associated with each orbital
    orbital_energy_type m_egys_;
};

template<typename TensorType, typename LHSBase, typename RHSBase>
bool operator==(const CanonicalSpace_<TensorType, LHSBase>& lhs,
                const CanonicalSpace_<TensorType, RHSBase>& rhs) {
    auto& cast_lhs = static_cast<const LHSBase&>(lhs);
    auto& cast_rhs = static_cast<const RHSBase&>(rhs);

    // TODO: Actually compare the tensors
    auto lhash = sde::hash_objects(lhs.orbital_energies());
    auto rhash = sde::hash_objects(rhs.orbital_energies());

    return std::tie(cast_lhs, lhash) == std::tie(cast_rhs, rhash);
}

template<typename TensorType, typename LHSBase, typename RHSBase>
bool operator!=(const CanonicalSpace_<TensorType, LHSBase>& lhs,
                const CanonicalSpace_<TensorType, RHSBase>& rhs) {
    return !(lhs == rhs);
}

//------------------------------- Implementations ------------------------------

template<typename OrbitalEnergyType, typename BaseType>
template<typename... Args>
CanonicalSpace_<OrbitalEnergyType, BaseType>::CanonicalSpace_(
  OrbitalEnergyType egys, Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_egys_(std::move(egys)) {}

template<typename OrbitalEnergyType, typename BaseType>
void CanonicalSpace_<OrbitalEnergyType, BaseType>::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_egys_);
}

} // namespace libchemist::orbital_space
