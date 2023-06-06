#include <catch2/catch.hpp>
#include <chemist/capping/cap.hpp>

using namespace chemist;

using atom_type = Cap::atom_type;

TEST_CASE("Cap") {
    Cap defaulted;
    atom_type atom0, atom1{"H", 1ul, 1837.289, 1.0, 2.0, 3.0};

    // Single-atom cap
    Cap c12(1, 2, atom0);

    // Cap comprised of two caps
    Cap c23(2, 3, atom0, atom1);

    SECTION("CTors") {
        SECTION("default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.anchor_index() == 0);
            REQUIRE(defaulted.replaced_index() == 0);
        }
        SECTION("Value") {
            REQUIRE(c12.size() == 1);
            REQUIRE(c12.anchor_index() == 1);
            REQUIRE(c12.replaced_index() == 2);
            REQUIRE(c12.atom(0) == atom0);

            REQUIRE(c23.size() == 2);
            REQUIRE(c23.anchor_index() == 2);
            REQUIRE(c23.replaced_index() == 3);
            REQUIRE(c23.atom(0) == atom0);
            REQUIRE(c23.atom(1) == atom1);
        }
        SECTION("Copy") {
            Cap other_defaulted(defaulted);
            REQUIRE(defaulted == other_defaulted);

            Cap other_c12(c12);
            REQUIRE(c12 == other_c12);

            Cap other_c23(c23);
            REQUIRE(c23 == other_c23);
        }
        SECTION("Moved") {
            Cap other_defaulted(defaulted);
            Cap moved_defaulted(std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);

            Cap other_c12(c12);
            Cap moved_c12(std::move(c12));
            REQUIRE(moved_c12 == other_c12);

            Cap other_c23(c23);
            Cap moved_c23(std::move(c23));
            REQUIRE(moved_c23 == other_c23);
        }
        SECTION("Copy Assignment") {
            Cap other_defaulted;
            auto pother_defaulted = &(other_defaulted = defaulted);
            REQUIRE(defaulted == other_defaulted);
            REQUIRE(pother_defaulted == &other_defaulted);

            Cap other_c12;
            auto pother_c12 = &(other_c12 = c12);
            REQUIRE(c12 == other_c12);
            REQUIRE(pother_c12 == &other_c12);

            Cap other_c23;
            auto pother_c23 = &(other_c23 = c23);
            REQUIRE(c23 == other_c23);
            REQUIRE(pother_c23 == &other_c23);
        }
        SECTION("Moved") {
            Cap other_defaulted(defaulted);
            Cap moved_defaulted;
            auto pmoved_defaulted = &(moved_defaulted = std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);
            REQUIRE(pmoved_defaulted == &moved_defaulted);

            Cap other_c12(c12);
            Cap moved_c12;
            auto pmoved_c12 = &(moved_c12 = std::move(c12));
            REQUIRE(moved_c12 == other_c12);
            REQUIRE(pmoved_c12 == &moved_c12);

            Cap other_c23(c23);
            Cap moved_c23;
            auto pmoved_c23 = &(moved_c23 = std::move(c23));
            REQUIRE(moved_c23 == other_c23);
            REQUIRE(pmoved_c23 == &moved_c23);
        }
    }
}
