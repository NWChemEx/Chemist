#include <LibChemist/BasisSetMap.hpp>
#include <LibChemist/LibChemist.hpp>
#include <catch2/catch.hpp>

using namespace LibChemist;
using range = typename std::pair<size_t, size_t>;


TEST_CASE("BasisSetMap Class") {
  auto mol = MoleculeManager().at("water");
  auto bs = apply_basis("sto-3g", mol);
  auto bsMap = BasisSetMap(bs);

  SECTION("Atom<->AO") {
    for(size_t i=0; i<mol.size(); ++i) {
      auto ao_range = bsMap.atom_to_ao(i);
      for(size_t j=ao_range.first; j<ao_range.second; ++j)
	REQUIRE(bsMap.ao_to_atom(j)==i);
    }
  }

  SECTION("Atom<->Shell") {
    for(size_t i=0; i<mol.size(); ++i) {
      auto shell_range = bsMap.atom_to_shell(i);
      for(size_t j=shell_range.first; j<shell_range.second; ++j)
	REQUIRE(bsMap.shell_to_atom(j)==i);      
  }

    SECTION("Shell<->AO") {
    for(size_t i=0; i<bs.nshells(); ++i) {
      auto ao_range = bsMap.shell_to_ao(i);
      for(size_t j=ao_range.first; j<ao_range.second; ++j)
	REQUIRE(bsMap.ao_to_shell(j)==i);            
    }
    }
  }
}
