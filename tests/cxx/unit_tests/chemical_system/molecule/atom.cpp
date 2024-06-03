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

#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <chemist/chemical_system/molecule/atom.hpp>
#include <map>
#include <sstream>
#include <type_traits>

using namespace chemist;

// Typedefs from Atom
using size_type   = typename Atom::size_type;
using coord_type  = typename Atom::coord_type;
using mass_type   = typename Atom::mass_type;
using name_type   = typename Atom::name_type;
using charge_type = typename Atom::charge_type;

// Typedefs for testing
using coords_type = std::array<coord_type, 3>;

void check_atom(Atom& ai, const coords_type& coords, size_type Z, mass_type m,
                name_type name, double q, size_type n) {
    const Atom& const_ai = ai;

    REQUIRE(ai.x() == coords[0]);
    REQUIRE(ai.y() == coords[1]);
    REQUIRE(ai.z() == coords[2]);

    REQUIRE(const_ai.x() == coords[0]);
    REQUIRE(const_ai.y() == coords[1]);
    REQUIRE(const_ai.z() == coords[2]);

    REQUIRE(ai.nuclear_charge() == q);
    REQUIRE(const_ai.nuclear_charge() == q);

    REQUIRE(ai.n_electrons() == n);
    REQUIRE(const_ai.n_electrons() == n);

    REQUIRE(const_ai.charge() == q - n);

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
        REQUIRE(std::is_same_v<coord_type, double>);
        REQUIRE(std::is_same_v<mass_type, double>);
        REQUIRE(std::is_same_v<name_type, std::string>);
        REQUIRE(std::is_same_v<charge_type, double>);
    }

    coords_type carts{1.0, 2.0, 3.0};
    size_type Z{1ul};
    mass_type m{1.0079};
    name_type h{"H"};

    SECTION("Default Ctor") {
        Atom ai;
        check_atom(ai, coords_type{}, 0, 0.0, "", 0.0, 0);
    }

    SECTION("State Ctors") {
        SECTION("With charge") {
            Atom ai(h, Z, m, 1.0, 2.0, 3.0, 4.0);
            check_atom(ai, carts, Z, m, h, 4.0, Z);
        }
        SECTION("With n_electrons") {
            Atom ai(h, Z, m, 1.0, 2.0, 3.0, 4.0, 2);
            check_atom(ai, carts, Z, m, h, 4.0, 2);
        }
        SECTION("Without charge") {
            Atom ai(h, Z, m, 1.0, 2.0, 3.0);
            check_atom(ai, carts, Z, m, h, 1.0, Z);
        }
    }

    Atom ai(h, Z, m, 1.0, 2.0, 3.0);
    SECTION("Copy Ctor") {
        Atom a2(ai);
        check_atom(a2, carts, Z, m, h, 1.0, Z);
        REQUIRE(a2 == ai);
        REQUIRE(!(a2 != ai));
    }

    SECTION("Copy Assignment") {
        Atom a2;
        REQUIRE(a2 != ai);
        REQUIRE(!(a2 == ai));
        Atom& pai = (a2 = ai);
        check_atom(a2, carts, Z, m, h, 1.0, Z);
        REQUIRE(a2 == ai);
        REQUIRE(!(a2 != ai));
        REQUIRE(&pai == &a2);
    }

    SECTION("Move Ctor") {
        Atom a2(std::move(ai));
        check_atom(a2, carts, Z, m, h, 1.0, Z);
    }

    SECTION("Move Assignment") {
        Atom a2;
        Atom& pai = (a2 = std::move(ai));
        check_atom(a2, carts, Z, m, h, 1.0, Z);
        REQUIRE(&pai == &a2);
    }

    SECTION("Printing") {
        std::stringstream corr;
        corr << "nelectrons : 1," << std::endl;
        corr << "name : H," << std::endl;
        corr << "atomic number : 1," << std::endl;
        corr << "mass : 0," << std::endl;
        corr << "charge : 1," << std::endl;
        corr << "x : 1," << std::endl;
        corr << "y : 2," << std::endl;
        corr << "z : 3";
        std::stringstream ss;
        ss << Atom{h, 1ul, 0.0, 1.0, 2.0, 3.0};
        REQUIRE(ss.str() == corr.str());
    }

} // TEST_CASE("Atom Class")

TEST_CASE("Atom serialization") {
    Atom a("H", size_type{1}, 1.0079, 1.0, 2.0, 3.0);
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
