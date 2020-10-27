#pragma once
#include "libchemist/orbital_space.hpp"

namespace libchemist {

// This class wraps OrbitalSpace for physically-motivated, type-enforcing
// reasons
template<typename element_type = double,
         typename tensor_type  = type::tensor<element_type>>
class OrthogonalSpace : public OrbitalSpace<element_type, tensor_type> {
public:
    using OrbitalSpace<element_type, tensor_type>::OrbitalSpace;
};

extern template class OrthogonalSpace<float, type::tensor<float>>;
extern template class OrthogonalSpace<float, type::tensor<double>>;
extern template class OrthogonalSpace<double, type::tensor<float>>;
extern template class OrthogonalSpace<double, type::tensor<double>>;

} // namespace libchemist