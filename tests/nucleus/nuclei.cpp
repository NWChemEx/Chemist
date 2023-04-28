/*
 * Copyright 2023 NWChemEx-Project
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
#include <chemist/nucleus/nuclei.hpp>

using namespace chemist;

/* Testing Notes:
 *
 * We assume the base class has been tested and works correctly. Subject to
 * that assumption, we need to test the ability to access elements by reference
 * (since we wrote the implementation), and obtaining the size, but do not need
 * to retest all the ways one can access the elements (i.e., operator[], at, and
 * iterators).
 */
TEST_CASE("Nuclei") {
    using set_type   = Nuclei;
    using value_type = typename set_type::value_type;

    value_type n0(1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    value_type n1(2ul, 4.0, 5.0, 6.0, 7.0, 5.0);

    set_type defaulted;
    set_type nuclei{n0, n1, n1}; // <- ensure we can add same charge 2x

    // Collect the addresses for testing aliasing/references
    using charge_type        = typename value_type::charge_type;
    using atomic_number_type = typename value_type::atomic_number_type;

    std::vector<charge_type*> old_addresses;
    std::vector<atomic_number_type*> z_addresses;
    for(std::size_t nuke_i = 0; nuke_i < 3; ++nuke_i) {
        z_addresses.push_back(&nuclei[nuke_i].Z());
        old_addresses.push_back(&nuclei[nuke_i].mass());
        old_addresses.push_back(&nuclei[nuke_i].charge());
        for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
            old_addresses.push_back(&nuclei[nuke_i].coord(coord_i));
        }
    }

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }

        SECTION("initializer_list") {
            REQUIRE(nuclei.size() == 3);
            REQUIRE(nuclei[0] == n0);
            REQUIRE(nuclei[1] == n1);
            REQUIRE(nuclei[2] == n1);
        }

        SECTION("Copy") {
            set_type copy0(defaulted);
            set_type copy1(nuclei);
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == nuclei);
        }

        SECTION("Move") {
            set_type copy0(defaulted);
            set_type move0(std::move(defaulted));
            set_type copy1(nuclei);

            // Do the move
            set_type move1(std::move(nuclei));

            // Check the values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);

            // Check that original references are still valid
            for(std::size_t nuke_i = 0; nuke_i < 3; ++nuke_i) {
                const auto idx = nuke_i * 5;
                REQUIRE(z_addresses[nuke_i] == &move1[nuke_i].Z());
                REQUIRE(old_addresses[idx] == &move1[nuke_i].mass());
                REQUIRE(old_addresses[idx + 1] == &move1[nuke_i].charge());
                for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                    const auto p = &move1[nuke_i].coord(coord_i);
                    REQUIRE(old_addresses[idx + 2 + coord_i] == p);
                }
            }
        }

        SECTION("Copy assignment") {
            set_type copy0;
            auto pcopy0 = &(copy0 = defaulted);
            set_type copy1;
            auto pcopy1 = &(copy1 = nuclei);

            // Check values
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == nuclei);

            // Check returns *this for chaining
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("Move assignment") {
            set_type copy0(defaulted);
            set_type move0;
            auto pmove0 = &(move0 = std::move(defaulted));
            set_type copy1(nuclei);
            set_type move1;

            // Do the move
            auto pmove1 = &(move1 = std::move(nuclei));

            // Check values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);

            // Check returns *this for chaining
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);

            // Check that original references are still valid
            for(std::size_t nuke_i = 0; nuke_i < 3; ++nuke_i) {
                const auto idx = nuke_i * 5;
                REQUIRE(z_addresses[nuke_i] == &move1[nuke_i].Z());
                REQUIRE(old_addresses[idx] == &move1[nuke_i].mass());
                REQUIRE(old_addresses[idx + 1] == &move1[nuke_i].charge());
                for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                    const auto p = &move1[nuke_i].coord(coord_i);
                    REQUIRE(old_addresses[idx + 2 + coord_i] == p);
                }
            }
        }
    }

    SECTION("push_back") {
        defaulted.push_back(n0);
        defaulted.push_back(n1);
        defaulted.push_back(n1);
        REQUIRE(defaulted == nuclei);
    }

    SECTION("at_()") {
        using rtype = decltype(nuclei[0]);
        STATIC_REQUIRE(std::is_same_v<rtype, typename set_type::reference>);

        for(std::size_t nuke_i = 0; nuke_i < 3; ++nuke_i) {
            const auto idx  = nuke_i * 5;
            const auto corr = nuke_i == 0 ? n0 : n1;
            auto qi         = nuclei[nuke_i];

            REQUIRE(qi.Z() == corr.Z());
            REQUIRE(z_addresses[nuke_i] == &qi.Z());

            REQUIRE(qi.mass() == corr.mass());
            REQUIRE(old_addresses[idx] == &qi.mass());

            REQUIRE(qi.charge() == corr.charge());
            REQUIRE(old_addresses[idx + 1] == &qi.charge());

            for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                auto& coord = qi.coord(coord_i);

                REQUIRE(coord == corr.coord(coord_i));
                REQUIRE(old_addresses[idx + 2 + coord_i] == &coord);
            }
        }
    }

    SECTION("at_() const") {
        using rtype = decltype(std::as_const(nuclei)[0]);
        using corr  = typename set_type::const_reference;
        STATIC_REQUIRE(std::is_same_v<rtype, corr>);

        for(std::size_t nuke_i = 0; nuke_i < 3; ++nuke_i) {
            const auto idx  = nuke_i * 5;
            const auto corr = nuke_i == 0 ? n0 : n1;
            auto qi         = std::as_const(nuclei)[nuke_i];

            REQUIRE(qi.Z() == corr.Z());
            REQUIRE(z_addresses[nuke_i] == &qi.Z());

            REQUIRE(qi.mass() == corr.mass());
            REQUIRE(old_addresses[idx] == &qi.mass());

            REQUIRE(qi.charge() == corr.charge());
            REQUIRE(old_addresses[idx + 1] == &qi.charge());

            for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                auto& coord = std::as_const(nuclei)[nuke_i].coord(coord_i);

                REQUIRE(coord == corr.coord(coord_i));
                REQUIRE(old_addresses[idx + 2 + coord_i] == &coord);
            }
        }
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(nuclei.size() == 3);
    }
}
