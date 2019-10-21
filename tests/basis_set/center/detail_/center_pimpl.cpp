#include <catch2/catch.hpp>
#include <libchemist/basis_set/center/detail_/center_pimpl.hpp>
#include <libchemist/basis_set/shell/shell.hpp>

using namespace libchemist::detail_;

static inline auto make_center() {
    std::vector pures{ShellType::pure, ShellType::cartesian, ShellType::pure};
    std::vector<std::size_t> ls{0, 2, 4};
    std::vector v(6, std::vector<double>{1.0, 2.0, 3.0});
    std::vector coefs{v[0], v[2], v[4]};
    std::vector exps{v[1], v[3], v[5]};
    std::vector<ShellPIMPL<double>> shells{
      ShellPIMPL<double>(pures[0], ls[0], v[0], v[1]),
      ShellPIMPL<double>(pures[1], ls[1], v[2], v[3]),
      ShellPIMPL<double>(pures[2], ls[2], v[4], v[5])};
    CenterPIMPL<double> p(pures, ls, coefs, exps);
    return std::make_pair(p, shells);
}

TEST_CASE("CenterPIMPL : default ctor") {
    CenterPIMPL<double> p;
    REQUIRE(p.size() == 0);
}

TEST_CASE("CenterPIMPL : value ctor") {
    auto[p, shells] = make_center();
    REQUIRE(p.size() == shells.size());
    for(std::size_t i = 0; i < p.size(); ++i) {
        auto shell = p.at(i);
        REQUIRE(shell->purity() == shells[i].purity());
        REQUIRE(shell->l() == shells[i].l());
    }
}
