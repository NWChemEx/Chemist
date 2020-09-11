#include "libchemist/orbital_space/orbital_space.hpp"

namespace libchemist::orbital_space {

template class AOSpace_<type::tensor<double>, BaseSpace<double>>;
template class AOSpace_<type::tensor<float>, BaseSpace<float>>;

}
