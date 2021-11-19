#include "chemist/ta_helpers/ta_helpers.hpp"
#include "chemist/tensor/apply_elementwise.hpp"

namespace chemist::tensor {

using tensor = ScalarTensorWrapper;

tensor apply_elementwise(const tensor& input,
                         const std::function<double(double)>& fxn) {
    const auto& t = input.get<TA::TSpArrayD>();
    return tensor(chemist::ta_helpers::apply_elementwise(t, fxn));
}

void apply_elementwise_inplace(tensor& input,
                               const std::function<void(double&)>& fxn) {
    auto& t = input.get<TA::TSpArrayD>();
    chemist::ta_helpers::apply_elementwise_inplace(t, fxn);
}

} // namespace chemist::tensor
