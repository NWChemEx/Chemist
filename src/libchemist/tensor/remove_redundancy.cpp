#include "../ta_helpers/remove_redundancy.hpp"
#include "libchemist/tensor/remove_redundancy.hpp"

namespace libchemist::tensor {

ScalarTensorWrapper remove_redundancy(const ScalarTensorWrapper& C,
                                      const ScalarTensorWrapper& S,
                                      double thresh) {
    const auto& C_ta = C.get<TA::TSpArrayD>();
    const auto& S_ta = S.get<TA::TSpArrayD>();
    auto new_C       = ta_helpers::remove_redundancy(C_ta, S_ta, thresh);
    return ScalarTensorWrapper(std::move(new_C));
}

} // namespace libchemist::tensor
