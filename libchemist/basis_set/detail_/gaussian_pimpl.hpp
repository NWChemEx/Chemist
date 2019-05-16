#pragma once
#include "libchemist/basis_set/detail_/gaussian_traits.hpp"
#include <utility>

namespace libchemist::basis_set::detail_ {

template<typename Traits>
class GaussianPIMPL {
public:
    using exp_type    = typename Traits::exp_type;
    using coef_type   = typename Traits::coef_type;
    using center_type = typename Traits::center_type;

    constexpr GaussianPIMPL(coef_type c, exp_type a, center_type r0) noexcept;

    constexpr auto& center() noexcept { return m_center_; }
    constexpr const auto& center() const noexcept { return m_center_; }

    constexpr auto& exp() noexcept { return m_exp_; }
    constexpr const auto& exp() const noexcept { return m_exp_; }
    constexpr auto& coef() noexcept { return m_coef_; }
    constexpr const auto& coef() const noexcept { return m_coef_; }

private:
    center_type m_center_;
    exp_type m_exp_;
    coef_type m_coef_;
};

template<typename CoefType, typename ExpType, typename CenterType>
constexpr auto make_guassian_pimpl(CoefType&& c, ExpType&& a,
                                   CenterType&& r0) noexcept {
    using clean_coef   = std::decay_t<CoefType>;
    using clean_exp    = std::decay_t<ExpType>;
    using clean_center = std::decay_t<CenterType>;
    using traits = detail_::GaussianTraits<clean_coef, clean_exp, clean_center>;
    using pimpl_type = detail_::GaussianPIMPL<traits>;
    return pimpl_type(std::forward<CoefType>(c), std::forward<ExpType>(a),
                      std::forward<CenterType>(r0));
}

template<typename Traits>
constexpr GaussianPIMPL<Traits>::GaussianPIMPL(
  typename Traits::coef_type c, typename Traits::exp_type a,
  typename Traits::center_type r0) noexcept :
  m_center_(std::move(r0)),
  m_exp_(std::move(a)),
  m_coef_(std::move(c)) {}

// template<typename Traits>
// class GaussianAOPIMPL {
// public:
//    using center_type = typename Traits::center_type;
//    const auto& center() const { return m_center_; }
//    const auto& exp(type::size i) const { return m_exps_[i]; }
//    const auto& coef(type::size i) const { return m_coefs_[i]; }
// private:
//    exp_container m_exps_;
//    coef_container m_coefs_;
//    center_type m_center_;
//};
//
// template<typename ParentType>
// class GaussianAOAliasPIMPL {
//    const auto& center() const { return m_parent_.center(); }
//    const auto& exp(type::size i) const {return m_parent_[m_me_].exp(i); }
//    const auto& coef(type::size i) const { return m_parent_[m_me_].coef(i); }
// private:
//    type::size m_me_;
//    ParentType m_parent_;
//};
//
// template<typename Traits>
// class AOShellPIMPL {
// public:
//    const auto& pure() const { return m_pure_; }
//    const auto&
// private:
//    center_type m_center_;
//    coef_container m_coefs_;
//    exp_container m_exps_;
//    am_type m_l_;
//    pure_type m_pure_;
//};
//
// template<typename ParentType>
// class AOShellAliasPIMPL {
//    type::size m_me_;
//    ParentType m_parent_;
//};

} // namespace libchemist::basis_set::detail_
