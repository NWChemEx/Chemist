#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/allclose.hpp"
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

bool allclose(const libchemist::type::tensor& actual,
              const libchemist::type::tensor& ref, double rtol, double atol) {
    using tensor_type = tensor::type::detail_::tensor<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    return ta_helpers::allclose(a, r, false, rtol, atol);
}

bool allclose(const libchemist::type::tensor_of_tensors& actual,
              const libchemist::type::tensor_of_tensors& ref, double rtol,
              double atol) {
    using tensor_type = tensor::type::detail_::tensor_of_tensors<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    auto inner_rank = actual.rank() - a.trange().rank();
    return ta_helpers::allclose_tot(a, r, inner_rank, false, rtol, atol);
}

bool abs_allclose(const libchemist::type::tensor& actual,
                  const libchemist::type::tensor& ref, double rtol,
                  double atol) {
    using tensor_type = tensor::type::detail_::tensor<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    return ta_helpers::allclose(a, r, true, rtol, atol);
}

bool abs_allclose(const libchemist::type::tensor_of_tensors& actual,
                  const libchemist::type::tensor_of_tensors& ref, double rtol,
                  double atol) {
    using tensor_type = tensor::type::detail_::tensor_of_tensors<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    auto inner_rank = actual.rank() - a.trange().rank();
    return ta_helpers::allclose_tot(a, r, inner_rank, true, rtol, atol);
}

} // namespace libchemist::tensor
