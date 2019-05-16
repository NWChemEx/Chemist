#pragma once
#include "libchemist/types.hpp"
#include <array>
#include <vector>

namespace libchemist::basis_set::detail_ {

template<typename CoefType, typename ExpType, typename CenterType,
         typename CoefArray = std::vector<CoefType>,
         typename ExpArray  = std::vector<ExpType>>
struct GaussianAOTraits {
    using coef_type      = CoefType;
    using exp_type       = ExpType;
    using center_type    = CenterType;
    using coef_container = CoefArray;
    using exp_container  = ExpArray;
};

template<type::size N, typename CoefType, typename ExpType, typename CenterType>
using StaticGaussianAOTraits =
  GaussianAOTraits<CoefType, ExpType, CenterType, std::array<CoefType, N>,
                   std::array<ExpType, N>>;

} // namespace libchemist::basis_set::detail_
