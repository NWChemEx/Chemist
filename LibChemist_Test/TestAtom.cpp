#include <LibChemist/Atom.hpp>
#include <catch/catch.hpp>
#include <type_traits>
#include <map>
#include <sstream>

using namespace LibChemist;
using Property      = typename Atom::Property;
using property_type = typename Atom::property_type;
using size_type     = typename Atom::size_type;
using coord_type    = typename Atom::coord_type;
using name_type     = typename Atom::name_type;
using prop_map_type = std::map<Property, property_type>;

void check_atom(Atom& ai,
                const coord_type& coords,
                const prop_map_type& props,
                const name_type& name) {
    const Atom& const_ai = ai;
    const Property vdw_radius = Property::vdw_radius;

    REQUIRE(ai.coords() == coords);
    REQUIRE(const_ai.coords() == coords);
    REQUIRE(ai.name() == name);
    REQUIRE(const_ai.name() == name);

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
        REQUIRE(std::is_same_v<name_type, std::string>);
    }

    prop_map_type map{{Property::mass, 1.0079}};
    coord_type carts{1.0, 2.0, 3.0};
    name_type h{"H"};

    SECTION("Default Ctor") {
        Atom ai;
        check_atom( ai, coord_type{}, prop_map_type{}, name_type{});
    }

    SECTION("State Ctor") {
        //I'm not going to test all permuations of this...


        SECTION("One property") {
            Atom ai(Property::mass, 1.0079);
            check_atom(ai, coord_type{}, map, name_type{});
        }

        SECTION("Two properties") {
            map[Property::cov_radius] = 0.031;
            Atom ai(Property::mass, 1.0079, Property::cov_radius, 0.031);
            check_atom(ai, coord_type{}, map, name_type{});
        }

        SECTION("Carts") {
            Atom ai(carts[0], carts[1], carts[2]);
            check_atom(ai, carts, prop_map_type{}, name_type{});
        }

        SECTION("Carts and Prop") {
            Atom ai(carts[0], carts[1], carts[2], Property::mass, 1.0079);
            check_atom(ai, carts, map, name_type{});
        }

        SECTION("Prop, Carts, and Prop") {
            map[Property::cov_radius] = 0.031;
            Atom ai(Property::mass, 1.0079, carts[0], carts[1], carts[2],
                    Property::cov_radius, 0.031);
            check_atom(ai, carts, map, name_type{});
        }

        SECTION("Name") {
            Atom ai(h);
            check_atom(ai, coord_type{}, prop_map_type{}, h);
        }

        SECTION("Name and Prop") {
            Atom ai(h, Property::mass, 1.0079);
            check_atom(ai, coord_type{}, map, h);
        }

        SECTION("Prop and Name ") {
            Atom ai(Property::mass, 1.0079, h);
            check_atom(ai, coord_type{}, map, h);
        }

        SECTION("Atomic Number") {
            Atom ai(1L);
            REQUIRE(ai.at_num() == 1L);
            REQUIRE(const_cast<const Atom&>(ai).at_num() == 1L);
        }
    }

    SECTION("Copy Ctor") {
        Atom ai(Property::mass, 1.0079);
        Atom a2(ai);
        check_atom(a2, coord_type{}, map, name_type{});
    }

    SECTION("Copy Assignment") {
        Atom ai;
        Atom a2(carts);
        Atom& pai = (ai = a2);
        check_atom(ai, carts, prop_map_type{}, name_type{});
        REQUIRE(&pai == &ai);
    }

    SECTION("Move Ctor") {
        Atom ai(Property::mass, 1.0079);
        Atom a2(std::move(ai));
        check_atom(a2, coord_type{}, map, name_type{});
    }

    SECTION("Move Assignment") {
        Atom ai;
        Atom a2(h);
        Atom& pai = (ai = std::move(a2));
        check_atom(ai, coord_type{}, prop_map_type{}, h);
        REQUIRE(&pai == & ai);
    }

    SECTION("Printing") {
        std::string corr =
          "H 1.000000000000000 2.000000000000000 3.000000000000000";
        std::stringstream ss;
        ss << Atom{h, carts};
        REQUIRE(ss.str() == corr);
    }


} // TEST_CASE("Atom Class")
