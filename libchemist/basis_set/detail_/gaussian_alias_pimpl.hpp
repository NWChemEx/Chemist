#pragma once
#include "libchemist/types.hpp"

namespace libchemist::basis_set::detail_ {

template<typename Traits, typename ParentType>
class GaussianAliasPIMPL {
public:
    using coef_type   = typename Traits::coef_type;
    using exp_type    = typename Traits::exp_type;
    using center_type = typename Traits::center_type;

    constexpr GaussianAliasPIMPL(type::size me, ParentType parent) noexcept;

    constexpr coef_type& coef() noexcept { return m_parent_->coef(m_me_); }
    constexpr const coef_type& coef() const noexcept {
        return m_parent_->coef(m_me_);
    }

    constexpr exp_type& exp() noexcept { return m_parent_->exp(m_me_); }
    constexpr const exp_type& exp() const noexcept {
        return m_parent_->exp(m_me_);
    }

    constexpr center_type& center() noexcept { return m_parent_->center(); }
    constexpr const center_type& center() const noexcept {
        return m_parent_->center();
    }

private:
    type::size m_me_;
    ParentType m_parent_;
};

template<typename Traits, typename ParentType>
constexpr auto make_gaussian_alias_pimpl(type::size me,
                                         ParentType&& parent) noexcept {
    using clean_parent = std::remove_reference_t<ParentType>;
    return GaussianAliasPIMPL<Traits, clean_parent>{
      me, std::forward<ParentType>(parent)};
}

template<typename Traits, typename ParentType>
constexpr GaussianAliasPIMPL<Traits, ParentType>::GaussianAliasPIMPL(
  type::size me, ParentType parent) noexcept :
  m_me_(me),
  m_parent_(std::move(parent)) {}

} // namespace libchemist::basis_set::detail_
