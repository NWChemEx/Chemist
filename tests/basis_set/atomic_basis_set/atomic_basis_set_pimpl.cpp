#include "chemist/basis_set/atomic_basis_set/atomic_basis_set_pimpl.hpp"
#include "chemist/basis_set/shell.hpp"
#include <catch2/catch.hpp>

using chemist::ShellType;
using namespace chemist::detail_;

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
    AtomicBasisSetPIMPL<double> p("custom", 1000, pures, ls, coefs, exps);
    return std::make_pair(p, shells);
}

TEST_CASE("AtomicBasisSetPIMPL : defaulte ctor") {
    AtomicBasisSetPIMPL<double> p;
    REQUIRE(p.size() == 0);
    REQUIRE(p.basis_set_name() == "");
    REQUIRE(p.atomic_number() == 0);
}

TEST_CASE("AtomicBasisSetPIMPL : name and atomic number ctor") {
    AtomicBasisSetPIMPL<double> p("custom", 1000);
    REQUIRE(p.size() == 0);
    REQUIRE(p.basis_set_name() == "custom");
    REQUIRE(p.atomic_number() == 1000);
}

TEST_CASE("AtomicBasisSetPIMPL : value ctor") {
    auto [p, shells] = make_center();
    REQUIRE(p.size() == shells.size());
    REQUIRE(p.basis_set_name() == "custom");
    REQUIRE(p.atomic_number() == 1000);
    for(std::size_t i = 0; i < p.size(); ++i) {
        auto shell = p.at(i);
        REQUIRE(shell->purity() == shells[i].purity());
        REQUIRE(shell->l() == shells[i].l());
    }
}

TEST_CASE("AtomicBasisSetPIMPL : copy ctor") {
    auto [p, shells] = make_center();
    AtomicBasisSetPIMPL<double> c2(p);
    REQUIRE(c2.size() == shells.size());
    REQUIRE(c2.basis_set_name() == "custom");
    REQUIRE(c2.atomic_number() == 1000);
    for(std::size_t i = 0; i < p.size(); ++i) {
        auto shell = c2.at(i);
        REQUIRE(shell->purity() == shells[i].purity());
        REQUIRE(shell->l() == shells[i].l());
    }
}

TEST_CASE("AtomicBasisSetPIMPL : add_shell") {
    AtomicBasisSetPIMPL<double> c;
    c.add_shell(ShellType::pure, 0, std::vector<double>{},
                std::vector<double>{});
    REQUIRE(c.size() == 1);
    REQUIRE(c.at(0)->purity() == ShellType::pure);
    REQUIRE(c.at(0)->l() == 0);
}

TEST_CASE("AtomicBasisSetPIMPL : size") {
    auto [p, shells] = make_center();
    REQUIRE(p.size() == shells.size());
}

TEST_CASE("AtomicBasisSetPIMPL : at") {
    auto [p, shells] = make_center();
    for(std::size_t i = 0; i < shells.size(); ++i) {
        REQUIRE(p.at(i)->purity() == shells[i].purity());
        REQUIRE(p.at(i)->l() == shells[i].l());
    }
}
