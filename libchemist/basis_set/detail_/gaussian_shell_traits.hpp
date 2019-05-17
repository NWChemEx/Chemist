#pragma once
#include "libchemist/types.hpp"
#include <array>
#include <vector>

namespace libchemist::basis_set::detail_ {

template<typename CoefArray, typename ExpArray, typename CenterType,
         bool IsPure = true, typename AMType = type::angular_momentum>
struct GaussianShellTraits_ {
    using center_type             = CenterType;
    using coef_array              = CoefArray;
    using exp_array               = ExpArray;
    using am_type                 = AMType;
    static constexpr bool is_pure = IsPure;
};

template<typename T, type::size N, bool IsPure = true,
         typename AMType = type::angular_momentum>
using StaticGuassianShellTraits =
  GaussianShellTraits_<std::array<T, N>, std::array<T, N>, type::coord<T>,
                       IsPure, AMType>;

} // namespace libchemist::basis_set::detail_
