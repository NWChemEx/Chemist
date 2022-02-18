#include "chemist/basis_set/ao_basis_set/ao_basis_set_pimpl.hpp"
#include <catch2/catch.hpp>

using pimpl_t  = chemist::detail_::AOBasisSetPIMPL<double>;
using center_t = chemist::AtomicBasisSet<double>;

TEST_CASE("AOBasisSetPIMPL : default ctor") {
    pimpl_t p{};
    REQUIRE(p.size() == 0);
}

TEST_CASE("AOBasisSetPIMPL : size") {
    pimpl_t p;
    p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
    REQUIRE(p.size() == 1);
}

TEST_CASE("AOBasisSetPIMPL : add_center") {
    pimpl_t p;
    p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
    REQUIRE(p.at(0) == center_t("", 0, 1.0, 2.0, 3.0));
}

TEST_CASE("AOBasisSetPIMPL : at") {
    pimpl_t p;
    p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
    REQUIRE(p.at(0) == center_t("", 0, 1.0, 2.0, 3.0));
}

TEST_CASE("AOBasisSetPIMPL : at() const") {
    pimpl_t p;
    p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
    REQUIRE(std::as_const(p).at(0) == center_t("", 0, 1.0, 2.0, 3.0));
}
