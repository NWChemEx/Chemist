#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/orbital_space/canonical_space.hpp"
#include "test_orbital_space.hpp"
#include <catch2/catch.hpp>

/* For testing purposes we assume:
 *
 * - The base class more-or-less passes through the CanonicalSpace_ class. i.e.,
 *   if the base class works, it'll work when used with CanonicalSpace_ too.
 *
 *   - The caveats are that we need to make sure it's accounted for in the ctor,
 *     hash, and comparison operations.
 *
 * - The overlap matrices provided by TensorMaker are suitable for use as
 *   orbital energies.
 */

using namespace libchemist;
using namespace libchemist::orbital_space;
using namespace libchemist::test;

TEMPLATE_PRODUCT_TEST_CASE("CanonicalSpace", "",
                           (type::tensor, type::tensor_of_tensors),
                           (float, double)) {
    // Assemble types needed for this unit test
    using tensor_type = TestType;
    using base_type   = BaseSpace_<tensor_type>;
    using space_type  = CanonicalSpace_<tensor_type, base_type>;

    // Some basic objects used many times throughout the unit test
    auto& world = TA::get_default_world();
    auto S1     = TensorMaker<tensor_type>::S(world);
    auto S2     = TensorMaker<tensor_type>::S2(world);
    base_type bs1(S1), bs2(S2);

    space_type st1(S1, S2);

    SECTION("Typedefs") {
        using orb_egy_type = typename space_type::orbital_energy_type;
        STATIC_REQUIRE(std::is_same_v<orb_egy_type, tensor_type>);
    }

    SECTION("CTors") {
        SECTION("Value") {
            REQUIRE(compare_tensors(st1.orbital_energies(), S1));
        }

        SECTION("Copy") {
            space_type st2(st1);
            REQUIRE(st2 == st1);
        }

        SECTION("Move") {
            space_type st2(S1, S2), st3(std::move(st1));
            REQUIRE(st2 == st3);
        }

        SECTION("Copy Assignment") {
            space_type st2;
            auto pst2 = &(st2 = st1);
            SECTION("Value") { REQUIRE(st2 == st1); }
            SECTION("Returns this") { REQUIRE(pst2 == &st2); }
        }

        SECTION("Move") {
            space_type st2(S1, S2), st3;
            auto pst3 = &(st3 = std::move(st1));
            SECTION("Value") { REQUIRE(st2 == st3); }
            SECTION("Returns this") { REQUIRE(pst3 == &st3); }
        }
    }

    SECTION("orbital_energies()") {
        REQUIRE(compare_tensors(st1.orbital_energies(), S1));
    }

    SECTION("orbital_energies() const") {
        REQUIRE(compare_tensors(std::as_const(st1).orbital_energies(), S1));
    }

    SECTION("hash") {
        auto hash1 = runtime::hash_objects(st1);

        SECTION("Same value") {
            auto hash2 = runtime::hash_objects(space_type(S1, S2));
            REQUIRE(hash1 == hash2);
        }

        SECTION("Different orbital energies") {
            auto hash2 = runtime::hash_objects(space_type(S2, S2));
            REQUIRE(hash1 != hash2);
        }

        SECTION("Different base instances") {
            auto hash2 = runtime::hash_objects(space_type(S1, S1));
            REQUIRE(hash1 != hash2);
        }
    }

    SECTION("Comparisons") {
        SECTION("Same value") {
            space_type st2(S1, S2);
            REQUIRE(st1 == st2);
            REQUIRE_FALSE(st1 != st2);
        }

        SECTION("Different orbital energies") {
            space_type st2(S2, S2);
            REQUIRE_FALSE(st1 == st2);
            REQUIRE(st1 != st2);
        }

        SECTION("Different base instances") {
            space_type st2(S1, S1);
            REQUIRE_FALSE(st1 == st2);
            REQUIRE(st1 != st2);
        }
    }
}
