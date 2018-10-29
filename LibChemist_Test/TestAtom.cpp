#include <LibChemist/Atom.hpp>
#include <SDE/Memoization.hpp>
#include <catch2/catch.hpp>
#include <map>
#include <sstream>
#include <type_traits>

using namespace LibChemist;
using size_type  = typename Atom::size_type;
using coord_type = typename Atom::coord_type;
using mass_type  = typename Atom::mass_type;
using name_type  = typename Atom::name_type;

static const std::array<std::string, 8> corr_hashes{
  "a1677191299b2e9bd2bb9c1f2ca7eb14", // Default ctor
  "0d6b2ee007a680ce2194ee3ee8ae8bcc", // Just mass
  "4feafab7208c06719cf9736858f7a0fa", // Just carts
  "d369eaa12dc906e43568a5246d2773e3", // Just Z
  "8a5fc30361b938770046df63cdb1b079", // Just name
  "90cb004d31e9ff7972892489da856720", // Z and carts
  "3d3ddb2af0da9ceab35ddf72216a2edc", // Z, carts, and mass
  "d89b9a76a5bf23161f572a20cee5a325"  // Z, carts, mass, and name
};

template<size_t hash>
void check_atom(Atom& ai, const coord_type& coords, size_type Z, mass_type m,
                name_type name) {
    const Atom& const_ai = ai;

    REQUIRE(ai.coords() == coords);
    REQUIRE(const_ai.coords() == coords);
    double* pCarts        = &ai[0];
    const double* pcCarts = &const_ai[0];
    for(size_type i = 0; i < 3; ++i) {
        REQUIRE(ai[i] == coords[i]);
        REQUIRE(const_ai[i] == coords[i]);
        REQUIRE(pCarts[i] == coords[i]);
        REQUIRE(pcCarts[i] == coords[i]);
    }

    REQUIRE(ai.name() == name);
    REQUIRE(const_ai.name() == name);

    REQUIRE(ai.Z() == Z);
    REQUIRE(const_ai.Z() == Z);

    REQUIRE(ai.mass() == m);
    REQUIRE(const_ai.mass() == m);

    SDE::Hasher h(SDE::HashType::Hash128);
    h(ai);
    REQUIRE(bphash::hash_to_string(h.finalize()) == corr_hashes[hash]);
}

TEST_CASE("Atom Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<coord_type, std::array<double, 3>>);
        REQUIRE(std::is_same_v<mass_type, double>);
        REQUIRE(std::is_same_v<name_type, std::string>);
    }

    coord_type carts{1.0, 2.0, 3.0};
    size_type Z{1ul};
    mass_type m{1.0079};
    name_type h{"H"};

    SECTION("Default Ctor") {
        Atom ai;
        check_atom<0>(ai, coord_type{}, 0, 0.0, "");
    }

    SECTION("State Ctor") {
        // I'm not going to test all permuations of this...

        SECTION("Mass") {
            Atom ai(m);
            check_atom<1>(ai, coord_type{}, 0, m, "");
        }

        SECTION("Carts") {
            Atom ai(carts);
            check_atom<2>(ai, carts, 0, 0.0, "");
        }

        SECTION("Z") {
            Atom ai(Z);
            check_atom<3>(ai, coord_type{}, Z, 0.0, "");
        }

        SECTION("Name") {
            Atom ai(h);
            check_atom<4>(ai, coord_type{}, 0, 0.0, h);
        }

        SECTION("Z and Carts") {
            Atom ai(Z, carts);
            check_atom<5>(ai, carts, Z, 0.0, "");
        }

        SECTION("Z, Carts, and mass") {
            Atom ai(carts, Z, m);
            check_atom<6>(ai, carts, Z, m, "");
        }

        SECTION("Z, Carts, mass, and name") {
            Atom ai(h, carts, m, Z);
            check_atom<7>(ai, carts, Z, m, h);
        }
    }

    Atom ai(h, m, Z, carts);
    SECTION("Copy Ctor") {
        Atom a2(ai);
        check_atom<7>(a2, carts, Z, m, h);
        REQUIRE(a2 == ai);
        REQUIRE(!(a2 != ai));
    }

    SECTION("Copy Assignment") {
        Atom a2;
        REQUIRE(a2 != ai);
        REQUIRE(!(a2 == ai));
        Atom& pai = (a2 = ai);
        check_atom<7>(a2, carts, Z, m, h);
        REQUIRE(a2 == ai);
        REQUIRE(!(a2 != ai));
        REQUIRE(&pai == &a2);
    }

    SECTION("Move Ctor") {
        Atom a2(std::move(ai));
        check_atom<7>(a2, carts, Z, m, h);
    }

    SECTION("Move Assignment") {
        Atom a2;
        Atom& pai = (a2 = std::move(ai));
        check_atom<7>(a2, carts, Z, m, h);
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
