#include "libchemist/hamiltonian/derived_operator.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;



TEST_CASE("DerivedOperator Class") {
  SECTION("Default CTor") {
    DerivedOperator ham;
    CHECK( ham.get_terms<ElectronKinetic>().size() == 0 );
  }

  SECTION("One Operator Construction") {
    DerivedOperator ham( ElectronKinetic{} );
    CHECK( ham.has_term<ElectronKinetic>() );
    CHECK( ham.get_terms<ElectronKinetic>().size() == 1 );
  }

  SECTION("Two Operator Construction") {
    DerivedOperator ham( ElectronKinetic{}, ElectronNuclearCoulomb{} );
    CHECK( ham.get_terms<ElectronKinetic>().size()        == 1 );
    CHECK( ham.get_terms<ElectronNuclearCoulomb>().size() == 1 );
  }

  SECTION("Single Term Add") {
    DerivedOperator ham; ham.add_term(ElectronKinetic{});
    CHECK( ham.get_terms<ElectronKinetic>().size() == 1 );
  }

  SECTION("Term Add Chain") {
    DerivedOperator ham; 
    ham.add_term(ElectronKinetic{}).add_term(ElectronNuclearCoulomb{});
    CHECK( ham.get_terms<ElectronKinetic>().size()        == 1 );
    CHECK( ham.get_terms<ElectronNuclearCoulomb>().size() == 1 );
  }
}
