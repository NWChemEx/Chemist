#pragma once
#include <tiledarray.h>

namespace libchemist::ta_helpers::detail_ {

// Makes indices for the contraction of one dimension of an arbitrarily ranked tensor with a matrix
inline std::tuple<std::string, std::string, std::string>
        contraction_dummy_annotations(std::size_t dim, std::size_t target_dim) {

    auto beginning = TA::detail::dummy_annotation(dim);

    std::ostringstream final;
    if (dim > 0) {
        if (target_dim == 0) { final << "iX"; }
        else { final << "i0"; }
    }
    for (unsigned int d = 1; d < dim; ++d) {
        if (d == target_dim) { final << ",iX"; }
        else { final << ",i" << d; }
    }

    std::ostringstream switcher;
    switcher << "i" << target_dim << ",iX";

    return {beginning, final.str(), switcher.str()};
}

} // namespace libchemist::detail_