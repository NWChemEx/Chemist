#pragma once

namespace libchemist::basis_set {

template<typename T, typename Container = std::vector<T>>
class GaussianShell {
public:
    using index_type = std::array<type::size, 2>;

    constexpr auto primitive(type::size i, type::size j) noexcept {
        return GaussianView{this, std::array{i, j}};
    }

    constexpr auto primitive(type::size i, type::size j) const noexcept {
        return GaussianView{this, std::array{i, j}};
    }

    constexpr auto ao(type::size i) noexcept {
        return GaussianAOView{this, std::array{i}};
    }

    constexpr auto ao(type::size i) const noexcept {
        return GaussianAOView{this, std::array{i}};
    }

    constexpr auto nprims(type::size) const noexcept { return m_coefs_.size(); }

    constexpr auto nao() const noexcept { m_is_pure_ ? 2 * m_l_ + 1 : 0; }

    constexpr auto& coefficient(type::size, type::size j) const noexcept {
        return m_coefs_[j];
    }
    constexpr auto& coefficient(type::size i) noexcept { return m_coefs_[i]; }
    constexpr auto& coefficient(type::size i) const noexcept {
        return m_coefs_[i];
    }

    constexpr auto& exponent(type::size, type::size j) const noexcept {
        return m_alpha_[j];
    }
    constexpr auto& exponent(type::size i) noexcept { return m_alpha_[i]; }
    constexpr auto& exponent(type::size i) const noexcept {
        return m_alpha_[i];
    }

    constexpr auto& center(type::size, type::size) const noexcept {
        return m_coord_;
    }
    constexpr auto& center(type::size) const noexcept { return m_coord_; }
    constexpr auto& center() noexcept { return m_coord_; }
    constexpr auto& center() const noexcept { return m_coord_; }

private:
    Container m_coefs_;
    Container m_alpha_;
    type::angular_momentum m_l_;
    bool m_is_pure_;
};

} // namespace libchemist::basis_set
