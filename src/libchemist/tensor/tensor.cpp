#include "libchemist/tensor/tensor.hpp"

namespace libchemist::tensor {

template class TensorWrapper<type::tensor_variant>;
template class TensorWrapper<type::tot_variant>;

} // namespace libchemist::tensor
