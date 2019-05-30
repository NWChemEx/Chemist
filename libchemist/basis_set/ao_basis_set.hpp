#pragma once
#include "libchemist/basis_set/detail_/ao_basis_set_traits.hpp"

namespace libchemist::basis_set {

template<typename T, typename Traits = AOBasisSetTraits<T>>
class AOBasisSet {
private:
public:
    using gaussian_traits =
      detail_::GaussianTraits<param_type, const coord_type>;
    using const_guassian =
      detail_::GaussianTraits<const param_type, const coord_type>;
    using ao_traits =
      detail_::GaussianAOTraits<const param_type, const coord_type>;
    using const_ao = ao_traits;
    using shell_traits =
      detail_::GaussianShellTraits<const param_type, const coord_type>

      using gaussian_view = GaussianView<my_type*> using const_view =
        GaussianView<const my_type*>

      constexpr auto& coord(type::size i) noexcept;
    constexpr const auto& coord(type::size i) const noexcept;

    constexpr auto& coef(type::size i, type::size j, type::size k,
                         type::size l) constexpr

      constexpr GaussianCenter<T> center(type::size i)
};

/// AOBasisSet object with # of parameters and shells known at compile-time
template<typename T, std::size_t NParam, std::size_t NShell,
         std::size_t NCoord = 1>
using StaticAOBasisSet =
  AOBasisSet<T, StaticAOBasisSetTraits<T, NParam, NShell, NCoord>>;

} // namespace libchemist::basis_set
