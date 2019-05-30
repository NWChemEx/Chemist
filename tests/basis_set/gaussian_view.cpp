#include <catch2/catch.hpp>
#include <libchemist/basis_set/gaussian_view.hpp>

using namespace libchemist::basis_set;

struct OneIdx {
    using index_type = std::array<std::size_t, 1>;
    auto make_view() { return GaussianView{this, 0ul}; }
    auto& coefficient(std::size_t) const noexcept { return m_coef; }
    auto& exponent(std::size_t) const noexcept { return m_exp; }
    auto& center(std::size_t) const noexcept { return m_center; }
    double m_coef = 1.1;
    double m_exp  = 2.2;
    std::array<double, 3> m_center{1.23, 2.34, 3.45};
};

TEST_CASE("GaussianView<OneIdx>") {
    OneIdx parent;
    auto g = parent.make_view();
    REQUIRE(&g.coefficient() == &parent.coefficient(0));
    REQUIRE(&g.exponent() == &parent.exponent(0));
    REQUIRE(&g.center() == &parent.center(0));
}
