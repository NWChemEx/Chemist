#include "libchemist/ta_helpers/remove_redundancy.hpp"
#include "libchemist/tensor/remove_redundancy.hpp"
#include "libchemist/tensor/tensor_wrapper.hpp"

namespace libchemist::tensor {

type::SparseTensorWrapper remove_redundancy(const type::SparseTensorWrapper& C,
                                            const type::SparseTensorWrapper& S,
                                            double thresh) {
    const auto& C_ta = C.get<TA::TSpArrayD>();
    const auto& S_ta = S.get<TA::TSpArrayD>();
    auto new_C       = ta_helpers::remove_redundancy(C_ta, S_ta, thresh);
    return type::SparseTensorWrapper(std::move(new_C));
}

} // namespace libchemist::tensor