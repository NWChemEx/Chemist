#pragma once
#include "libchemist/types.hpp"
#include <array>
#include <vector>

namespace libchemist::basis_set::detail_ {

template<typename CoefArray, typename ExpArray, typename CenterType>
struct GaussianAOTraits_ {
    using center_type    = CenterType;
    using coef_container = CoefArray;
    using exp_container  = ExpArray;
};

template<typename T>
using GaussianAOTraits =
  GaussianAOTraits_<std::vector<T>, std::vector<T>, type::coord<T>>;

template<typename T, type::size N>
using StaticGaussianAOTraits =
  GaussianAOTraits_<std::array<T, N>, std::array<T, N>, type::coord<T>>;

} // namespace libchemist::basis_set::detail_
