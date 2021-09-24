#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/apply_elementwise.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"

namespace libchemist::tensor {

using tensor = type::SparseTensorWrapper;

tensor apply_elementwise(const tensor& input,
                         const std::function<double(double)>& fxn) {
    const auto& t = input.get<TA::TSpArrayD>();
    return tensor(libchemist::ta_helpers::apply_elementwise(t, fxn));
}

void apply_elementwise_inplace(tensor& input,
                               const std::function<void(double&)>& fxn) {
    auto& t = input.get<TA::TSpArrayD>();
    libchemist::ta_helpers::apply_elementwise_inplace(t, fxn);
}

} // namespace libchemist::tensor
