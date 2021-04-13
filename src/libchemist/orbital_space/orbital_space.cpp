#include "libchemist/orbital_space/orbital_space.hpp"

namespace libchemist::orbital_space {
template class BaseSpace_<type::tensor<double>>;
template class BaseSpace_<type::tensor<float>>;
template class BaseSpace_<type::tensor_of_tensors<double>>;
template class BaseSpace_<type::tensor_of_tensors<float>>;

template class DependentBaseSpace_<sparse_map::SparseMapEE, SparseBase<double>>;
template class DependentBaseSpace_<sparse_map::SparseMapEE, SparseBase<float>>;

template class AOSpace_<AOBasisSet<double>, BaseSpace<double>>;
template class AOSpace_<AOBasisSet<float>, BaseSpace<float>>;
template class AOSpace_<AOBasisSet<double>, SparseDependentBase<double>>;
template class AOSpace_<AOBasisSet<float>, SparseDependentBase<float>>;

} // namespace libchemist::orbital_space
