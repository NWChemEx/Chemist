#include "shape_pimpl.hpp"

namespace chemist::tensor::detail_ {

template class ShapePIMPL<field::Scalar>;
template class ShapePIMPL<field::Tensor>;

} // namespace chemist::tensor::detail_
