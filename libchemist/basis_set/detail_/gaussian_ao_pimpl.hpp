#pragma once
#include "libchemist/basis_set/detail_/gaussian_ao_traits.hpp"
#include "libchemist/basis_set/detail_/gaussian_traits.hpp"
#include <tuple>

namespace libchemist::basis_set::detail_ {

template<typename Traits>
class GaussianAOPIMPL {
private:
    using coef_container = typename Traits::coef_container;
    using exp_container  = typename Traits::exp_container;

public:
    using coef_type   = typename Traits::coef_type;
    using exp_type    = typename Traits::exp_type;
    using center_type = typename Traits::center_type;
    using prim_traits = GaussianTraits<coef_type, exp_type, const center_type>;

    template<typename T, typename U, typename V>
    constexpr GaussianAOPIMPL(T&& c, U&& a, V&& r0) noexcept;

    constexpr auto size() const noexcept { return m_cs_.size(); }

    constexpr auto& coef(type::size i) noexcept { return m_cs_[i]; }
    constexpr const auto& coef(type::size i) const noexcept { return m_cs_[i]; }

    constexpr auto& exp(type::size i) noexcept { return m_as_[i]; }
    constexpr const auto& exp(type::size i) const noexcept { return m_as_[i]; }

    constexpr auto& center() noexcept { return m_center_; }
    constexpr const auto& center() const noexcept { return m_center_; }

private:
    coef_container m_cs_;
    exp_container m_as_;
    center_type m_center_;
};

template<typename... Args>
constexpr auto make_gaussian_ao_pimpl(Args&&... args) noexcept {
    constexpr auto nprims = sizeof...(Args);
    using tuple_type      = std::tuple<std::decay_t<Args>...>;
    using elem0_type      = std::tuple_element_t<0, tuple_type>;
    using coef_type       = typename elem0_type::coef_type;
    using exp_type        = typename elem0_type::exp_type;
    using center_type     = typename elem0_type::center_type;

    // Put the coefficents, exponents, and centers into arrays
    std::array<coef_type, nprims> cs{args.coef()...};
    std::array<exp_type, nprims> as{args.exp()...};
    auto tuple    = std::make_tuple(std::forward<Args>(args)...);
    center_type r = std::get<0>(tuple).center();

    using traits =
      StaticGaussianAOTraits<nprims, coef_type, exp_type, center_type>;
    using pimpl_type = GaussianAOPIMPL<traits>;

    return pimpl_type(std::move(cs), std::move(as), std::move(r));
}

template<typename Traits>
template<typename T, typename U, typename V>
constexpr GaussianAOPIMPL<Traits>::GaussianAOPIMPL(T&& c, U&& a,
                                                   V&& r0) noexcept :
  m_cs_(std::forward<T>(c)),
  m_as_(std::forward<U>(a)),
  m_center_(std::forward<V>(r0)) {}

} // namespace libchemist::basis_set::detail_
