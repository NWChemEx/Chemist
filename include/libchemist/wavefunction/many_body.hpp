#pragma once
#include "libchemist/wavefunction/nonrelativistic.hpp"

namespace libchemist::wavefunction {

/** @brief Wavefunction for many-body theories.
 *
 *  This wavefunction is designed for use with Moller-Plesset perturbation
 *  theory and coupled cluster theory. At the moment it is only capable of
 *  holding single and double excitation amplitudes, thus modifications are
 *  required for higher-orders.
 *
 *  @tparam ReferenceType Type of the reference wavefunction.
 *  @tparam TensorType Type of the tensor holding the amplitudes
 */
template<typename ReferenceType, typename TensorType>
class ManyBody {
private:
    /// True if ReferenceType can be implicitly converted to @p T
    template<typename T>
    static constexpr bool reference_is_convertible_v =
      std::is_convertible_v<ReferenceType, T>;

    /// Enables a function when ReferenceType can be implicitly converte to @p T
    template<typename T>
    using enable_if_convertible_t =
      std::enable_if<reference_is_convertible_v<T>>;

public:
    /// Type of the reference wavefunction
    using reference_wf_type = ReferenceType;

    /// Type of a read-only reference to the reference wavefunction
    using const_reference_wf_reference = const reference_wf_type&;

    /// Type of a read-only pointer to the reference wavefunction
    using const_reference_wf_pointer = std::shared_ptr<const reference_wf_type>;

    /// Type of the amplitudes
    using amplitude_type = TensorType;

    /// Type of a read-only reference to the amplitudes
    using const_amplitude_reference = const amplitude_type&;

    /// Type of a read-only pointer to the amplitudes
    using const_amplitude_pointer = std::shared_ptr<const amplitude_type>;

    /** @brief Creates a defaulted many-body wavefunction.
     *
     *  The wavefunction resulting from this ctor has no state. State can be
     *  given to the instance by assigning to it another many-body wavefunction.
     *
     *  @throw None No throw guarantee.
     */
    ManyBody() = default;

    /** @brief Creates a ManyBody wavefunction with the provided state.
     *
     *  @param[in] ref The wavefunction used as the reference.
     *  @param[in] t2  The amplitudes associated with pairwise excitations.
     *                 Default is an empty tensor.
     *  @param[in] t1  The  amplitudes associated with single excitations.
     *                 Default is an empty tensor.
     *  @throw std::bad_alloc if allocating the shared_ptrs fails. Strong throw
     *                        guarantee.
     */
    explicit ManyBody(reference_wf_type ref, amplitude_type t2 = {},
                      amplitude_type t1 = {});

    /** @brief Makes a ManyBody wavefunction by aliasing the state of another
     *         instance.
     *
     *  @param[in] pref A shared_ptr to the reference wavefunction.
     *  @param[in] pt2  A shared_ptr to the amplitudes generated by pairwise
     *                  excitations.
     *  @param[in] pt1  A shared_ptr to the amplitudes generated by single
     *                  excitations.
     *
     *  @throw None No throw guarantee.
     */
    ManyBody(const_reference_wf_pointer pref, const_amplitude_pointer pt2,
             const_amplitude_pointer pt1 = {});

    /** @brief Enables implicit conversions among ManyBody wavefunctions.
     *
     *  @tparam T The reference type we are converting to.
     *  @tparam <anonymous> A parameter used to disable this conversion, via
     *                      SFINAE, when @p ReferenceType is not convertible to
     *                      @p T.
     *
     *  @return A ManyBody<T, TensorType> instance which aliases the state of
     *          this instance.
     */
    template<typename T, typename = enable_if_convertible_t<T>>
    operator ManyBody<T, TensorType>() const;

    /** @brief Returns the wavefunction this wavefunction is based on.
     *
     *  @return A read-only reference to the reference wavefunction.
     *
     *  @throw std::runtime_error if the reference has not been set. Strong
     *                            throw guarantee.
     */
    const_reference_wf_reference reference_wavefunction() const;

    /** @brief Returns the @p N (plus 1) particle amplitudes
     *
     *  @tparam N Following usual C++ convention, @p N starts at 0. So the one
     *            particle amplitudes are returned when @p N == 0, the two
     *            particle amplitudes are returned when @p N == 1, etc.
     *
     *  @note This function will trip a static assert if @p N is not valid.
     *
     *  @throw std::runtime_error if the amplitudes were not set. Strong throw
     *                            guarantee.
     */
    template<std::size_t N>
    const_amplitude_reference amplitudes() const;

    /// Convenience function for returning the one particle amplitudes
    const_amplitude_reference t1() const { return amplitudes<0>(); }

    /// Convenience function for returning the two-particle amplitudes
    const_amplitude_reference t2() const { return amplitudes<1>(); }

    bool operator==(const ManyBody& rhs) const;

    void hash(pluginplay::Hasher& h) const;

    // TODO: actual implementation, documentation and testing
    template<typename Archive,
             typename = std::enable_if_t<madness::is_output_archive_v<Archive>>>
    void serialize(Archive ar) const {}

    template<typename Archive,
             typename = std::enable_if_t<madness::is_input_archive_v<Archive>>>
    void serialize(Archive ar) {}

private:
    /// Maximum number of excitations allowed
    static constexpr std::size_t m_order_ = 2;

    /// The reference determinant
    const_reference_wf_pointer m_pref_;

    /// The amplitudes
    std::array<const_amplitude_pointer, m_order_> m_amplitudes_;
};

// -----------------------------------------------------------------------------
//                                Typedefs
// -----------------------------------------------------------------------------

/// Type of a ManyBody correlated wavefunction, using MOs
using ManyBodyWf = ManyBody<Reference, type::tensor>;

/// Type of a ManyBody correlated wavefunction, using canonical MOs
using CanonicalManyBodyWf = ManyBody<CanonicalReference, type::tensor>;

/// Type of a ManyBody correlated wavefunction using localized orbitals
using SparseManyBodyWf = ManyBody<SparseReference, type::tensor>;

/// Type of a ManyBody correlated wavefunction using tensor of tensors
using SparseToTManyBodyWf =
  ManyBody<SparseToTReference, type::tensor_of_tensors>;
// -----------------------------------------------------------------------------
//                             Related Functions
// -----------------------------------------------------------------------------

template<typename LHSReferenceType, typename LHSTensorType,
         typename RHSReferenceType, typename RHSTensorType>
bool operator==(const ManyBody<LHSReferenceType, LHSTensorType>& lhs,
                const ManyBody<RHSReferenceType, RHSTensorType>& rhs) {
    if constexpr(!std::is_convertible_v<decltype(rhs), decltype(lhs)>) {
        return false;
    } else {
        return lhs.operator==(rhs);
    }
}

template<typename LHSReferenceType, typename LHSTensorType,
         typename RHSReferenceType, typename RHSTensorType>
bool operator!=(const ManyBody<LHSReferenceType, LHSTensorType>& lhs,
                const ManyBody<RHSReferenceType, RHSTensorType>& rhs) {
    return !(lhs == rhs);
}

// -----------------------------------------------------------------------------
//                             Inline Implementations
// -----------------------------------------------------------------------------

template<typename ReferenceType, typename TensorType>
template<typename T, typename>
ManyBody<ReferenceType, TensorType>::operator ManyBody<T, TensorType>() const {
    auto new_ref = std::make_shared<T>(m_pref_ ? T(*m_pref_) : T());
    return ManyBody<T, TensorType>(new_ref, m_amplitudes_[1], m_amplitudes_[0]);
}

template<typename ReferenceType, typename TensorType>
template<std::size_t N>
typename ManyBody<ReferenceType, TensorType>::const_amplitude_reference
ManyBody<ReferenceType, TensorType>::amplitudes() const {
    if(m_amplitudes_[N]) return *(m_amplitudes_[N]);
    std::string msg = std::to_string(N + 1) + "-body amplitudes were not set.";
    throw std::runtime_error(msg);
}

// -----------------------------------------------------------------------------
//                       Template Instantiations
// -----------------------------------------------------------------------------

extern template class ManyBody<Reference, type::tensor>;
extern template class ManyBody<CanonicalReference, type::tensor>;
extern template class ManyBody<SparseReference, type::tensor>;
extern template class ManyBody<SparseToTReference, type::tensor_of_tensors>;

} // namespace libchemist::wavefunction
