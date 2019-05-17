#pragma once
#include "libchemist/types.hpp"
#include <array>
#include <vector>

namespace libchemist::basis_set::detail_ {

template<typename CenterType, typename ShellArray>
struct GaussianCenterTraits {
    using center_type = CenterType;
    using shell_array = ShellArray;
};

} // namespace libchemist::basis_set::detail_
