#pragma once
#include <chemist/enums.hpp>

namespace chemist::detail_ {

template<typename AngularMomentumType>
auto compute_n_aos(AngularMomentumType L, ShellType purity) {
    const bool is_pure = purity == ShellType::pure;
    // The "else" return value is the closed form for 2+L choose 2
    return is_pure ? (2ul * L + 1ul) : ((L * L + 3ul * L + 2ul) / 2ul);
}

} // namespace chemist::detail_