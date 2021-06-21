#pragma once
#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/orbital_space/types.hpp"
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
class CanonicalSpace : public BaseType {
public:
    /// Type of the tensor used to store the orbital energies
    using orbital_energy_type = OrbitalEnergyType;

    /** @brief Creates a new CanonicalSpace_ which does not have any orbital
     *         energies.
     *
     */
    CanonicalSpace() = default;

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
    explicit CanonicalSpace(OrbitalEnergyType egys, Args&&... args);

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

    /// Also compares the orbital energies
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The energies associated with each orbital
    orbital_energy_type m_egys_;
};

template<typename LOrbitalEnergyType, typename LBaseType,
         typename ROrbitalEnergyType, typename RBaseType>
bool operator==(const CanonicalSpace<LOrbitalEnergyType, LBaseType>& lhs,
                const CanonicalSpace<ROrbitalEnergyType, RBaseType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>)
        return false;
    else {
        if(lhs.orbital_energies() != rhs.orbital_energies()) return false;
        const LBaseType& lbase = lhs;
        const RBaseType& rbase = rhs;
        return lhs == rhs;
    }
}

template<typename LOrbitalEnergyType, typename LBaseType,
         typename ROrbitalEnergyType, typename RBaseType>
bool operator!=(const CanonicalSpace<LOrbitalEnergyType, LBaseType>& lhs,
                const CanonicalSpace<ROrbitalEnergyType, RBaseType>& rhs) {
    return !(lhs == rhs);
}

using CanonicalSpaceD = CanonicalSpace<type::tensor<double>, DerivedSpaceD>;
using CanonicalIndSpaceD =
  CanonicalSpace<type::tensor<double>, IndDerivedSpaceD>;

using CanonicalDepSpaceD =
  CanonicalSpace<type::tensor_of_tensors<double>, DepDerivedSpaceD>;

extern template class CanonicalSpace<type::tensor<double>, DerivedSpaceD>;
extern template class CanonicalSpace<type::tensor<double>, IndDerivedSpaceD>;
extern template class CanonicalSpace<type::tensor_of_tensors<double>,
                                     DepDerivedSpaceD>;

//------------------------------- Implementations ------------------------------

} // namespace libchemist::orbital_space
