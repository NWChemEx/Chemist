#pragma once
#include "chemist/detail_/hashing.hpp"
#include "chemist/orbital_space/derived_space.hpp"
#include "chemist/types.hpp"

namespace chemist::orbital_space {
/** @brief Models an orbital space in which the Fock matrix is diagonal.
 *
 *  For orbital spaces which diagonalize the Fock matrix we can associate an
 *  energy with each orbital (specifically the eigenvalue of the Fock matrix
 *  associated with that orbital). This class models an orbital space that
 *  diagonalizes the Fock matrix in addition to the properties bestowed on
 * the space by `BaseType`.
 *
 *  @tparam OrbitalEnergyType Type of the tensor used to hold the orbital
 *                            energies.
 *  @tparam BaseType Type of the class from which this orbital space
 * inherits.
 */
template<typename OrbitalEnergyType, typename BaseType>
class CanonicalSpace : public BaseType {
public:
    /// Type of the tensor used to store the orbital energies
    using orbital_energy_type = OrbitalEnergyType;

    /// Type of a pointer to the orbital energies
    using orbital_energy_ptr = std::shared_ptr<const OrbitalEnergyType>;

    /// Type of a read-only reference to the orbital energies
    using const_energy_reference = const orbital_energy_type&;

    /** @brief Creates a new CanonicalSpace which does not have any orbital
     *         energies.
     *
     *  This ctor will initialize a completely defaulted CanonicalSpace
     *  instance. This includes the CanonicalSpace part of the object as
     * well as the base class part.
     *
     *  @throw None No throw guarantee.
     */
    CanonicalSpace() = default;

    /** @brief Creates a CanonicalSpace_ with the provided state.
     *
     *  @tparam Args The types of the arguments which will be forwarded to
     * the base class.
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

    template<typename... Args>
    explicit CanonicalSpace(orbital_energy_ptr pegys, Args&&... args);

    /** @brief Returns the energies of the orbitals in this orbital space.
     *
     *  @return The tensor containing the orbital energies in a read-only
     *          state.
     *
     *  @throw std::runtime_error if the instance does not contain any orbital
     *                            energies. Strong throw guarantee.
     */
    const_energy_reference orbital_energies() const;

    /** @brief Returns the pointer holding the orbital energies
     *
     *  @throw None No throw guarantee.
     */
    orbital_energy_ptr orbital_energies_data() const { return m_pegys_; }

    /** @brief Determines if this space is the same as @p rhs.
     *
     *  @param[in] rhs The space we are comparing to.
     *
     *  @return True if this space is equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const CanonicalSpace& rhs) const noexcept;

protected:
    /// Adds the orbital energies to the hash internal to `h`
    virtual void hash_(chemist::detail_::Hasher& h) const override;

    /// Also compares the orbital energies
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The energies associated with each orbital
    orbital_energy_ptr m_pegys_;
};

/** @brief Compares two CanonicalSpace instances for equality.
 *  @relates CanonicalSpace
 *
 *  Two CanonicalSpace instances are equal if they have: the same type, the
 *  same orbital energies, and the base class part of the objects compare
 * equal.
 *
 *  @tparam LOrbitalEnergyType The type of the tensor storing the orbital
 *                             energies in @p lhs.
 *  @tparam LBaseType The type that @p lhs 's CanonicalSpace inherits from.
 *  @tparam ROrbitalEnergyType The type of the tensor storing the orbital
 *                             energies in @p rhs.
 *  @tparam RBaseType The type that @p rhs's CanonicalSpace inherits from.
 *
 *  @param[in] lhs The instance on the left side of the equality operator.
 *  @param[in] rhs The instance on the right side of the equality operator.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 *
 *  @throw ??? Throws if comparing the orbital energies throws or if the
 * base comparison throws. Same throw guarantee.
 */
template<typename LOrbitalEnergyType, typename LBaseType,
         typename ROrbitalEnergyType, typename RBaseType>
bool operator==(const CanonicalSpace<LOrbitalEnergyType, LBaseType>& lhs,
                const CanonicalSpace<ROrbitalEnergyType, RBaseType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>)
        return false;
    else {
        lhs.operator==(rhs);
    }
}

/** @brief Determines if two CanonicalSpace instances are different.
 *  @relates CanonicalSpace
 *
 *  Two CanonicalSpace instances are equal if they have: the same type, the
 *  same orbital energies, and the base class part of the objects compare
 * equal.
 *
 *  @tparam LOrbitalEnergyType The type of the tensor storing the orbital
 *                             energies in @p lhs.
 *  @tparam LBaseType The type that @p lhs 's CanonicalSpace inherits from.
 *  @tparam ROrbitalEnergyType The type of the tensor storing the orbital
 *                             energies in @p rhs.
 *  @tparam RBaseType The type that @p rhs's CanonicalSpace inherits from.
 *
 *  @param[in] lhs The instance on the left side of the not equal operator.
 *  @param[in] rhs The instance on the right side of the not equal operator.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 *
 *  @throw ??? Throws if comparing the orbital energies throws or if the
 * base comparison throws. Same throw guarantee.
 */
template<typename LOrbitalEnergyType, typename LBaseType,
         typename ROrbitalEnergyType, typename RBaseType>
bool operator!=(const CanonicalSpace<LOrbitalEnergyType, LBaseType>& lhs,
                const CanonicalSpace<ROrbitalEnergyType, RBaseType>& rhs) {
    return !(lhs == rhs);
}

/// CanonicalSpace which inherits from DerivedSpace
using CanonicalSpaceD = CanonicalSpace<type::tensor, DerivedSpaceD>;

/// CanonicalSpace which inherits from IndDerivedSpace
using CanonicalToTSpace =
  CanonicalSpace<type::tensor_of_tensors, ToTDerivedSpace>;

extern template class CanonicalSpace<type::tensor, DerivedSpaceD>;
extern template class CanonicalSpace<type::tensor_of_tensors, ToTDerivedSpace>;

// ----------------------------- Implementations
// -------------------------------

template<typename OrbitalEnergyType, typename BaseType>
template<typename... Args>
CanonicalSpace<OrbitalEnergyType, BaseType>::CanonicalSpace(
  OrbitalEnergyType egys, Args&&... args) :
  CanonicalSpace(std::make_shared<OrbitalEnergyType>(std::move(egys)),
                 std::forward<Args>(args)...) {}

template<typename OrbitalEnergyType, typename BaseType>
template<typename... Args>
CanonicalSpace<OrbitalEnergyType, BaseType>::CanonicalSpace(
  orbital_energy_ptr pegys, Args&&... args) :
  m_pegys_(pegys), BaseType(std::forward<Args>(args)...) {}

} // namespace chemist::orbital_space
