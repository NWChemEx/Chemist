#pragma once
#include <chemist/operators/fock.hpp>
#include <chemist/orbital_space/orbital_space.hpp>

namespace chemist::wavefunction {

template<typename InactiveSpaceType,
         typename ActiveSpaceType  = InactiveSpaceType,
         typename VirtualSpaceType = InactiveSpaceType,
         typename CoreSpaceType    = InactiveSpaceType>
class ActiveSpace {
    /// True if InactiveSpaceType can be implicitly cast to @p T
    template<typename T>
    static constexpr bool inact_implicit_convert_v =
      std::is_convertible_v<InactiveSpaceType, T>;

    /// True if ActiveSpaceType can be implicitly cast to @p T
    template<typename T>
    static constexpr bool act_implicit_convert_v =
      std::is_convertible_v<ActiveSpaceType, T>;

    /// True if VirtualSpaceType can be implicitly cast to @p T
    template<typename T>
    static constexpr bool vir_implicit_convert_v =
      std::is_convertible_v<VirtualSpaceType, T>;

    /// True if CoreSpaceType can be implicitly cast to @p T
    template<typename T>
    static constexpr bool core_implicit_convert_v =
      std::is_convertible_v<CoreSpaceType, T>;

    /// True if InactiveSpaceType can be implicitly cast to @p T,
    ///         ActiveSpaceType can be implicitly cast to @p U,
    ///         VirtualSpaceType can be implicitly cast to @p V and
    ///         CoreSpaceType can be implicitly cast to @p W
    template<typename T, typename U, typename V, typename W>
    static constexpr bool is_implicit_convertible_v =
      inact_implicit_convert_v<T> && act_implicit_convert_v<U> &&
      vir_implicit_convert_v<V>   && core_implicit_convert_v<W>;

    /// Enables a function if is_implicit_convertible_v is true
    template<typename T, typename U, typename V, typename W>
    using enable_if_convertible_t =
      std::enable_if_t<is_implicit_convertible_v<T, U, V, W>>;

public:
    /// Type of the inactive molecular orbitals
    using inactive_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the inactive orbitals
    using const_inactive_reference = const inactive_orbital_type&;

    // Type of a read-only pointer to the inactive orbitals
    using const_inactive_pointer = std::shared_ptr<const inactive_orbital_type>;

    /// Type of the active molecular orbitals
    using active_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the active orbitals
    using const_active_reference = const active_orbital_type&;

    // Type of a read-only pointer to the active orbitals
    using const_active_pointer = std::shared_ptr<const active_orbital_type>;

    /// Type of the virtual molecular orbitals
    using virtual_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the virtual orbitals
    using const_virtual_reference = const virtual_orbital_type&;

    // Type of a read-only pointer to the virtual orbitals
    using const_virtual_pointer = std::shared_ptr<const virtual_orbital_type>;

    /// Type of the core molecular orbitals
    using core_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the core orbitals
    using const_core_reference = const core_orbital_type&;

    // Type of a read-only pointer to the core orbitals
    using const_core_pointer = std::shared_ptr<const core_orbital_type>;

    /// Type of the Fock operator which generated these orbitals
    using fock_operator_type = operators::Fock;

    /// Type of a read-only reference to the Fock operator
    using const_fock_reference = const fock_operator_type&;

    /// Type of a read-only pointer to the Fock operator
    using const_fock_pointer = std::shared_ptr<const fock_operator_type>;

    /** @brief Creates an active space with all default-initialized members.
     *
     *  The instance resulting from this ctor has all default initialized
     *  members and is thus suitable for wavefunctions involving zero electrons.
     *  To make the instance "useful" one has to copy/move assign to it.
     *
     *  @throw None No throw guarantee.
     */
    ActiveSpace() = default;

    /** @brief Creates an active space with the provided state.
     *
     *  @param[in] inact The orbitals that are inactive in the multiconfigurational 
     *                   wavefunction.
     *  @param[in]   act The orbitals that are active in the multiconfigurational 
     *                   wavefunction.
     *  @param[in]  virt The orbitals that are virtual in the multiconfigurational 
     *                   wavefunction.
     *  @param[in]  core The orbitals that are core (inactive) in the 
     *                   multiconfigurational  wavefunction.
     *  @param[in] inact_fock The Fock operator associated with the density of the
     *                  inactive orbitals.
     *  @param[in]   act_fock The Fock operator associated with the density of the
     *                  active orbitals.
     *
     *  @throw None No throw gurantee
     */
    ActiveSpace(inactive_orbital_type inact, active_orbital_type act,
                virtual_orbital_type virt, core_orbital_type core,
                fock_operator_type inact_fock, fock_operator_type act_fock);


    /** @brief Creates an active space by aliasing the provided state.
     *
     *
     *  @param[in] pinact  The inactive orbitals to alias.
     *  @param[in] pact    The active orbitals to alias.
     *  @param[in] pvirt   The virtual orbitals to alias.
     *  @param[in] pcore   The core orbitals to alias.
     *  @param[in] pinact_fock The inactive Fock operator to alias
     *  @param[in] pact_fock   The active Fock operator to alias
     *
     *  @throw None No throw guarantee.
     */
    ActiveSpace(const_inactive_pointer pinact, const_active_pointer pact,
                const_virtual_pointer pvirt, const_core_pointer pcore,
                const_fock_pointer pinact_fock, const_fock_pointer pact_fock);


    /** @brief The inactive orbitals of the multiconfigurational wavefunction.
     *
     *  @return The orbitals which are inactive in the multiconfigurational
     *    wavefunction.
     *
     *  @throw std::runtime_error if the inactive orbitals have not been set.
     */
    const_inactive_reference inactive_orbitals() const;

    /** @brief The active orbitals of the multiconfigurational wavefunction.
     *
     *  @return The orbitals which are active in the multiconfigurational
     *    wavefunction.
     *
     *  @throw std::runtime_error if the active orbitals have not been set.
     */
    const_active_reference active_orbitals() const;

    /** @brief The virtual orbitals of the multiconfigurational wavefunction.
     *
     *  @return The orbitals which are virtual in the multiconfigurational
     *    wavefunction.
     *
     *  @throw std::runtime_error if the virtual orbitals have not been set.
     */
    const_virtual_reference virtual_orbitals() const;

    /** @brief The core orbitals of the multiconfigurational wavefunction.
     *
     *  @return The orbitals which are core in the multiconfigurational
     *    wavefunction.
     *
     *  @throw std::runtime_error if the core orbitals have not been set.
     */
    const_core_reference core_orbitals() const;

    /** @brief The inactive Fock operator associated with this space.
     *
     *  @return The inactive Fock operator associated with this space.
     *
     *  @throw std::runtime_error if the inactive fock operator has not been set
     */
    const_fock_reference inactive_fock_operator() const;

    /** @brief The active Fock operator associated with this space.
     *
     *  @return The active Fock operator associated with this space.
     *
     *  @throw std::runtime_error if the active fock operator has not been set
     */
    const_fock_reference active_fock_operator() const;

    /** @brief Enables implicit upcasting of the orbital spaces in an active
     *         space.
     *
     *  The orbital spaces in an ActiveSpace are polymorphic. It sometimes
     *  occurs that we have an instance of type  `ActiveSpace<X, Y, Z, A>` and
     *  we want to pass it as if it was of type `ActiveSpace<T, U, V, W>` (where
     *  `X` derives from `T` and `Y` derives from `U` and so on). 
     *  This function makes it so such conversions happen automagically.
     *
     *  @tparam T The type we want to cast `InactiveSpaceType` to.
     *  @tparam U The type we want to cast `ActiveSpaceType` to.
     *  @tparam V The type we want to cast `VirtualSpaceType` to.
     *  @tparam W The type we want to cast `CoreSpaceType` to.
     *  @tparam <anonymous> Ensurse via SFINAE that this conversion only
     *                      participates in overload resolution when
     *                      types are implicitly convertible to their target space
     *
     *
     *  @return The current instance casted to a `ActiveSpace<T, U, V, W>`
     *          instance.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T, typename U, typename V, typename W,
             typename = enable_if_convertible_t<T, U, V, W>>
    operator ActiveSpace<T, U, V, W>() const {
        return ActiveSpace<T, U, V, W>(m_pinact_, m_pact_, m_pvirt_, m_pcore_,
                                       m_pinact_fock_, m_pact_fock_);
    }

    /** @brief Determines if this instance is equal to another
     *
     *  Two ActiveSpace instances are equal if their inactive, active, virtual, 
     *  core and Fock operators compare equal.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance is equal to @p rhs and false otherwise.
     */
    bool operator==(const ActiveSpace& rhs) const;

    /** @brief Updates a hasher with the state of this active space.
     *
     *  @param[in,out] h The hasher to hash the ActiveSpace with. After
     *                   this call the internal state of @p h will have been
     *                   updated with hashes of this instance's state.
     *
     */
    void hash(chemist::detail_::Hasher& h) const;

private:
    /// The inactive orbitals
    const_inactive_pointer m_pinact_;

    /// The active orbitals
    const_active_pointer m_pact_;

    /// The virtual orbitals
    const_virtual_pointer m_pvirt_;

    /// The core orbitals
    const_core_pointer m_pcore_;

    /// The inactive fock operator
    const_fock_pointer m_pinact_fock_;

    /// The active fock operator
    const_fock_pointer m_pact_fock_;
};

/** @brief Compares two ActiveSpace instances for equality.
 *  @relates ActiveSpace
 *
 *  Two ActiveSpace instances are equal if their inactive, active, virtual, 
 *  core and Fock operators compare equal.
 *
 *  @tparam LHSInactSpace the type of the inactive space in @p lhs.
 *  @tparam LHSActSpace the type of the active space in @p lhs.
 *  @tparam LHSVirtSpace the type of the virtual space in @p lhs.
 *  @tparam LHSCoreSpace the type of the core space in @p lhs.
 *  @tparam RHSInactSpace the type of the inactive space in @p rhs.
 *  @tparam RHSActSpace the type of the active space in @p rhs.
 *  @tparam RHSVirtSpace the type of the virtual space in @p rhs.
 *  @tparam RHSCoreSpace the type of the core space in @p rhs.
 *
 *  @param[in] lhs The ActiveSpace on the left of the equality.
 *  @param[in] rhs The ActiveSpace on the right of the equality.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 */
template<typename LHSInactSpace, typename LHSActSpace, typename LHSVirtSpace, 
         typename LHSCoreSpace, typename RHSInactSpace, typename RHSActSpace,
         typename RHSVirtSpace, typename RHSCoreSpace>
bool operator==(const ActiveSpace<LHSInactSpace, LHSActSpace, LHSVirtSpace, LHSCoreSpace>& lhs,
                const ActiveSpace<RHSInactSpace, RHSActSpace, RHSVirtSpace, RHSCoreSpace>& rhs) {
    if constexpr(!std::is_convertible_v<decltype(rhs), decltype(lhs)>) {
        return false;
    } else {
        return lhs.operator==(rhs);
    }
}

/** @brief Determiness if two ActiveSpace instances are different.
 *  @relates ActiveSpace
 *
 *  Two ActiveSpace instances are equal if their occupied, virtual, and
 *  Fock operators compare equal. They are different if they are not equal.
 *
 *  @tparam LHSOccSpace the type of the occupied space in @p lhs.
 *  @tparam LHSVirtSpace the type of the virtual space in @p lhs.
 *  @tparam RHSOccSpace the type of the occupied space in @p rhs.
 *  @tparam RHSVirtSpace the type of the virtual space in @p rhs.
 *
 *  @param[in] lhs The ActiveSpace on the left of the not equal operator.
 *  @param[in] rhs The ActiveSpace on the right of the non equal operator.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 */
template<typename LHSInactSpace, typename LHSActSpace, typename LHSVirtSpace, 
         typename LHSCoreSpace, typename RHSInactSpace, typename RHSActSpace,
         typename RHSVirtSpace, typename RHSCoreSpace>
bool operator!=(const ActiveSpace<LHSInactSpace, LHSActSpace, LHSVirtSpace, LHSCoreSpace>& lhs,
                const ActiveSpace<RHSInactSpace, RHSActSpace, RHSVirtSpace, RHSCoreSpace>& rhs) {
    return !(lhs == rhs);
}

/// Type of a determinant space which uses derived orbitals
using DerivedActiveSpace = ActiveSpace<orbital_space::DerivedSpaceD>;

extern template class ActiveSpace<orbital_space::DerivedSpaceD>;

} // namespace chemist::wavefunction
