#include "orthogonal_space.hpp"

namespace chemist {

template class OrthogonalSpace<float, type::tensor<float>>;
template class OrthogonalSpace<float, type::tensor<double>>;
template class OrthogonalSpace<double, type::tensor<float>>;
template class OrthogonalSpace<double, type::tensor<double>>;

} // namespace chemist