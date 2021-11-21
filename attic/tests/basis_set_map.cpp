#include <catch2/catch.hpp>
#include <chemist/basis_set_map.hpp>
#include <chemist/chemist.hpp>

using namespace chemist;
using range     = typename std::pair<size_t, size_t>;
using size_type = std::size_t;

void compare_maps(const size_t nAtoms, BasisSetMap& lhs, BasisSetMap& rhs) {
    for(size_t i = 0; i < nAtoms; ++i) {
        auto lhs_shell_range = lhs.atom_to_shell(i);
        REQUIRE(lhs_shell_range == rhs.atom_to_shell(i));
        for(size_t j = lhs_shell_range.first; j < lhs_shell_range.second; ++j)
            REQUIRE(lhs.shell_to_ao(j) == rhs.shell_to_ao(j));
    }
}

TEST_CASE("BasisSetMap Class") {
    auto mol   = MoleculeManager().at("water");
    auto bs    = apply_basis("sto-3g", mol);
    auto bsMap = BasisSetMap(bs);

    SECTION("Atom<->AO") {
        for(size_t i = 0; i < mol.size(); ++i) {
            auto ao_range = bsMap.atom_to_ao(i);
            for(size_t j = ao_range.first; j < ao_range.second; ++j)
                REQUIRE(bsMap.ao_to_atom(j) == i);
        }
    }

    SECTION("Atom<->Shell") {
        for(size_t i = 0; i < mol.size(); ++i) {
            auto shell_range = bsMap.atom_to_shell(i);
            for(size_t j = shell_range.first; j < shell_range.second; ++j)
                REQUIRE(bsMap.shell_to_atom(j) == i);
        }

        SECTION("Shell<->AO") {
            for(size_t i = 0; i < bs.nshells(); ++i) {
                auto ao_range = bsMap.shell_to_ao(i);
                for(size_t j = ao_range.first; j < ao_range.second; ++j)
                    REQUIRE(bsMap.ao_to_shell(j) == i);
            }
        }

        SECTION("Copy Ctor") {
            auto bsMap2 = BasisSetMap(bsMap);
            compare_maps(mol.size(), bsMap, bsMap2);
        }

        SECTION("Move Ctor") {
            auto bsMap2(std::move(bsMap));
            compare_maps(mol.size(), bsMap, bsMap2);
        }

        SECTION("Copy Assignment") {
            BasisSetMap bsMap2;
            auto& pbsm = (bsMap2 = bsMap);
            compare_maps(mol.size(), bsMap, bsMap2);
            REQUIRE(&pbsm == &bsMap2);
        }

        SECTION("Move Assignment") {
            BasisSetMap bsMap2;
            auto& pbsm = (bsMap2 = std::move(bsMap));
            compare_maps(mol.size(), bsMap, bsMap2);
            REQUIRE(&pbsm == &bsMap2);
        }
    }
}
