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
public:
    /// Type of the reference wavefunction
    using reference_wf_type = ReferenceType;
    /// Type of the amplitudes
    using tensor_type = TensorType;

    ManyBody() = default;
    explicit ManyBody(reference_wf_type ref, tensor_type t2 = {},
                      tensor_type t1 = {});

    const auto& reference_wavefunction() const { return m_ref_; }

    const auto& t1() const { return m_t1_; }

    const auto& t2() const { return m_t2_; }

    void hash(pluginplay::Hasher& h) const;

private:
    reference_wf_type m_ref_;
    tensor_type m_t1_;
    tensor_type m_t2_;
};

template<typename LHSReferenceType, typename LHSTensorType,
         typename RHSReferenceType, typename RHSTensorType>
bool operator==(const ManyBody<LHSReferenceType, LHSTensorType>& lhs,
                const ManyBody<RHSReferenceType, RHSTensorType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return false;
    } else {
        return std::tie(lhs.reference_wavefunction(), lhs.t1(), lhs.t2()) ==
               std::tie(rhs.reference_wavefunction(), rhs.t1(), rhs.t2());
    }
}

template<typename LHSReferenceType, typename LHSTensorType,
         typename RHSReferenceType, typename RHSTensorType>
bool operator!=(const ManyBody<LHSReferenceType, LHSTensorType>& lhs,
                const ManyBody<RHSReferenceType, RHSTensorType>& rhs) {
    return !(lhs == rhs);
}

/// Type of a ManyBody correlated wavefunction, using MOs
using ManyBodyWf = ManyBody<Reference, type::tensor>;

/// Type of a ManyBody correlated wavefunction, using canonical MOs
using CanonicalManyBodyWf = ManyBody<CanonicalReference, type::tensor>;

/// Type of a ManyBody correlated wavefunction built from local MOs
using LocalManyBodyWf = ManyBody<LocalReference, type::tensor_of_tensors>;

/// Type of a wavefunction built frorm quasi-canonical local MOs
using CanonicalLocalManyBodyWf =
  ManyBody<CanonicalLocalReference, type::tensor_of_tensors>;

} // namespace libchemist::wavefunction

#include "many_body.ipp"
