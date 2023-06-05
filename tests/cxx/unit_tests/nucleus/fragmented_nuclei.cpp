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
    full.add_fragment({0});
    full.add_fragment({1});
    full.add_fragment({2});

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
        SECTION("copy assignment") {
            FragmentedNuclei other_defaulted;
            auto pother_defaulted = &(defaulted = other_defaulted);
            REQUIRE(defaulted == other_defaulted);
            REQUIRE(pother_defaulted == &defaulted);

            FragmentedNuclei other_empty;
            auto pother_empty = &(other_empty = empty);
            REQUIRE(empty == other_empty);
            REQUIRE(pother_empty == &other_empty);

            FragmentedNuclei other_full;
            auto pother_full = &(other_full = full);
            REQUIRE(full == other_full);
            REQUIRE(pother_full == &other_full);
        }
        SECTION("move assignment") {
            FragmentedNuclei other_defaulted(defaulted);
            FragmentedNuclei moved_defaulted;
            auto pmoved_defaulted = &(moved_defaulted = std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);
            REQUIRE(pmoved_defaulted == &moved_defaulted);

            FragmentedNuclei other_empty(empty);
            FragmentedNuclei moved_empty;
            auto pmoved_empty = &(moved_empty = std::move(empty));
            REQUIRE(moved_empty == other_empty);
            REQUIRE(pmoved_empty == &moved_empty);

            FragmentedNuclei other_full(full);
            FragmentedNuclei moved_full;
            auto pmoved_full = &(moved_full = std::move(full));
            REQUIRE(moved_full == other_full);
            REQUIRE(pmoved_full == &moved_full);
        }
    }

    // Test we implemented size_ correctly, using the public-facing API
    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(empty.size() == 0);
        REQUIRE(full.size() == 3);
    }

    SECTION("add_fragment(initializer_list)") {
        REQUIRE_THROWS_AS(defaulted.add_fragment({0}), std::runtime_error);
        REQUIRE_THROWS_AS(empty.add_fragment({0}), std::out_of_range);

        REQUIRE(full.size() == 3);
        full.add_fragment({0, 1});
        REQUIRE(full.size() == 4);

        // Nothing happens if fragment is already present
        full.add_fragment({0, 1});
        REQUIRE(full.size() == 4);
    }

    SECTION("add_fragment(iterators)") {
        std::vector<std::size_t> indices{0, 1};
        auto b = indices.begin();
        auto e = indices.end();
        REQUIRE_THROWS_AS(defaulted.add_fragment(b, e), std::runtime_error);
        REQUIRE_THROWS_AS(empty.add_fragment(b, e), std::out_of_range);

        REQUIRE(full.size() == 3);
        full.add_fragment(indices.begin(), indices.end());
        REQUIRE(full.size() == 4);

        // Nothing happens if fragment is already present
        full.add_fragment(indices.begin(), indices.end());
        REQUIRE(full.size() == 4);
    }

    SECTION("has_supersystem") {
        REQUIRE_FALSE(defaulted.has_supersystem());
        REQUIRE(empty.has_supersystem());
        REQUIRE(full.has_supersystem());
    }

    SECTION("supersystem") {
        REQUIRE_THROWS_AS(defaulted.supersystem(), std::runtime_error);
        REQUIRE(empty.supersystem() == chemist::Nuclei());
        REQUIRE(full.supersystem() == nukes);
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

        other_nukes.add_fragment({0});
        other_nukes.add_fragment({1});
        other_nukes.add_fragment({2});
        REQUIRE(full == other_nukes);
        REQUIRE_FALSE(full != other_nukes);
    }
}
