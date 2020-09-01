#include "libchemist/orbital_space/ao_space.hpp"

namespace libchemist {

#define OrbSpace OrbitalSpace<e_type, t_type>
#define AOS AOSpace<e_type, t_type>

template<typename e_type, typename t_type>
AOS::AOSpace() : OrbSpace() {}

template<typename e_type, typename t_type>
AOS::AOSpace(basis_type bs, t_type S) :
  OrbSpace(bs, S,
           TA::diagonal_array<t_type, tensor_elems>(S.world(), S.trange()),
           TA::diagonal_array<t_type, tensor_elems>(S.world(), S.trange()), S) {
}

template<typename e_type, typename t_type>
t_type AOS::transform_from_ao(const t_type X, const size_vec& modes) const {
    return X;
}

template<typename e_type, typename t_type>
t_type AOS::transform_to_ao(const t_type X, const size_vec& modes) const {
    return X;
}

#undef AOS
#undef OrbSpace

template class AOSpace<float, type::tensor<float>>;
template class AOSpace<float, type::tensor<double>>;
template class AOSpace<double, type::tensor<float>>;
template class AOSpace<double, type::tensor<double>>;
//template class AOSpace<float, type::tensor_of_tensors<float>>;
//template class AOSpace<float, type::tensor_of_tensors<double>>;
//template class AOSpace<double, type::tensor_of_tensors<float>>;
//template class AOSpace<double, type::tensor_of_tensors<double>>;

} // namespace libchemist
