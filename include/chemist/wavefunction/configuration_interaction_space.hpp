#pragma once
#include <chemist/operators/fock.hpp>
#include <chemist/orbital_space/orbital_space.hpp>
#include <chemist/density/density.hpp>

namespace chemist::wavefunction {

template<typename InactiveSpaceType,
         typename ActiveSpaceType  = InactiveSpaceType,
         typename VirtualSpaceType = InactiveSpaceType,
         typename CoreSpaceType    = InactiveSpaceType>
class ConfigurationInteractionSpace {
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
    /// Type of sizes
    using size_type = int;

    /// Type of the inactive molecular orbitals
    using inactive_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the inactive orbitals
    using const_inactive_reference = const inactive_orbital_type&;

    /// Type of a read-only pointer to the inactive orbitals
    using const_inactive_pointer = std::shared_ptr<const inactive_orbital_type>;

    /// Type of the active molecular orbitals
    using active_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the active orbitals
    using const_active_reference = const active_orbital_type&;

    /// Type of a read-only pointer to the active orbitals
    using const_active_pointer = std::shared_ptr<const active_orbital_type>;

    /// Type of the virtual molecular orbitals
    using virtual_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the virtual orbitals
    using const_virtual_reference = const virtual_orbital_type&;

    /// Type of a read-only pointer to the virtual orbitals
    using const_virtual_pointer = std::shared_ptr<const virtual_orbital_type>;

    /// Type of the core molecular orbitals
    using core_orbital_type = InactiveSpaceType;

    /// Type of a read-only reference to the core orbitals
    using const_core_reference = const core_orbital_type&;

    /// Type of a read-only pointer to the core orbitals
    using const_core_pointer = std::shared_ptr<const core_orbital_type>;

    /// Type of the active space 1RDM representing the CI state
    using active_1rdm_type = OneElectronDensity<ActiveSpaceType>;

    /// Type of a read-only reference to the active space 1RDM
    using const_active_1rdm_reference = const active_1rdm_type&;

    /// Type of a read-only pointer to the active space 1RDM
    using const_active_1rdm_pointer = std::shared_ptr<const active_1rdm_type>;

    /// Type of the active space 2RDM representing the CI state
    using active_2rdm_type = TwoElectronDensity<ActiveSpaceType>;

    /// Type of a read-only reference to the active space 2RDM
    using const_active_2rdm_reference = const active_2rdm_type&;

    /// Type of a read-only pointer to the active space 2RDM
    using const_active_2rdm_pointer = std::shared_ptr<const active_2rdm_type>;

    

    /** @brief Creates an active space with all default-initialized members.
     *
     *  The instance resulting from this ctor has all default initialized
     *  members and is thus suitable for wavefunctions involving zero electrons.
     *  To make the instance "useful" one has to copy/move assign to it.
     *
     *  @throw None No throw guarantee.
     */
    ConfigurationInteractionSpace() = default;

    /** @brief Creates an active space with the provided state.
     *
     *  @param[in] n_act_alpha Number of alpha electrons
     *  @param[in] n_act_beta  Number of beta electrons
     *  @param[in] inact The orbitals that are inactive in the multiconfigurational 
     *                   wavefunction.
     *  @param[in]   act The orbitals that are active in the multiconfigurational 
     *                   wavefunction.
     *  @param[in]  virt The orbitals that are virtual in the multiconfigurational 
     *                   wavefunction.
     *  @param[in]  core The orbitals that are core (inactive) in the 
     *                   multiconfigurational  wavefunction.
     *  @param[in]  ordm The active-space 1RDM corresponding to the CI state
     *  @param[in]  trdm The active-space 2RDM corresponding to the CI state
     *
     *  @throw None No throw gurantee
     */
    ConfigurationInteractionSpace(size_type n_act_alpha, size_type n_act_beta,
                inactive_orbital_type inact, active_orbital_type act,
                virtual_orbital_type virt, core_orbital_type core,
                active_1rdm_type ordm, active_2rdm_type trdm);


    /** @brief Creates an active space by aliasing the provided state.
     *
     *
     *  @param[in] n_act_alpha Number of alpha electrons
     *  @param[in] n_act_beta  Number of beta electrons
     *  @param[in] pinact  The inactive orbitals to alias.
     *  @param[in] pact    The active orbitals to alias.
     *  @param[in] pvirt   The virtual orbitals to alias.
     *  @param[in] pcore   The core orbitals to alias.
     *  @param[in] pordm   The 1RDM to alias
     *  @param[in] ptrdm   The 2RDM to alias
     *
     *  @throw None No throw guarantee.
     */
    ConfigurationInteractionSpace(size_type n_act_alpha, size_type n_act_beta,
                const_inactive_pointer pinact, const_active_pointer pact,
                const_virtual_pointer pvirt, const_core_pointer pcore,
                const_active_1rdm_pointer pordm, const_active_2rdm_pointer ptrdm);

    size_type n_active_alpha_electrons() const;
    size_type n_active_beta_electrons() const;

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

    /** @brief The active space 1RDM associated with this CI state.
     *
     *  @return The active space 1RDM associated with this CI state.
     *
     *  @throw std::runtime_error if the 1RDM has not been set
     */
    const_active_1rdm_reference active_1rdm() const;

    /** @brief The active space 2RDM associated with this CI state.
     *
     *  @return The active space 2RDM associated with this CI state.
     *
     *  @throw std::runtime_error if the 2RDM has not been set
     */
    const_active_2rdm_reference active_2rdm() const;
    /** @brief Enables implicit upcasting of the orbital spaces in an active
     *         space.
     *
     *  The orbital spaces in an ConfigurationInteractionSpace are polymorphic. It sometimes
     *  occurs that we have an instance of type  `ConfigurationInteractionSpace<X, Y, Z, A>` and
     *  we want to pass it as if it was of type `ConfigurationInteractionSpace<T, U, V, W>` (where
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
     *  @return The current instance casted to a `ConfigurationInteractionSpace<T, U, V, W>`
     *          instance.
     *
     *  @throw None No throw guarantee.
     */
    template<typename T, typename U, typename V, typename W,
             typename = enable_if_convertible_t<T, U, V, W>>
    operator ConfigurationInteractionSpace<T, U, V, W>() const {
        return ConfigurationInteractionSpace<T, U, V, W>(m_n_act_alpha_,m_n_act_beta_,
          m_pinact_, m_pact_, m_pvirt_, m_pcore_, m_p1rdm_, m_p2rdm_);
    }

    /** @brief Determines if this instance is equal to another
     *
     *  Two ConfigurationInteractionSpace instances are equal if their inactive, active, virtual, 
     *  core and Fock operators compare equal.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance is equal to @p rhs and false otherwise.
     */
    bool operator==(const ConfigurationInteractionSpace& rhs) const;

    /** @brief Updates a hasher with the state of this active space.
     *
     *  @param[in,out] h The hasher to hash the ActiveSpace with. After
     *                   this call the internal state of @p h will have been
     *                   updated with hashes of this instance's state.
     *
     */
    void hash(chemist::detail_::Hasher& h) const;

private:
    /// Number of active alpha electrons
    size_type m_n_act_alpha_ = 0;

    /// Number of active beta electrons
    size_type m_n_act_beta_ = 0;

    /// The inactive orbitals
    const_inactive_pointer m_pinact_;

    /// The active orbitals
    const_active_pointer m_pact_;

    /// The virtual orbitals
    const_virtual_pointer m_pvirt_;

    /// The core orbitals
    const_core_pointer m_pcore_;

    /// The active space 1RDM
    const_active_1rdm_pointer m_p1rdm_;

    /// The active space 2RDM
    const_active_2rdm_pointer m_p2rdm_;
};

/** @brief Compares two ConfigurationInteractionSpace instances for equality.
 *  @relates ConfigurationInteractionSpace
 *
 *  Two ConfigurationInteractionSpace instances are equal if their inactive, active, virtual, 
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
 *  @param[in] lhs The ConfigurationInteractionSpace on the left of the equality.
 *  @param[in] rhs The ConfigurationInteractionSpace on the right of the equality.
 *
 *  @return True if @p lhs is equal to @p rhs and false otherwise.
 */
template<typename LHSInactSpace, typename LHSActSpace, typename LHSVirtSpace, 
         typename LHSCoreSpace, typename RHSInactSpace, typename RHSActSpace,
         typename RHSVirtSpace, typename RHSCoreSpace>
bool operator==(const ConfigurationInteractionSpace<LHSInactSpace, LHSActSpace, LHSVirtSpace, LHSCoreSpace>& lhs,
                const ConfigurationInteractionSpace<RHSInactSpace, RHSActSpace, RHSVirtSpace, RHSCoreSpace>& rhs) {
    if constexpr(!std::is_convertible_v<decltype(rhs), decltype(lhs)>) {
        return false;
    } else {
        return lhs.operator==(rhs);
    }
}

/** @brief Determiness if two ConfigurationInteractionSpace instances are different.
 *  @relates ConfigurationInteractionSpace
 *
 *  Two ConfigurationInteractionSpace instances are equal if their occupied, virtual, and
 *  Fock operators compare equal. They are different if they are not equal.
 *
 *  @tparam LHSOccSpace the type of the occupied space in @p lhs.
 *  @tparam LHSVirtSpace the type of the virtual space in @p lhs.
 *  @tparam RHSOccSpace the type of the occupied space in @p rhs.
 *  @tparam RHSVirtSpace the type of the virtual space in @p rhs.
 *
 *  @param[in] lhs The ConfigurationInteractionSpace on the left of the not equal operator.
 *  @param[in] rhs The ConfigurationInteractionSpace on the right of the non equal operator.
 *
 *  @return False if @p lhs is equal to @p rhs and true otherwise.
 */
template<typename LHSInactSpace, typename LHSActSpace, typename LHSVirtSpace, 
         typename LHSCoreSpace, typename RHSInactSpace, typename RHSActSpace,
         typename RHSVirtSpace, typename RHSCoreSpace>
bool operator!=(const ConfigurationInteractionSpace<LHSInactSpace, LHSActSpace, LHSVirtSpace, LHSCoreSpace>& lhs,
                const ConfigurationInteractionSpace<RHSInactSpace, RHSActSpace, RHSVirtSpace, RHSCoreSpace>& rhs) {
    return !(lhs == rhs);
}

/// Type of a determinant space which uses derived orbitals
using DerivedConfigurationInteractionSpace = ConfigurationInteractionSpace<orbital_space::DerivedSpaceD>;

extern template class ConfigurationInteractionSpace<orbital_space::DerivedSpaceD>;

} // namespace chemist::wavefunction
