#include <catch2/catch.hpp>
#include <chemist/nucleus/fragmented_nuclei.hpp>

using namespace chemist;

TEST_CASE("FragmentedNuclei") {
    FragmentedNuclei defaulted;
    FragmentedNuclei empty(chemist::Nuclei{});
    Nuclei nukes;
    nukes.push_back(Nuclei::value_type("H", 1ul, 1.0, 0.0, 0.0, 0.0, 1.0));
    nukes.push_back(Nuclei::value_type("He", 2ul, 4.0, 1.0, 2.0, 3.0, 2.0));
    nukes.push_back(Nuclei::value_type("C", 6ul, 12.0, 4.0, 5.0, 6.0, 6.0));
    FragmentedNuclei full(nukes);
    full.add_fragment(0);
    full.add_fragment(1);
    full.add_fragment(2);

    SECTION("CTors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("value") {
            REQUIRE(empty.size() == 0);
            REQUIRE(empty.supersystem() == Nuclei{});

            REQUIRE(full.size() == 3);
            REQUIRE(full.supersystem() == nukes);
        }
        SECTION("copy") {
            FragmentedNuclei other_defaulted(defaulted);
            REQUIRE(defaulted == other_defaulted);

            FragmentedNuclei other_empty(empty);
            REQUIRE(empty == other_empty);

            FragmentedNuclei other_full(full);
            REQUIRE(full == other_full);
        }
        SECTION("move") {
            FragmentedNuclei other_defaulted(defaulted);
            FragmentedNuclei moved_defaulted(std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);

            FragmentedNuclei other_empty(empty);
            FragmentedNuclei moved_empty(std::move(empty));
            REQUIRE(moved_empty == other_empty);

            FragmentedNuclei other_full(full);
            FragmentedNuclei moved_full(std::move(full));
            REQUIRE(moved_full == other_full);
        }
    }

    SECTION("comparisons") {
        // Default v default
        REQUIRE(defaulted == FragmentedNuclei());
        REQUIRE_FALSE(defaulted != FragmentedNuclei());

        // Default v non-default
        REQUIRE_FALSE(defaulted == empty);
        REQUIRE(defaulted != empty);

        // Both hold default Nuclei objects
        REQUIRE(empty == FragmentedNuclei(Nuclei()));
        REQUIRE_FALSE(empty != FragmentedNuclei(Nuclei()));

        // Different Nuclei objects
        FragmentedNuclei other_nukes(nukes);
        REQUIRE_FALSE(empty == other_nukes);
        REQUIRE(empty != FragmentedNuclei(nukes));

        // Same Nuclei objects, different sets
        REQUIRE_FALSE(full == other_nukes);
        REQUIRE(full != other_nukes);

        other_nukes.add_fragment(0);
        other_nukes.add_fragment(1);
        other_nukes.add_fragment(2);
        REQUIRE(full == other_nukes);
        REQUIRE_FALSE(full != other_nukes);
    }
}
