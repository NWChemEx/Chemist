#pragma once
#include "libchemist/basis_set/detail_/gaussian_ao_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include "libchemist/basis_set/gaussian.hpp"
#include <tuple>

namespace libchemist::basis_set {

template<typename Traits>
class GaussianAO_ {
public:
    template<typename T, typename U, typename V>
    constexpr GaussianAO_(T&& c, U&& a, V&& r0) noexcept;

    constexpr auto size() const noexcept { return (*m_cs_).size(); }

    constexpr auto& center() noexcept { return *m_center_; }
    constexpr const auto& center() const noexcept { return *m_center_; }

    constexpr auto& coef(type::size i) noexcept { return (*m_cs_)[i]; }
    constexpr const auto& coef(type::size i) const noexcept {
        return (*m_cs_)[i];
    }

    constexpr auto& exp(type::size i) noexcept { return (*m_as_)[i]; }
    constexpr const auto& exp(type::size i) const noexcept {
        return (*m_as_)[i];
    }

    auto operator[](type::size i) noexcept {
        const auto* ptr = &center();
        return make_gaussian(&coef(i), &exp(i), ptr);
    }

    constexpr auto operator[](type::size i) const noexcept {
        return make_gaussian(coef(i), exp(i), center());
    }

private:
    detail_::holder<typename Traits::coef_container> m_cs_;
    detail_::holder<typename Traits::exp_container> m_as_;
    detail_::holder<typename Traits::center_type> m_center_;
};

template<typename T>
using GaussianAO = GaussianAO_<detail_::GaussianAOTraits<T>>;

template<typename T, type::size N>
using StaticGaussianAO = GaussianAO_<detail_::StaticGaussianAOTraits<T, N>>;

template<typename CoefArray, typename ExpArray, typename CenterType>
constexpr auto make_gaussian_ao(CoefArray&& cs, ExpArray&& as,
                                CenterType&& r0) noexcept {
    using clean_coefs  = std::remove_reference_t<CoefArray>;
    using clean_exps   = std::remove_reference_t<ExpArray>;
    using clean_center = std::remove_reference_t<CenterType>;
    using traits =
      detail_::GaussianAOTraits_<clean_coefs, clean_exps, clean_center>;
    return GaussianAO_<traits>(std::forward<CoefArray>(cs),
                               std::forward<ExpArray>(as),
                               std::forward<CenterType>(r0));
}

#define AO_CLASS_TYPE GaussianAO_<Traits>

template<typename Traits>
template<typename T, typename U, typename V>
constexpr AO_CLASS_TYPE::GaussianAO_(T&& c, U&& a, V&& r0) noexcept :
  m_cs_(std::forward<T>(c)),
  m_as_(std::forward<U>(a)),
  m_center_(std::forward<V>(r0)) {}

#undef AO_CLASS_TYPE
} // namespace libchemist::basis_set
