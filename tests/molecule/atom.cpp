/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chemist/molecule/atom.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <map>
#include <sstream>
#include <type_traits>

using namespace chemist;
using size_type   = typename Atom::size_type;
using coord_type  = typename Atom::coord_type;
using mass_type   = typename Atom::mass_type;
using name_type   = typename Atom::name_type;
using charge_type = typename Atom::charge_type;

void check_atom(Atom& ai, const coord_type& coords, size_type Z, mass_type m,
                name_type name, double q) {
    const Atom& const_ai = ai;

    REQUIRE(ai.x() == coords[0]);
    REQUIRE(ai.y() == coords[1]);
    REQUIRE(ai.z() == coords[2]);

    REQUIRE(const_ai.x() == coords[0]);
    REQUIRE(const_ai.y() == coords[1]);
    REQUIRE(const_ai.z() == coords[2]);

    REQUIRE(ai.charge() == q);
    REQUIRE(const_ai.charge() == q);

    REQUIRE(ai.name() == name);
    REQUIRE(const_ai.name() == name);

    REQUIRE(ai.Z() == Z);
    REQUIRE(const_ai.Z() == Z);

    REQUIRE(ai.mass() == m);
    REQUIRE(const_ai.mass() == m);
}

TEST_CASE("Atom Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<coord_type, std::array<double, 3>>);
        REQUIRE(std::is_same_v<mass_type, double>);
        REQUIRE(std::is_same_v<name_type, std::string>);
        REQUIRE(std::is_same_v<charge_type, double>);
    }

    coord_type carts{1.0, 2.0, 3.0};
    size_type Z{1ul};
    mass_type m{1.0079};
    name_type h{"H"};

    SECTION("Default Ctor") {
        Atom ai;
        check_atom(ai, coord_type{}, 0, 0.0, "", 0.0);
    }

    SECTION("State Ctor") {
        // I'm not going to test all permuations of this...

        SECTION("Mass") {
            Atom ai(m);
            check_atom(ai, coord_type{}, 0, m, "", 0.0);
        }

        SECTION("Carts") {
            Atom ai(carts);
            check_atom(ai, carts, 0, 0.0, "", 0.0);
        }

        SECTION("Z") {
            Atom ai(Z);
            check_atom(ai, coord_type{}, Z, 0.0, "", 1.0);
        }

        SECTION("Name") {
            Atom ai(h);
            check_atom(ai, coord_type{}, 0, 0.0, h, 0.0);
        }

        SECTION("Z and Carts") {
            Atom ai(Z, carts);
            check_atom(ai, carts, Z, 0.0, "", 1.0);
        }

        SECTION("Z, Carts, and mass") {
            Atom ai(carts, Z, m);
            check_atom(ai, carts, Z, m, "", 1.0);
        }

        SECTION("Z, Carts, mass, and name") {
            Atom ai(h, carts, m, Z);
            check_atom(ai, carts, Z, m, h, 1.0);
        }
    }

    Atom ai(h, m, Z, carts);
    SECTION("Copy Ctor") {
        Atom a2(ai);
        check_atom(a2, carts, Z, m, h, 1.0);
        REQUIRE(a2 == ai);
        REQUIRE(!(a2 != ai));
    }

    SECTION("Copy Assignment") {
        Atom a2;
        REQUIRE(a2 != ai);
        REQUIRE(!(a2 == ai));
        Atom& pai = (a2 = ai);
        check_atom(a2, carts, Z, m, h, 1.0);
        REQUIRE(a2 == ai);
        REQUIRE(!(a2 != ai));
        REQUIRE(&pai == &a2);
    }

    SECTION("Move Ctor") {
        Atom a2(std::move(ai));
        check_atom(a2, carts, Z, m, h, 1.0);
    }

    SECTION("Move Assignment") {
        Atom a2;
        Atom& pai = (a2 = std::move(ai));
        check_atom(a2, carts, Z, m, h, 1.0);
        REQUIRE(&pai == &a2);
    }

    SECTION("Printing") {
        std::string corr =
          "H 1.000000000000000 2.000000000000000 3.000000000000000";
        std::stringstream ss;
        ss << Atom{h, carts};
        REQUIRE(ss.str() == corr);
    }

} // TEST_CASE("Atom Class")

TEST_CASE("Atom serialization") {
    Atom a("H", coord_type{1.0, 2.0, 3.0}, 1.0079, size_type{1});
    Atom a2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(a);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(a2);
    }
    REQUIRE(a == a2);
}
