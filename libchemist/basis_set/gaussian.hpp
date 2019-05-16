#pragma once
#include "libchemist/basis_set/detail_/gaussian_alias_pimpl.hpp"
#include "libchemist/basis_set/detail_/gaussian_pimpl.hpp"
#include <tuple>

namespace libchemist::basis_set {

template<typename PIMPLType>
class Gaussian {
public:
    using coef_type   = typename PIMPLType::coef_type;
    using exp_type    = typename PIMPLType::exp_type;
    using center_type = typename PIMPLType::center_type;

    explicit constexpr Gaussian(PIMPLType pimpl) noexcept;

    constexpr auto& coef() noexcept { return m_pimpl_.coef(); }
    constexpr const auto& coef() const noexcept { return m_pimpl_.coef(); }

    constexpr double& exp() noexcept { return m_pimpl_.exp(); }
    constexpr const auto& exp() const noexcept { return m_pimpl_.exp(); }

    constexpr auto& center() noexcept { return m_pimpl_.center(); }
    constexpr const auto& center() const noexcept { return m_pimpl_.center(); }

private:
    PIMPLType m_pimpl_;
};

template<typename CoefType, typename ExpType, typename CenterType>
constexpr auto make_gaussian(CoefType&& c, ExpType&& a,
                             CenterType&& r0) noexcept {
    return Gaussian(detail_::make_guassian_pimpl(std::forward<CoefType>(c),
                                                 std::forward<ExpType>(a),
                                                 std::forward<CenterType>(r0)));
}

template<typename Traits, typename ParentType>
constexpr auto make_gaussian_alias(type::size me,
                                   ParentType&& parent) noexcept {
    return Gaussian{detail_::make_gaussian_alias_pimpl<Traits>(
      me, std::forward<ParentType>(parent))};
}

template<typename Traits1, typename Traits2>
constexpr bool operator==(const Gaussian<Traits1>& lhs,
                          const Gaussian<Traits2>& rhs) noexcept {
    return std::tie(lhs.center()[0], lhs.center()[1], lhs.center()[2],
                    lhs.exp(), lhs.coef()) ==
           std::tie(rhs.center()[0], rhs.center()[1], rhs.center()[2],
                    rhs.exp(), rhs.coef());
}

template<typename Traits1, typename Traits2>
constexpr bool operator!=(const Gaussian<Traits1>& lhs,
                          const Gaussian<Traits2>& rhs) noexcept {
    return !(lhs == rhs);
}

template<typename PIMPLType>
constexpr Gaussian<PIMPLType>::Gaussian(PIMPLType pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

} // namespace libchemist::basis_set
