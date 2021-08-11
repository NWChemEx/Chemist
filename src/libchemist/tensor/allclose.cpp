#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/allclose.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"
#include "libchemist/tensor/types.hpp"

namespace libchemist::tensor {

bool allclose(const type::SparseTensorWrapper& actual,
              const type::SparseTensorWrapper& ref, double rtol, double atol) {
    using tensor_type = tensor::type::detail_::tensor<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    return ta_helpers::allclose(a, r, false, rtol, atol);
}

bool allclose(const type::ToTWrapper& actual, const type::ToTWrapper& ref,
              double rtol, double atol) {
    using tensor_type = tensor::type::detail_::tensor_of_tensors<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    auto inner_rank = actual.rank() - a.trange().rank();
    return ta_helpers::allclose_tot(a, r, inner_rank, false, rtol, atol);
}

bool abs_allclose(const type::SparseTensorWrapper& actual,
                  const type::SparseTensorWrapper& ref, double rtol,
                  double atol) {
    using tensor_type = tensor::type::detail_::tensor<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    return ta_helpers::allclose(a, r, true, rtol, atol);
}

bool abs_allclose(const type::ToTWrapper& actual, const type::ToTWrapper& ref,
                  double rtol, double atol) {
    using tensor_type = tensor::type::detail_::tensor_of_tensors<double>;
    const auto& a     = actual.get<tensor_type>();
    const auto& r     = ref.get<tensor_type>();

    auto inner_rank = actual.rank() - a.trange().rank();
    return ta_helpers::allclose_tot(a, r, inner_rank, true, rtol, atol);
}

} // namespace libchemist::tensor
