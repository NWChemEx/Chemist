#include <LibChemist/Atom.hpp>
#include <catch/catch.hpp>
#include <type_traits>

using namespace LibChemist;
using Property = typename Atom::Property;
using property_type = typename Atom::property_type;
using size_type     = typename Atom::size_type;
using coord_type    = typename Atom::coord_type;
using prop_map_type = std::map<Property, property_type>;

void check_atom(Atom& ai,
                const coord_type& coords,
                const prop_map_type& props) {
    const Atom& const_ai = ai;
    const Property vdw_radius = Property::vdw_radius;

    REQUIRE(ai.coords() == coords);
    REQUIRE(const_ai.coords() == coords);
    for(size_type i = 0; i < 3; ++i){
        REQUIRE(ai[i] == coords[i]);
        REQUIRE(const_ai[i] == coords[i]);
    }

    //We never set vdw_radius in these tests
    REQUIRE_THROWS_AS(ai.property(vdw_radius), std::out_of_range);
    REQUIRE_THROWS_AS(const_ai.property(vdw_radius), std::out_of_range);
    REQUIRE_THROWS_AS(ai[vdw_radius], std::out_of_range);
    REQUIRE_THROWS_AS(const_ai[vdw_radius], std::out_of_range);
    for(auto& [prop, value] : props ){
        REQUIRE(ai.count(prop));
        REQUIRE(ai.property(prop) == value);
        REQUIRE(const_ai.property(prop) == value);
        REQUIRE(ai[prop] == value);
        REQUIRE(const_ai[prop] == value);
    }
}


TEST_CASE("Atom Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<Property, AtomProperty>);
        REQUIRE(std::is_same_v<property_type, double>);
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<coord_type, std::array<double, 3>>);
    }

    SECTION("Default Ctor") {
        Atom ai;
        check_atom( ai, coord_type{}, prop_map_type{});
    }

    SECTION("Prop Ctor") {
        prop_map_type map{{Property::mass, 1.0079}};
        SECTION("One property"){
            Atom ai(Property::mass, 1.0079);
            check_atom(ai, coord_type{}, map);
        }

        SECTION("Two properties") {
            map[Property::cov_radius] = 0.031;
            Atom ai(Property::mass, 1.0079, Property::cov_radius, 0.031);
            check_atom(ai, coord_type{}, map);
        }
    }

    SECTION("Copy Ctor") {
        Atom ai(Property::mass, 1.0079);
        Atom a2(ai);
        check_atom(a2, coord_type{}, prop_map_type{{Property::mass, 1.0079}});
    }

    SECTION("Copy Assignment") {
        Atom ai;
        Atom a2(Property::mass, 1.0079);
        Atom& pai = (ai = a2);
        check_atom(ai, coord_type{}, prop_map_type{{Property::mass, 1.0079}});
        REQUIRE(&pai == &ai);
    }

    SECTION("Move Ctor") {
        Atom ai(Property::mass, 1.0079);
        Atom a2(std::move(ai));
        check_atom(a2, coord_type{}, prop_map_type{{Property::mass, 1.0079}});
    }

    SECTION("Move Assignment") {
        Atom ai;
        Atom a2(Property::mass, 1.0079);
        Atom& pai = (ai = std::move(a2));
        check_atom(ai, coord_type{}, prop_map_type{{Property::mass, 1.0079}});
        REQUIRE(&pai == & ai);
    }

} // TEST_CASE("Atom Class")
