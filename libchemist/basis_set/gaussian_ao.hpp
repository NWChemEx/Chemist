#pragma once
#include "libchemist/basis_set/gaussian_view.hpp"
#include <tuple>

namespace libchemist::basis_set {

template<typename T, typename Container = std::vector<T>>
class GaussianAO {
public:
    using index_type = std::array<type::size, 1>;

    template<typename CoefType, typename ExpType, typename CoordType>
    constexpr GaussianAO(std::initializer_list<CoefType> c,
                         std::initializer_list<ExpType> a, CoordType&& r) :
      GaussianAO(c.begin(), c.end(), a.begin(), a.end(),
                 std::forward<CoordType>(r)) {}

    template<typename CoefCont, typename ExpCont, typename CoordType>
    constexpr GaussianAO(CoefCont&& cs, ExpCont&& as, CoordType&& r) :
      m_coefs_(std::forward<CoefCont>(cs)),
      m_alpha_(std::forward<ExpCont>(as)),
      m_coord_(std::forward<CoordType>(r)) {}

    constexpr auto primitive(type::size i) noexcept {
        return GaussianView{this, i};
    }
    constexpr auto primitive(type::size i) const noexcept {
        return GaussianView{this, i};
    }

    constexpr auto nprims() const noexcept { return m_coefs_.size(); }

    constexpr auto& coefficient(type::size i) noexcept { return m_coefs_[i]; }
    constexpr auto& coefficient(type::size i) const noexcept {
        return m_coefs_[i];
    }

    constexpr auto& exponent(type::size i) noexcept { return m_alpha_[i]; }
    constexpr auto& exponent(type::size i) const noexcept {
        return m_alpha_[i];
    }
    constexpr auto& center(type::size) const { return m_coord_; }

    constexpr auto& center() { return m_coord_; }
    constexpr auto& center() const { return m_coord_; }

    template<typename U, typename V>
    constexpr bool operator==(const GaussianAO<U, V>& rhs) const noexcept {
        return detail_::compare_ao(*this, rhs);
    }

    template<typename U, typename V>
    constexpr bool operator!=(const GaussianAO<U, V>& rhs) const noexcept {
        return !((*this) == rhs);
    }

private:
    Container m_coefs_;
    Container m_alpha_;
    type::coord<T> m_coord_;
};

template<std::size_t NPrims, typename T = double>
using StaticGaussianAO = GaussianAO<T, std::array<T, NPrims>>;

} // namespace libchemist::basis_set
