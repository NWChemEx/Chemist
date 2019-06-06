#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_alias.hpp>

using namespace libchemist::basis_set;

struct OneIdx {
    using value_type   = double;
    constexpr OneIdx() = default;
    constexpr auto primitive(std::size_t) const {
        return GaussianAlias{this, std::array{0ul}};
    }
    constexpr auto& coefficient(std::size_t) const noexcept { return m_coef; }
    constexpr auto& exponent(std::size_t) const noexcept { return m_exp; }
    constexpr auto& center(std::size_t) const noexcept { return m_center; }
    constexpr bool has_center(std::size_t) const noexcept { return true; }

    double m_coef = 1.23;
    double m_exp  = 4.56;
    std::array<double, 3> m_center{1.1, 2.2, 3.3};
};

TEST_CASE("constexpr GaussianAlias<OneIdx>") {
    constexpr OneIdx parent;
    STATIC_REQUIRE(&parent.primitive(0ul).coefficient() ==
                   &parent.coefficient(0));
    STATIC_REQUIRE(&parent.primitive(0ul).exponent() == &parent.exponent(0));
    STATIC_REQUIRE(&parent.primitive(0ul).center() == &parent.center(0));
    STATIC_REQUIRE(parent.primitive(0ul).has_center());
}
