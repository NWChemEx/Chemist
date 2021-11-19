#include "chemist/ta_helpers/ta_helpers.hpp"
#include "chemist/tensor/allclose.hpp"
#include "chemist/tensor/detail_/backends/tiled_array.hpp"

namespace chemist::tensor {

using ta_tensor_type =
  backends::TiledArrayTraits<field::Scalar>::tensor_type<double>;
using ta_tot_type =
  backends::TiledArrayTraits<field::Tensor>::tensor_type<double>;

bool allclose(const ScalarTensorWrapper& actual, const ScalarTensorWrapper& ref,
              double rtol, double atol) {
    using tensor_type = ta_tensor_type;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    return ta_helpers::allclose(a, r, false, rtol, atol);
}

bool allclose(const TensorOfTensorsWrapper& actual,
              const TensorOfTensorsWrapper& ref, double rtol, double atol) {
    using tensor_type = ta_tot_type;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    auto inner_rank = actual.rank() - a.trange().rank();
    return ta_helpers::allclose_tot(a, r, inner_rank, false, rtol, atol);
}

bool abs_allclose(const ScalarTensorWrapper& actual,
                  const ScalarTensorWrapper& ref, double rtol, double atol) {
    using tensor_type = ta_tensor_type;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    return ta_helpers::allclose(a, r, true, rtol, atol);
}

bool abs_allclose(const TensorOfTensorsWrapper& actual,
                  const TensorOfTensorsWrapper& ref, double rtol, double atol) {
    using tensor_type = ta_tot_type;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    auto inner_rank = actual.rank() - a.trange().rank();
    return ta_helpers::allclose_tot(a, r, inner_rank, true, rtol, atol);
}

} // namespace chemist::tensor
