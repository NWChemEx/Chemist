#include <LibChemist/Atom.hpp>
#include <catch/catch.hpp>
#include<type_traits>

using namespace LibChemist;
using property_type = typename Atom::property_type;
using size_type = typename Atom::size_type;
using coord_type = typename Atom::coord_type;

TEST_CASE("Atom Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<property_type, double>);
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<coord_type, std::array<double, 3>>);
    }
} // TEST_CASE("Atom Class")


