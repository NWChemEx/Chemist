#pragma once
#include "libchemist/wavefunction/nonrelativistic.hpp"

namespace libchemist::wavefunction {

template<typename ReferenceType, typename TensorType>
class Perturbative {
public:
    using reference_wf_type = ReferenceType;
    using tensor_type       = TensorType;

    Perturbative() = default;
    Perturbative(reference_wf_type ref, tensor_type t2, tensor_type t1 = {});

private:
    reference_wf_type m_ref_;
    tensor_type m_t1_;
    tensor_type m_t2_;
};

/// Type of a perturbative correlated wavefunction, using MOs
using Perturbative = Perturbative<Reference>;

/// Type of a perturbative correlated wavefunction, using canonical MOs
using CanonicalPerturbative = Perturbative<CanonicalReference>;

/// Type of a perturbative correlated wavefunction built from local MOs
using LocalPerturbative = Perturbative<LocalReference>;

/// Type of a wavefunction built frorm quasi-canonical local MOs
using CanonicalLocalPertrubative = Perturbative<CanonicalLocalReference>;

} // namespace libchemist::wavefunction

#include "perturbative.ipp"
