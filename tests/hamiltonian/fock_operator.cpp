#include "libchemist/hamiltonian/fock_operator.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;



TEST_CASE("FockOperator Class") {
  SECTION("Default CTor") {
    FockOperator ham;
    CHECK( ham.get_terms<ElectronKinetic>().size() == 0 );
  }

  SECTION("One Operator Construction") {
    FockOperator ham( ElectronKinetic{} );
    CHECK( ham.has_term<ElectronKinetic>() );
    CHECK( ham.get_terms<ElectronKinetic>().size() == 1 );
  }

  SECTION("Two Operator Construction") {
    FockOperator ham( ElectronKinetic{}, ElectronNuclearCoulomb{} );
    CHECK( ham.get_terms<ElectronKinetic>().size()        == 1 );
    CHECK( ham.get_terms<ElectronNuclearCoulomb>().size() == 1 );
  }

  SECTION("Single Term Add") {
    FockOperator ham; ham.add_term(ElectronKinetic{});
    CHECK( ham.get_terms<ElectronKinetic>().size() == 1 );
  }

  SECTION("Term Add Chain") {
    FockOperator ham; 
    ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearCoulomb{});
    CHECK( ham.get_terms<ElectronKinetic>().size()        == 1 );
    CHECK( ham.get_terms<ElectronNuclearCoulomb>().size() == 1 );
  }
}

