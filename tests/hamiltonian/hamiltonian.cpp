#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;

TEST_CASE("Hamiltonian Class") {
  SECTION("Default CTor") {
    Hamiltonian ham;
    CHECK( ham.get_terms<ElectronKinetic>().size() == 0 );
  }

  SECTION("One Operator Construction") {
    Hamiltonian ham( ElectronKinetic{} );
    CHECK( ham.get_terms<ElectronKinetic>().size() == 1 );
  }

  SECTION("Two Operator Construction") {
    Hamiltonian ham( ElectronKinetic{}, ElectronNuclearCoulomb{} );
    CHECK( ham.get_terms<ElectronKinetic>().size()        == 1 );
    CHECK( ham.get_terms<ElectronNuclearCoulomb>().size() == 1 );
  }

  SECTION("Single Term Add") {
    Hamiltonian ham; ham.add_term(ElectronKinetic{});
    CHECK( ham.get_terms<ElectronKinetic>().size() == 1 );
  }

  SECTION("Term Add Chain") {
    Hamiltonian ham; 
    ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearCoulomb{});
    CHECK( ham.get_terms<ElectronKinetic>().size()        == 1 );
    CHECK( ham.get_terms<ElectronNuclearCoulomb>().size() == 1 );
  }
}
