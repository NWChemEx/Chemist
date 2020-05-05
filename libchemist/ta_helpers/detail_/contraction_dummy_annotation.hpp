#pragma once
#include <tiledarray.h>

namespace libchemist::detail_ {

// Makes indices for the contraction of one dimension of an arbitrarily ranked tensor with a matrix
inline std::tuple<std::string, std::string, std::string>
        contraction_dummy_annotations(std::size_t DIM, std::size_t TARGET_DIM) {

    auto beginning = TA::detail::dummy_annotation(DIM);

    std::ostringstream final;
    if (DIM > 0) {
        if (TARGET_DIM == 0) { final << "iX"; }
        else { final << "i0"; }
    }
    for (unsigned int d = 1; d < DIM; ++d) {
        if (d == TARGET_DIM) { final << ",iX"; }
        else { final << ",i" << d; }
    }

    std::ostringstream switcher;
    switcher << "i" << TARGET_DIM << ",iX";

    return {beginning, final.str(), switcher.str()};
}

} // namespace libchemist::detail_