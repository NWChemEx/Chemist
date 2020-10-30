#pragma once
#include "libchemist/ta_helpers/einsum/detail_/detail_.hpp"
#include "libchemist/ta_helpers/einsum/detail_/index_map.hpp"
#include <tiledarray.h>

namespace libchemist::ta_helpers::einsum::detail_ {

template<std::size_t... Is>
auto make_range_(const std::vector<types::range>& ranges,
                 std::index_sequence<Is...>) {
    return TA::Range({ranges[Is]...});
}

template<typename std::size_t N>
auto make_range_(const std::vector<types::range>& ranges) {
    return make_range_(ranges, std::make_index_sequence<N>());
}

inline auto make_range(const std::vector<types::range>& ranges) {
    switch(ranges.size()) {
        case(1): {
            return make_range_<1>(ranges);
        }
        case(2): {
            return make_range_<2>(ranges);
        }
        case(3): {
            return make_range_<3>(ranges);
        }
        case(4): {
            return make_range_<4>(ranges);
        }
        case(5): {
            return make_range_<5>(ranges);
        }
        case(6): {
            return make_range_<6>(ranges);
        }
        case(7): {
            return make_range_<7>(ranges);
        }
        default: {
            throw std::logic_error("I got tired of adding cases");
        }
    }
}

template<typename LHSType, typename RHSType>
auto block_kernel(const IndexMap& indices, const types::assoc_range& ranges,
                  LHSType&& lhs, RHSType&& rhs) {
    std::decay_t<LHSType> result(make_range(indices.select_result(ranges)), 0);
    auto index = detail_::initial_index(ranges);

    do {
        // Get integral indices for elements of tensors
        auto&& [oindex, lindex, rindex] = indices.select(index);

        // Use integral indices to get the elements
        auto& result_elem = result(oindex);
        const auto& lelem = lhs(lindex);
        const auto& relem = rhs(rindex);

        // Combine the elements
        result_elem += lelem * relem;

    } while(!detail_::increment_index(index, ranges));
    return result;
}

} // namespace libchemist::einsum::detail_