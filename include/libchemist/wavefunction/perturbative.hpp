#pragma once
#include "libchemist/wavefunction/type_traits.hpp"

namespace libchemist::wavefunction {

template<typename ReferenceType, typename ElementType = double>
class Perturbative {
private:
    using t_type   = type::tensor<ElementType>;
    using tot_type = type::tensor_of_tensors<ElementType>;

public:
    constexpr bool is_local = is_local_v<ReferenceType>;
    using element_type      = ElementType;
    using tensor_type       = std::conditional_t<is_local, tot_type, t_type>;

    Perturbative(ReferenceType ref, tensor_type t2, tensor_type t1 = {});

private:
    ReferenceType m_ref_;
    tensor_type m_t1_;
    tensor_type m_t2_;
};

} // namespace libchemist::wavefunction

#include "perturbative.ipp"
