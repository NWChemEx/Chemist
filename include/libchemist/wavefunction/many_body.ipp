/** @brief ManyBody.ipp
 *
 *  Implementations of the wavefunction::ManyBody class. This file should
 *  not be included from any file except for
 *  libchemist/wavefunction/ManyBody.hpp
 */
#pragma once

namespace libchemist::wavefunction {

#define MANYBODY ManyBody<ReferenceType, TensorType>

template<typename ReferenceType, typename TensorType>
MANYBODY::ManyBody(reference_wf_type ref, tensor_type t2, tensor_type t1) :
  m_ref_(std::move(ref)), m_t1_(std::move(t1)), m_t2_(std::move(t2)) {}

template<typename ReferenceType, typename TensorType>
void MANYBODY::hash(pluginplay::Hasher& h) const {
    h(m_ref_, m_t1_, m_t2_);
}

#undef ManyBody

} // namespace libchemist::wavefunction
