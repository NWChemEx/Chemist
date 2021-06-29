#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace libchemist;


template <typename OpType, std::size_t RefNE, std::size_t RefNN>
void check_operator_particle_count() {
    STATIC_REQUIRE( OpType::n_electrons == RefNE );
    STATIC_REQUIRE( OpType::n_nuclei    == RefNN );
}

template <typename OpType>
void check_stateless_operator_equality() {
  OpType lhs, rhs;
  REQUIRE( lhs == rhs );
  REQUIRE( static_cast<const Operator&>(lhs) == static_cast<const Operator&>(rhs) );
}

TEST_CASE("Operator Sanity") {
    SECTION("Particle Count") {
        check_operator_particle_count< ElectronKinetic,         1, 0>();
        check_operator_particle_count< ElectronNuclearCoulomb,  1, 1>();
        check_operator_particle_count< ElectronElectronCoulomb, 2, 0>();

        check_operator_particle_count< MeanFieldElectronCoulomb,       1, 0>();
        check_operator_particle_count< MeanFieldElectronExactExchange, 1, 0>();
        check_operator_particle_count< KohnShamExchangeCorrelation,    1, 0>();

        //check_operator_particle_count< ClassicalElectronDipoleField,     1, 0>();
        //check_operator_particle_count< ClassicalElectronQuadrupoleField, 1, 0>();
        //check_operator_particle_count< ClassicalElectronOctupoleField,   1, 0>();

    }

    SECTION("Stateless Operator Comparison") {
        check_stateless_operator_equality<ElectronKinetic>();
        check_stateless_operator_equality<ElectronElectronCoulomb>();
        check_stateless_operator_equality<MeanFieldElectronCoulomb>();
        check_stateless_operator_equality<MeanFieldElectronExactExchange>();
        check_stateless_operator_equality<KohnShamExchangeCorrelation>();
    }

    SECTION("Electron-Nuclear Coulomb Comparison") {
        ElectronNuclearCoulomb lhs, rhs;
        lhs.potential().add_charge( PointCharge<double>(-1., 0., 0., 0.) );
        lhs.potential().add_charge( PointCharge<double>(-1., 0., 0., 2.) );
        rhs.potential().add_charge( PointCharge<double>(-2., 0., 0., 0.) );
        rhs.potential().add_charge( PointCharge<double>(-2., 0., 0., 2.) );

        REQUIRE( lhs == lhs );
        REQUIRE( rhs == rhs );
        REQUIRE( lhs != rhs );

        const auto& lhs_as_operator = static_cast<const Operator&>(lhs);
        const auto& rhs_as_operator = static_cast<const Operator&>(rhs);

        REQUIRE( lhs_as_operator == lhs_as_operator );
        REQUIRE( rhs_as_operator == rhs_as_operator );
        REQUIRE( lhs_as_operator != rhs_as_operator );
    }
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
