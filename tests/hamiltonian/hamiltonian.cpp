#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;

TEST_CASE("Hamiltonian Class") {
  Hamiltonian ham;
  SECTION("Default CTor") {
    CHECK( ham.terms<1>().size() == 0 );
    CHECK( ham.terms<2>().size() == 0 );
  }

  SECTION("One Operator Construction") {
    ham.add_term( ElectronKinetic{} );
    CHECK( ham.terms<1>().size() == 1 );
    CHECK( ham.terms<2>().size() == 0 );
  }

  SECTION("Two Operator Construction") {
    ham.add_term( ElectronKinetic{} ).add_term( ElectronNuclearCoulomb{} );
    CHECK( ham.terms<1>().size() == 2 );
    CHECK( ham.terms<2>().size() == 0 );
  }

  SECTION("Many Body Hamiltonian") {
    ham.add_term( ElectronKinetic{} )
       .add_term( ElectronNuclearCoulomb{} )
       .add_term( ElectronElectronCoulomb{} );
    CHECK( ham.terms<1>().size() == 2 );
    CHECK( ham.terms<2>().size() == 1 );
  }
}
