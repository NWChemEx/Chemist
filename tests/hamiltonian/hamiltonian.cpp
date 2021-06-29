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


    SECTION("Default Comparisons") {
        Hamiltonian h1, h2;
        auto& d1 = static_cast<const DerivedOperator&>(h1);
        auto& d2 = static_cast<const DerivedOperator&>(h2);
        REQUIRE( h1 == h1 );
        REQUIRE( h1.is_equal(h1) );
        REQUIRE( h1.is_equal(d1) );
        REQUIRE( d1.is_equal(h1) );
        REQUIRE( d1.is_equal(d1) );

        REQUIRE( h1 == h2 );
        REQUIRE( h1.is_equal(h2) );
        REQUIRE( h1.is_equal(d2) );
        REQUIRE( d1.is_equal(h2) );
        REQUIRE( d1.is_equal(d2) );

        REQUIRE( h2.is_equal(h1) );
        REQUIRE( h2.is_equal(d1) );
        REQUIRE( d2.is_equal(h1) );
        REQUIRE( d2.is_equal(d1) );
    }


    SECTION("Stateful Comparisons") {
        Hamiltonian h1(ElectronKinetic{}), h2(ElectronElectronCoulomb{});
        auto& d1 = static_cast<const DerivedOperator&>(h1);
        auto& d2 = static_cast<const DerivedOperator&>(h2);

        REQUIRE( h1 != h2 );
        REQUIRE( not h1.is_equal(h2) );
        REQUIRE( not h1.is_equal(d2) );
        REQUIRE( not d1.is_equal(h2) );
        REQUIRE( not d1.is_equal(d2) );

        REQUIRE( not h2.is_equal(h1) );
        REQUIRE( not h2.is_equal(d1) );
        REQUIRE( not d2.is_equal(h1) );
        REQUIRE( not d2.is_equal(d1) );
    }




}

