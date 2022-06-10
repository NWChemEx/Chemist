#include "chemist/wavefunction/many_body.hpp"

namespace chemist::wavefunction {

#define MANY_BODY_TPARAMS template<typename ReferenceType, typename TensorType>
#define MANY_BODY ManyBody<ReferenceType, TensorType>

MANY_BODY_TPARAMS
MANY_BODY::ManyBody(reference_wf_type ref, amplitude_type t2,
                    amplitude_type t1) :
  ManyBody(std::make_shared<reference_wf_type>(std::move(ref)),
           std::make_shared<amplitude_type>(std::move(t2)),
           std::make_shared<amplitude_type>(std::move(t1))) {}

MANY_BODY_TPARAMS
MANY_BODY::ManyBody(const_reference_wf_pointer pref,
                    const_amplitude_pointer pt2, const_amplitude_pointer pt1) :
  m_pref_(pref), m_amplitudes_{pt1, pt2} {}

MANY_BODY_TPARAMS
typename MANY_BODY::const_reference_wf_reference
MANY_BODY::reference_wavefunction() const {
    if(m_pref_) return *m_pref_;
    throw std::runtime_error("No reference wavefunction set");
}

MANY_BODY_TPARAMS
bool MANY_BODY::operator==(const ManyBody& rhs) const {
    if(m_pref_) {
        if(!rhs.m_pref_) return false;
        if(reference_wavefunction() != rhs.reference_wavefunction())
            return false;
    } else {
        if(rhs.m_pref_) return false;
    }

    if(m_order_ != rhs.m_order_) {
        return false;
    } else {
        for(std::size_t i = 0; i < m_order_; ++i) {
            auto t_lhs = m_amplitudes_[i];
            auto t_rhs = rhs.m_amplitudes_[i];
            if(t_lhs) {
                if(!t_rhs) return false; // LHS has amplitude, RHS doesn't
                if((*t_lhs) != (*t_rhs)) return false; // Both have, not equal
            } else {
                if(t_rhs) return false; // LHS doesn't have, RHS does
            }
        }
        return true; // Getting here means the amplitudes are the same
    }
}

MANY_BODY_TPARAMS
void MANY_BODY::hash(chemist::detail_::Hasher& h) const {
    h(m_pref_, m_amplitudes_);
}

#undef MANY_BODY
#undef MANY_BODY_TPARAMS

template class ManyBody<Reference, type::tensor>;
template class ManyBody<CanonicalReference, type::tensor>;
template class ManyBody<SparseReference, type::tensor>;
template class ManyBody<SparseToTReference, type::tensor_of_tensors>;

} // namespace chemist::wavefunction
