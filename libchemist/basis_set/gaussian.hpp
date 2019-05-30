#pragma once
#include "libchemist/basis_set/detail_/utility.hpp"
#include "libchemist/types.hpp"

namespace libchemist::basis_set {

/**
 *
 * @tparam T
 */
template<typename T>
class Gaussian {
public:
    template<typename CoefType, typename ExpType, typename CoordType>
    constexpr Gaussian(CoefType&& c, ExpType&& a, CoordType&& r) noexcept :
      m_coefs_(std::forward<CoefType>(c)),
      m_alpha_(std::forward<ExpType>(a)),
      m_coord_(std::forward<CoordType>(r)) {}

    constexpr auto& coefficient() noexcept { return m_coefs_; }
    constexpr const auto& coefficient() const noexcept { return m_coefs_; }

    constexpr auto& exponent() noexcept { return m_alpha_; }
    constexpr const auto& exponent() const noexcept { return m_alpha_; }

    constexpr auto& center() noexcept { return m_coord_; }
    constexpr const auto& center() const noexcept { return m_coord_; }

    template<typename U>
    constexpr bool operator==(const Gaussian<U>& rhs) const noexcept {
        return detail_::compare_gaussians(*this, rhs);
    }

    template<typename U>
    constexpr bool operator!=(const Gaussian<U>& rhs) const noexcept {
        return !((*this) == rhs);
    }

private:
    T m_coefs_;
    T m_alpha_;
    type::coord<T> m_coord_;
};

} // namespace libchemist::basis_set
