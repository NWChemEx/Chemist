#include "libchemist/hamiltonian/operator.hpp"
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
  auto& lhs_as_operator = static_cast<const Operator&>(lhs);
  auto& rhs_as_operator = static_cast<const Operator&>(rhs);
  REQUIRE( lhs == rhs );
  REQUIRE( lhs_as_operator == rhs_as_operator ); // Trivially true
  REQUIRE( lhs_as_operator.is_equal(rhs) );
  REQUIRE( lhs_as_operator.is_equal(rhs_as_operator) );
  REQUIRE( lhs.is_equal(rhs) );
  REQUIRE( lhs.is_equal(rhs_as_operator) );
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
        lhs.add_charge( -1., 0., 0., 0. );
        lhs.add_charge( -1., 0., 0., 2. );
        rhs.add_charge( -2., 0., 0., 0. );
        rhs.add_charge( -2., 0., 0., 2. );

        REQUIRE( lhs == lhs );
        REQUIRE( rhs == rhs );
        REQUIRE( lhs != rhs );

        const auto& lhs_as_operator = static_cast<const Operator&>(lhs);
        const auto& rhs_as_operator = static_cast<const Operator&>(rhs);

        // These are trivial
        REQUIRE( lhs_as_operator == lhs_as_operator );
        REQUIRE( rhs_as_operator == rhs_as_operator );
        REQUIRE( lhs_as_operator == rhs_as_operator );

        // These are not
        REQUIRE( lhs.is_equal(lhs) );
        REQUIRE( lhs_as_operator.is_equal(lhs) );
        REQUIRE( lhs_as_operator.is_equal(lhs_as_operator) );
        REQUIRE( lhs.is_equal(lhs_as_operator) );

        REQUIRE( rhs.is_equal(rhs) );
        REQUIRE( rhs_as_operator.is_equal(rhs) );
        REQUIRE( rhs_as_operator.is_equal(rhs_as_operator) );
        REQUIRE( rhs.is_equal(rhs_as_operator) );

        REQUIRE( !lhs.is_equal(rhs) );
        REQUIRE( !lhs_as_operator.is_equal(rhs) );
        REQUIRE( !lhs_as_operator.is_equal(rhs_as_operator) );
        REQUIRE( !lhs.is_equal(rhs_as_operator) );

        REQUIRE( !rhs.is_equal(lhs) );
        REQUIRE( !rhs_as_operator.is_equal(lhs) );
        REQUIRE( !rhs_as_operator.is_equal(lhs_as_operator) );
        REQUIRE( !rhs.is_equal(lhs_as_operator) );
    }
}

