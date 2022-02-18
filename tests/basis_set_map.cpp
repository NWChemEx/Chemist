#include "chemist/basis_set_map.hpp"
#include "chemist/chemist.hpp"
#include <catch2/catch.hpp>
#include <utilities/iter_tools/enumerate.hpp>

#include <iostream>

/*
using namespace chemist;
using range     = typename std::pair<size_t, size_t>;
using size_type = std::size_t;

static const std::vector<size_type> s2a_corr{0, 0, 0, 1, 2};
static const std::vector<size_type> ao2s_corr{0, 1, 2, 2, 2, 3, 4};
static const std::vector<size_type> ao2a_corr{0, 0, 0, 0, 0, 1, 2};
static const std::vector<range> a2s_corr{
  std::make_pair(0, 3), std::make_pair(3, 4), std::make_pair(4, 5)};
static const std::vector<range> s2ao_corr{
  std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 5),
  std::make_pair(5, 6), std::make_pair(6, 7)};
static const std::vector<range> a2ao_corr{
  std::make_pair(0, 5), std::make_pair(5, 6), std::make_pair(6, 7)};

void compare_maps(const size_t nAtoms, BasisSetMap<double>& lhs,
                  BasisSetMap<double>& rhs) {
    for(size_t i = 0; i < nAtoms; ++i) {
        auto lhs_shell_range = lhs.atom_to_shell(i);
        REQUIRE(lhs_shell_range == rhs.atom_to_shell(i));
        for(size_t j = lhs_shell_range.first; j < lhs_shell_range.second; ++j)
            REQUIRE(lhs.shell_to_ao(j) == rhs.shell_to_ao(j));
    }
}

TEST_CASE("BasisSetMap<double> Class") {
    auto mol   = MoleculeManager().at("water");
    auto bsm   = BasisSetManager();
    load_sto_three_g(bsm);
    auto aos   = apply_basis("sto-3g", mol, bsm);
    auto bs    = aos.basis_set();
    auto bsMap = BasisSetMap<double>(bs);

    SECTION("Atom<->AO") {
        for(size_t i = 0; i < bs.size(); ++i) {
            REQUIRE(bsMap.atom_to_ao(i) == a2ao_corr.at(i));
        }
        for(size_t i = 0; i < bs.n_aos(); ++i) {
            REQUIRE(bsMap.ao_to_atom(i) == ao2a_corr.at(i));
        }
    }

    SECTION("Atom<->Shell") {
        for(size_t i = 0; i < bs.size(); ++i) {
            REQUIRE(bsMap.atom_to_shell(i) == a2s_corr.at(i));
        }
        for(size_t i = 0; i < bs.n_shells(); ++i) {
            REQUIRE(bsMap.shell_to_atom(i) == s2a_corr.at(i));
        }
    }

    SECTION("Shell<->AO") {
        for(size_t i = 0; i < bs.n_shells(); ++i) {
            REQUIRE(bsMap.shell_to_ao(i) == s2ao_corr.at(i));
        }
        for(size_t i = 0; i < bs.n_aos(); ++i) {
            REQUIRE(bsMap.ao_to_shell(i) == ao2s_corr.at(i));
        }
    }

    SECTION("Copy Ctor") {
        auto bsMap2 = BasisSetMap<double>(bsMap);
        compare_maps(mol.size(), bsMap, bsMap2);
    }

    SECTION("Move Ctor") {
        auto bsMap2(std::move(bsMap));
        compare_maps(mol.size(), bsMap, bsMap2);
    }

    SECTION("Copy Assignment") {
        BasisSetMap<double> bsMap2;
        auto& pbsm = (bsMap2 = bsMap);
        compare_maps(mol.size(), bsMap, bsMap2);
        REQUIRE(&pbsm == &bsMap2);
    }

    SECTION("Move Assignment") {
        BasisSetMap<double> bsMap2;
        auto& pbsm = (bsMap2 = std::move(bsMap));
        compare_maps(mol.size(), bsMap, bsMap2);
        REQUIRE(&pbsm == &bsMap2);
    }
}*/