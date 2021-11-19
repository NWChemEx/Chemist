#include "chemist/tensor/conversions.hpp"

namespace chemist::tensor {

std::vector<double> to_vector(const ScalarTensorWrapper& t) {
    auto t_ta = t.get<TA::TSpArrayD>();
    t_ta.make_replicated();
    std::vector<double> rv(t.size(), 0.0);

    for(const auto& tile_i : t_ta) {
        const auto& i_range = tile_i.get().range();
        for(auto idx : i_range) rv[i_range.ordinal(idx)] = tile_i.get()[idx];
    }
    return rv;
}

} // namespace chemist::tensor
