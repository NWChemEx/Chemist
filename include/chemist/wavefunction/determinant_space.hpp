#pragma once
#include "chemist/operators/fock.hpp"
#include "chemist/orbital_space/orbital_space.hpp"

namespace chemist::wavefunction {

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
    /// True if OccupiedSpaceType can be implicitly cast to @p T
    template<typename T>
    static constexpr bool occ_implicit_convert_v =
      std::is_convertible_v<OccupiedSpaceType, T>;

    /// True if VirtualSpaceType can be implicitly cast to @p T
    template<typename T>
    static constexpr bool vir_implicit_convert_v =
      std::is_convertible_v<VirtualSpaceType, T>;

    /// True if OccupiedSpaceType can be implicitly cast to @p T and
    /// VirtualSpaceType can be implicitly cast to @p U
    template<typename T, typename U>
    static constexpr bool is_implicit_convertible_v =
      occ_implicit_convert_v<T>&& vir_implicit_convert_v<U>;

    /// Enables a function if is_implicit_convertible_v is true
    template<typename T, typename U>
    using enable_if_convertible_t =
      std::enable_if_t<is_implicit_convertible_v<T, U>>;

public:
    /// Type of the occupied molecular orbitals
    using occupied_orbital_type = OccupiedSpaceType;

    /// Type of a read-only reference to the occupied orbitals
    using const_occupied_reference = const occupied_orbital_type&;

    /// Type of a read-only pointer to the occupied orbitals
    using const_occupied_pointer = std::shared_ptr<const occupied_orbital_type>;

    /// Type of the virtual molecular orbitals
    using virtual_orbital_type = VirtualSpaceType;

    /// Type of a read-only reference to the virtual orbitals
    using const_virtual_reference = const virtual_orbital_type&;

    /// Type of a rad-only pointer to the virtual orbitals
    using const_virtual_pointer = std::shared_ptr<const virtual_orbital_type>;

    /// Type of the Fock operator which generated these orbitals
    using fock_operator_type = operators::Fock;

    /// Type of a read-only reference to the Fock operator
    using const_fock_reference = const fock_operator_type&;

    /// Type of a read-only pointer to the Fock operator
    using const_fock_pointer = std::shared_ptr<const fock_operator_type>;

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
     *  @param[in] fock The Fock operator associated with the density of the
     *                  occupied orbitals.
     *
     *  @throw None No throw gurantee
     */
    DeterminantSpace(occupied_orbital_type occ, virtual_orbital_type virt,
                     fock_operator_type fock);

    /** @brief Creates a determinant space by aliasing the provided state.
     *
     *
     *  @param[in] pocc  The occupied orbitals to alias.
     *  @param[in] pvirt The virtual orbitals to alias.
     *  @param[in] pfock The Fock operator to alias
     *
     *  @throw None No throw guarantee.
     */
    DeterminantSpace(const_occupied_pointer pocc, const_virtual_pointer pvirt,
                     const_fock_pointer pfock);

    /** @brief The orbitals occupied in the reference determinant.
     *
     *  @return The orbitals which are occupied in the reference determinant.
     *
     *  @throw std::runtime_error if the occupied orbitals have not been set.
     */
    const_occupied_reference occupied_orbitals() const;

    /** @brief The orbitals which are unoccupied in the reference determinant
     *
     *  @return The orbitals which are unoccupied in the reference determinant
     *
     *  @throw std::runtime_error if the virtual orbitals have not been set.
     */
    const_virtual_reference virtual_orbitals() const;

    /** @brief The Fock operator which generated these orbitals.
     *
     *  @return The Fock operator which generated the orbitals.
     *
     *  @throw std::runtime_error if the fock operator has not been set
     */
    const_fock_reference fock_operator() const;

    /** @brief Enables implicit upcasting of the orbital spaces in a determinant
     *         space.
     *
     *  The orbital spaces in a DeterminantSpace are polymorphic. It sometimes
     *  occurs that we have an instance of type  `DeterminantSpace<X, Y>` and
     *  we want to pass it as if it was of type `DeterminantSpace<T, U>` (where
     *  `X` derives from `T` and `Y` derives from `U`). This function makes it
     *  so such conversions happen automagically.
     *
     *  @tparam T The type we want to cast `OccupiedSpaceType` to.
     *  @tparam U The type we want to cast `VirtualSpaceType` to.
     *  @tparam <anonymous> Ensurse via SFINAE that this conversion only
     *                      participates in overload resolution when
     *                      `OccupiedSpaceType` is implicitly convertible to
     *                      @p T, and `VirtualSpaceType` is implicitly
     *                      convertible to @p U.
     *
     *
     *  @return The current instance casted to a `DeterminantSpace<T, U>`
     *          instance.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T, typename U, typename = enable_if_convertible_t<T, U>>
    operator DeterminantSpace<T, U>() const {
        return DeterminantSpace<T, U>(m_pocc_, m_pvirt_, m_pfock_);
    }

    /** @brief Determines if this instance is equal to another
     *
     *  Two DeterminantSpace instances are equal if their occupied, virtual, and
     *  Fock operators compare equal.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance is equal to @p rhs and false otherwise.
     */
    bool operator==(const DeterminantSpace& rhs) const;

    /** @brief Updates a hasher with the state of this determinant space.
     *
     *  @param[in,out] h The hasher to hash the DeterminantSpace with. After
     *                   this call the internal state of @p h will have been
     *                   updated with hashes of this instance's state.
     *
     */
    void hash(chemist::detail_::Hasher& h) const;

private:
    /// The occupied orbitals
    const_occupied_pointer m_pocc_;

    /// The virtual orbitals
    const_virtual_pointer m_pvirt_;

    /// The fock operator
    const_fock_pointer m_pfock_;
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
                const DeterminantSpace<RHSOccSpace, RHSVirtSpace>& rhs) {
    if constexpr(!std::is_convertible_v<decltype(rhs), decltype(lhs)>) {
        return false;
    } else {
        return lhs.operator==(rhs);
    }
}

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

/// Type of a determinant which uses sparse maps
using SparseDeterminant = DeterminantSpace<orbital_space::CanonicalIndSpace>;

/// Type of a determinant with ToT for the virtuals
using SparseToTDeterminant = DeterminantSpace<orbital_space::CanonicalIndSpace,
                                              orbital_space::CanonicalToTSpace>;

extern template class DeterminantSpace<orbital_space::DerivedSpaceD>;
extern template class DeterminantSpace<orbital_space::CanonicalSpaceD>;
extern template class DeterminantSpace<orbital_space::CanonicalIndSpace>;
extern template class DeterminantSpace<orbital_space::CanonicalIndSpace,
                                       orbital_space::CanonicalToTSpace>;

} // namespace chemist::wavefunction
