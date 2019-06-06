#include <catch2/catch.hpp>
#include <libchemist/basis_set/detail_/gaussian_basis_set_pimpl.hpp>

using namespace libchemist;
using namespace basis_set::detail_;

TEST_CASE("GaussianBasisSetPIMPL<double>()") {
    GaussianBasisSetPIMPL p;
    REQUIRE(p.ncenters() == 0);
    REQUIRE(p.nshells() == 0);
    REQUIRE(p.naos() == 0);
    REQUIRE(p.nunique_prims() == 0);

    SECTION("Throws if requested center is out of bounds") {
        REQUIRE_THROWS_AS(p.nshells(0), std::out_of_range);
    }
}

TEST_CASE("GaussianBasisSetPIMPL<double>(1c 0s)") {
    GaussianBasisSetPIMPL p;
    constexpr type::coord<double> r0{1.1, 2.2, 3.3};
    p.add_center(r0, 0);

    REQUIRE(p.ncenters() == 1);
    REQUIRE(p.nshells() == 0);
    REQUIRE(p.nshells(0) == 0);
    REQUIRE(p.naos() == 0);
    REQUIRE(p.nunique_prims() == 0);
    REQUIRE(p.center(0) == r0);

    SECTION("Throws if we try to add a shell") {
        std::vector<double> v;
        REQUIRE_THROWS_AS(p.add_shell(true, 0, v, v), std::runtime_error);
    }

    SECTION("Throws if requested shell is out of bounds") {
        REQUIRE_THROWS_AS(p.naos(0, 0), std::out_of_range);
    }
}

TEST_CASE("GaussianBasisSetPIMPL<double>(1c 1s 5ao 3p)") {
    GaussianBasisSetPIMPL p;
    constexpr type::coord<double> r0{1.1, 2.2, 3.3};
    p.add_center(r0, 1);

    SECTION("Throws if we try to add another center") {
        REQUIRE_THROWS_AS(p.add_center(r0, 1), std::runtime_error);
    }

    SECTION("Add the shell") {
        std::vector cs{4.4, 5.5, 6.6};
        std::vector as{7.7, 8.8, 9.9};
        p.add_shell(true, 2, cs, as);

        REQUIRE(p.nshells() == 1);
        REQUIRE(p.nshells(0) == 1);
        REQUIRE(p.naos() == 5);
        REQUIRE(p.naos(0) == 5);
        REQUIRE(p.naos(0, 0) == 5);
        REQUIRE(p.nunique_prims() == 3);
        REQUIRE(p.nunique_prims(0) == 3);
        REQUIRE(p.nunique_prims(0, 0) == 3);
        REQUIRE(p.nunique_prims(0, 0, 0) == 3);
        REQUIRE(p.coefficient(0, 0, 0, 0) == 4.4);
        REQUIRE(p.coefficient(0, 0, 0, 1) == 5.5);
        REQUIRE(p.coefficient(0, 0, 0, 2) == 6.6);
        REQUIRE(p.exponent(0, 0, 0, 0) == 7.7);
        REQUIRE(p.exponent(0, 0, 0, 1) == 8.8);
        REQUIRE(p.exponent(0, 0, 0, 2) == 9.9);

        SECTION("Throws if we try to add another shell") {
            REQUIRE_THROWS_AS(p.add_shell(true, 2, cs, as), std::runtime_error);
        }
        SECTION("Throws if we request a bad ao") {
            REQUIRE_THROWS_AS(p.nunique_prims(0, 0, 6), std::out_of_range);
        }
        SECTION("Throws if we request a bad prim") {
            REQUIRE_THROWS_AS(p.coefficient(0, 0, 0, 3), std::out_of_range);
        }
    }
}

TEST_CASE("GaussianBasisSetPIMPL<double>(2c 4s 15ao 10p)") {
    GaussianBasisSetPIMPL p;
    constexpr type::coord<double> r0{1.1, 2.2, 3.3};
    std::vector cs0{4.4, 5.5, 6.6};
    std::vector cs1{1.1, 2.2};
    std::vector as0{7.7, 8.8, 9.9};
    std::vector as1{3.3, 4.4};
    p.add_center(r0, 2);
    p.add_shell(true, 2, cs0, as0);
    p.add_shell(true, 0, cs1, as1);
    p.add_center(r0, 2);
    p.add_shell(false, 2, cs1, as1);
    p.add_shell(true, 1, cs0, as0);

    REQUIRE(p.nshells() == 4);
    REQUIRE(p.nshells(0) == 2);
    REQUIRE(p.nshells(1) == 2);

    REQUIRE(p.naos() == 15);
    REQUIRE(p.naos(0) == 6);
    REQUIRE(p.naos(1) == 9);
    REQUIRE(p.naos(0, 0) == 5);
    REQUIRE(p.naos(0, 1) == 1);
    REQUIRE(p.naos(1, 0) == 6);
    REQUIRE(p.naos(1, 1) == 3);

    REQUIRE(p.nunique_prims() == 10);
    REQUIRE(p.nunique_prims(0) == 5);
    REQUIRE(p.nunique_prims(1) == 5);
    REQUIRE(p.nunique_prims(0, 0) == 3);
    REQUIRE(p.nunique_prims(0, 1) == 2);
    REQUIRE(p.nunique_prims(1, 0) == 2);
    REQUIRE(p.nunique_prims(1, 1) == 3);

    for(std::size_t i = 0; i < 5; ++i) REQUIRE(p.nunique_prims(0, 0, i) == 3);

    REQUIRE(p.nunique_prims(0, 1, 0) == 2);

    for(std::size_t i = 0; i < 6; ++i) REQUIRE(p.nunique_prims(1, 0, i) == 2);

    for(std::size_t i = 0; i < 3; ++i) REQUIRE(p.nunique_prims(1, 1, i) == 3);

    for(std::size_t i = 0; i < 5; ++i) {
        REQUIRE(p.coefficient(0, 0, i, 0) == 4.4);
        REQUIRE(p.coefficient(0, 0, i, 1) == 5.5);
        REQUIRE(p.coefficient(0, 0, i, 2) == 6.6);
        REQUIRE(p.exponent(0, 0, i, 0) == 7.7);
        REQUIRE(p.exponent(0, 0, i, 1) == 8.8);
        REQUIRE(p.exponent(0, 0, i, 2) == 9.9);
    }

    REQUIRE(p.coefficient(0, 1, 0, 0) == 1.1);
    REQUIRE(p.coefficient(0, 1, 0, 1) == 2.2);
    REQUIRE(p.exponent(0, 1, 0, 0) == 3.3);
    REQUIRE(p.exponent(0, 1, 0, 1) == 4.4);

    for(std::size_t i = 0; i < 6; ++i) {
        REQUIRE(p.coefficient(1, 0, i, 0) == 1.1);
        REQUIRE(p.coefficient(1, 0, i, 1) == 2.2);
        REQUIRE(p.exponent(1, 0, i, 0) == 3.3);
        REQUIRE(p.exponent(1, 0, i, 1) == 4.4);
    }

    for(std::size_t i = 0; i < 3; ++i) {
        REQUIRE(p.coefficient(1, 1, i, 0) == 4.4);
        REQUIRE(p.coefficient(1, 1, i, 1) == 5.5);
        REQUIRE(p.coefficient(1, 1, i, 2) == 6.6);
        REQUIRE(p.exponent(1, 1, i, 0) == 7.7);
        REQUIRE(p.exponent(1, 1, i, 1) == 8.8);
        REQUIRE(p.exponent(1, 1, i, 2) == 9.9);
    }
}
