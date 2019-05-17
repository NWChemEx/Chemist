#pragma once
#include "libchemist/basis_set/detail_/gaussian_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include <tuple>

namespace libchemist::basis_set {

template<typename Traits>
class Gaussian_ {
public:
    template<typename T, typename U, typename V>
    constexpr Gaussian_(T&& c, U&& a, V&& r0) noexcept;

    constexpr auto& center() noexcept { return *m_center_; }
    constexpr const auto& center() const noexcept { return *m_center_; }

    constexpr auto& exp() noexcept { return *m_exp_; }
    constexpr const auto& exp() const noexcept { return *m_exp_; }

    constexpr auto& coef() noexcept { return *m_coef_; }
    constexpr const auto& coef() const noexcept { return *m_coef_; }

private:
    detail_::holder<typename Traits::coef_type> m_coef_;
    detail_::holder<typename Traits::exp_type> m_exp_;
    detail_::holder<typename Traits::center_type> m_center_;
};

template<typename T>
using Gaussian = Gaussian_<detail_::GaussianTraits<T>>;

template<typename CoefType, typename ExpType, typename CenterType>
constexpr auto make_gaussian(CoefType&& c, ExpType&& a,
                             CenterType&& r0) noexcept {
    using clean_coef   = std::remove_reference_t<CoefType>;
    using clean_exp    = std::remove_reference_t<ExpType>;
    using clean_center = std::remove_reference_t<CenterType>;
    using traits =
      detail_::GaussianTraits_<clean_coef, clean_exp, clean_center>;

    return Gaussian_<traits>(std::forward<CoefType>(c),
                             std::forward<ExpType>(a),
                             std::forward<CenterType>(r0));
}

template<typename Traits1, typename Traits2>
constexpr auto operator==(const Gaussian_<Traits1>& lhs,
                          const Gaussian_<Traits2>& rhs) noexcept {
    return std::tie(lhs.coef(), lhs.exp(), lhs.center()[0], lhs.center()[1],
                    lhs.center()[2]) ==
           std::tie(rhs.coef(), rhs.exp(), rhs.center()[0], rhs.center()[1],
                    rhs.center()[2]);
}

template<typename Traits1, typename Traits2>
constexpr auto operator!=(const Gaussian_<Traits1>& lhs,
                          const Gaussian_<Traits2>& rhs) noexcept {
    return !(lhs == rhs);
}

//-------------------------------Implementations--------------------------------

template<typename Traits>
template<typename T, typename U, typename V>
constexpr Gaussian_<Traits>::Gaussian_(T&& c, U&& a, V&& r0) noexcept :
  m_coef_(std::forward<T>(c)),
  m_exp_(std::forward<U>(a)),
  m_center_(std::forward<V>(r0)) {}

} // namespace libchemist::basis_set
