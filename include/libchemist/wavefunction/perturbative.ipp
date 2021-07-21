#pragma once

namespace libchemist::wavefunction {

#define PERTURBATIVE Perturbative<ReferenceType, ElementType>

template<typename ReferenceType, typename ElementType>
PERTURBATIVE::Perturbative(ReferenceType ref, tensor_type t2, tensor_type t1) :
  m_ref_(std::move(ref)), m_t1_(std::move(t1)), m_t2_(std::move(t2)) {}

#undef PERTURBATIVE

} // namespace libchemist::wavefunction
