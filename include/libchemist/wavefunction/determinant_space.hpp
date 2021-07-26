#pragma once
#include "libchemist/operator/fock_operator.hpp"
#include "libchemist/orbital_space/orbital_space.hpp"

namespace libchemist::wavefunction {

/** @brief Describes the space of determinant wavefunctions which can be made
 *         from molecular orbitals.
 *
 *  This class describes the space of orbital determinants that can be formed
 *  from a set of orbitals. This class assumes that the alpha and beta orbitals
 *  are restricted to having the same form and that determinants are formed by
 *  exciting from an occupied space to a virtual space. Additional partitionings
 *  of the orbital space and/or relaxation of the spin-restriction are handled
 *  by other classees.
 *
 *  @tparam OccupiedSpaceType The type used to model the occupied space. Assumed
 *                            to be the type of an instantiation of
 *                            orbital_space::DerivedSpace, or a class derived
 *                            from one.
 *  @tparam VirtualSpaceType The type used to model the virtual space. Same type
 *                           assumptions as @p OccupiedSpaceType. Defaults to
 *                           @p OccupiedSpaceType.
 */
template<typename OccupiedSpaceType,
         typename VirtualSpaceType = OccupiedSpaceType>
class DeterminantSpace {
public:
    /// Type of the occupied molecular orbitals
    using occupied_orbital_type = OccupiedSpaceType;

    /// Type of the virtual molecular orbitals
    using virtual_orbital_type = VirtualSpaceType;

    /// Type of the Fock operator which generated these orbitals
    using fock_operator_type = FockOperator;

    /** @brief Creates a determinant space with all default-initialized membes.
     *
     *  The instance resulting from this ctor has all default initialized
     *  members and is thus suitable for determninants involving zero electrons.
     *  To make the instance "useful" one has to copy/move assign to it.
     *
     *  @throw None No throw guarantee.
     */
    DeterminantSpace() = default;

    /** @brief Creates a determinant space with the provided state.
     *
     *  @param[in] occ The orbitals that are occupied in the reference
     *                 determinant.
     *  @param[in] virt The orbitals that can be used to form the rest of the
     *                  determinant space.
     *  @param[in] fock The Fock operator which generated these orbitals.
     *
     *  @throw None No throw gurantee
     */
    DeterminantSpace(occupied_orbital_type occ, virtual_orbital_type virt,
                     fock_operator_type fock);

    /** @brief The orbitals occupied in the reference determinant.
     *
     *  @return The orbitals which are occupied in the reference determinant.
     *
     *  @throw None No throw guarantee.
     */
    const auto& occupied_orbitals() const noexcept { return m_occ_; }

    /** @brief The orbitals which are unoccupied in the reference determinant
     *
     *  @return The orbitals which are unoccupied in the reference determinant
     *
     *  @throw None No throw guarantee.
     */
    const auto& virtual_orbitals() const noexcept { return m_virt_; }

    /** @brief The Fock operator which generated these orbitals.
     *
     *  @return The Fock operator which generated the orbitals.
     *
     *  @throw None No throw guarantee.
     */
    const auto& fock_operator() const noexcept { return m_fock_; }

    /** @brief Updates a hasher with the state of this determinant space.
     *
     *  @param[in,out] h The hasher to hash the DeterminantSpace with. After
     *                   this call the internal state of @p h will have been
     *                   updated with hashes of this instance's state.
     *
     */
    void hash(pluginplay::Hasher& h) const;

private:
    /// The occupied orbitals
    occupied_orbital_type m_occ_;

    /// The virtual orbitals
    virtual_orbital_type m_virt_;

    /// The fock operator
    fock_operator_type m_fock_;
};

/** @brief Compares two DeterminantSpace instances for equality.
 *  @relates DeterminantSpace
 *
 *  Two DeterminantSpace instances are equal if their occupied, virtual, and
 *  Fock operators compare equal.
 *
 *  @tparam LHSOccSpace the type of the occupied space in @p lhs.
 *  @tparam LHSVirtSpace the type of the virtual space in @p lhs.
 *  @tparam RHSOccSpace the type of the occupied space in @p rhs.
 *  @tparam RHSVirtSpace the type of the virtual space in @p rhs.
 *
 *  @param[in] lhs The DeterminantSpace on the left of the equality.
 *  @param[in] rhs The DeterminantSpace on the right of the equality.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 */
template<typename LHSOccSpace, typename LHSVirtSpace, typename RHSOccSpace,
         typename RHSVirtSpace>
bool operator==(const DeterminantSpace<LHSOccSpace, LHSVirtSpace>& lhs,
                const DeterminantSpace<RHSOccSpace, RHSVirtSpace>& rhs);

/** @brief Determiness if two DeterminantSpace instances are different.
 *  @relates DeterminantSpace
 *
 *  Two DeterminantSpace instances are equal if their occupied, virtual, and
 *  Fock operators compare equal. They are different if they are not equal.
 *
 *  @tparam LHSOccSpace the type of the occupied space in @p lhs.
 *  @tparam LHSVirtSpace the type of the virtual space in @p lhs.
 *  @tparam RHSOccSpace the type of the occupied space in @p rhs.
 *  @tparam RHSVirtSpace the type of the virtual space in @p rhs.
 *
 *  @param[in] lhs The DeterminantSpace on the left of the not equal operator.
 *  @param[in] rhs The DeterminantSpace on the right of the non equal operator.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 */
template<typename LHSOccSpace, typename LHSVirtSpace, typename RHSOccSpace,
         typename RHSVirtSpace>
bool operator!=(const DeterminantSpace<LHSOccSpace, LHSVirtSpace>& lhs,
                const DeterminantSpace<RHSOccSpace, RHSVirtSpace>& rhs) {
    return !(lhs == rhs);
}

/// Type of a determinant space which uses derived orbitals
using Determinant = DeterminantSpace<orbital_space::DerivedSpaceD>;

/// Type of a determinant space which uses canonical MOs
using CanonicalDeterminant = DeterminantSpace<orbital_space::CanonicalSpaceD>;

/// Type of a determinant space which uses local orbitals
using LocalDeterminant = DeterminantSpace<orbital_space::IndDerivedSpace,
                                          orbital_space::DepDerivedSpace>;

/// Type of a determinant space which uses quasicanonical local orbitals
using CanonicalLocalDeterminant =
  DeterminantSpace<orbital_space::CanonicalIndSpace,
                   orbital_space::CanonicalDepSpace>;

} // namespace libchemist::wavefunction

#include "determinant_space.ipp"
