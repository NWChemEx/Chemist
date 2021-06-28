#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;


template <typename OpType, std::size_t RefNE, std::size_t RefNN>
void check_operator_sanity() {
    STATIC_REQUIRE( OpType::n_electrons == RefNE );
    STATIC_REQUIRE( OpType::n_nuclei    == RefNN );
}

TEST_CASE("Operator Sanity") {
    check_operator_sanity< ElectronKinetic,         1, 0>();
    check_operator_sanity< ElectronNuclearCoulomb,  1, 1>();
    check_operator_sanity< ElectronElectronCoulomb, 2, 0>();

    check_operator_sanity< ClassicalElectronDipoleField,     1, 0>();
    check_operator_sanity< ClassicalElectronQuadrupoleField, 1, 0>();
    check_operator_sanity< ClassicalElectronOctupoleField,   1, 0>();

    check_operator_sanity< MeanFieldElectronCoulomb,       1, 0>();
    check_operator_sanity< MeanFieldElectronExactExchange, 1, 0>();
    check_operator_sanity< KohnShamExchangeCorrelation,    1, 0>();
}



TEST_CASE("Hamiltonian Class") {
  SECTION("Default CTor") {
    Hamiltonian ham;
    CHECK( ham.get_terms<ElectronKinetic>().size() == 0 );
  }

  SECTION("One Operator Construction") {
    Hamiltonian ham( ElectronKinetic{} );
    CHECK( ham.has_term<ElectronKinetic>() );
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
