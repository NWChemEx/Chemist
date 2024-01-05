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
#include <chemist/fragmenting/capping/cap.hpp>

using namespace chemist::fragmenting;

using value_type = Cap::value_type;

TEST_CASE("fragmenting::capping::Cap") {
    Cap defaulted;
    value_type nucleus0, nucleus1{"H", 1ul, 1837.289, 1.0, 2.0, 3.0};

    // Single-nucleus cap
    Cap c12(1, 2, nucleus0);

    // Cap comprised of two nuclei
    Cap c23(2, 3, nucleus0, nucleus1);

    SECTION("CTors") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") {
            REQUIRE(c12.size() == 1);
            REQUIRE(c12.get_anchor_index() == 1);
            REQUIRE(c12.get_replaced_index() == 2);
            REQUIRE(c12.at(0) == nucleus0);

            REQUIRE(c23.size() == 2);
            REQUIRE(c23.get_anchor_index() == 2);
            REQUIRE(c23.get_replaced_index() == 3);
            REQUIRE(c23.at(0) == nucleus0);
            REQUIRE(c23.at(1) == nucleus1);
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

        SECTION("Move Assignment") {
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

    SECTION("insert") {
        Cap other_c23(2, 3, nucleus0);
        REQUIRE(other_c23 != c23);

        other_c23.insert(nucleus1);
        REQUIRE(other_c23 == c23);
    }

    SECTION("at") {
        REQUIRE_THROWS_AS(defaulted.at(0), std::out_of_range);

        REQUIRE(c12.at(0) == nucleus0);
        REQUIRE_THROWS_AS(c12.at(1), std::out_of_range);

        REQUIRE(c23.at(0) == nucleus0);
        REQUIRE(c23.at(1) == nucleus1);
        REQUIRE_THROWS_AS(c23.at(2), std::out_of_range);

        // Is writeable?
        c12.at(0) = nucleus1;
        REQUIRE(c12.at(0) == nucleus1);
    }

    SECTION("at() const") {
        REQUIRE_THROWS_AS(std::as_const(defaulted).at(0), std::out_of_range);

        REQUIRE(std::as_const(c12).at(0) == nucleus0);
        REQUIRE_THROWS_AS(std::as_const(c12).at(1), std::out_of_range);

        REQUIRE(std::as_const(c23).at(0) == nucleus0);
        REQUIRE(std::as_const(c23).at(1) == nucleus1);
        REQUIRE_THROWS_AS(std::as_const(c23).at(2), std::out_of_range);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(c12.size() == 1);
        REQUIRE(c23.size() == 2);
    }

    SECTION("set_anchor_index") {
        defaulted.set_anchor_index(1);
        REQUIRE(defaulted.get_anchor_index() == 1);

        // Can overwrite existing value
        c12.set_anchor_index(3);
        REQUIRE(c12.get_anchor_index() == 3);
    }

    SECTION("get_anchor_index") {
        REQUIRE_THROWS_AS(defaulted.get_anchor_index(),
                          std::bad_optional_access);

        REQUIRE(c12.get_anchor_index() == 1);
        REQUIRE(c23.get_anchor_index() == 2);
    }

    SECTION("set_replaced_index") {
        defaulted.set_replaced_index(1);
        REQUIRE(defaulted.get_replaced_index() == 1);

        // Can overwrite existing value
        c12.set_replaced_index(3);
        REQUIRE(c12.get_replaced_index() == 3);
    }

    SECTION("get_replaced_index") {
        REQUIRE_THROWS_AS(defaulted.get_replaced_index(),
                          std::bad_optional_access);

        REQUIRE(c12.get_replaced_index() == 2);
        REQUIRE(c23.get_replaced_index() == 3);
    }

    SECTION("comparisons") {
        // Default v default
        Cap other_default;
        REQUIRE(other_default == defaulted);
        REQUIRE_FALSE(other_default != defaulted);

        // Default vs non-default
        REQUIRE(defaulted != c12);
        REQUIRE_FALSE(defaulted == c12);

        // Same value
        Cap other_c12(1, 2, nucleus0);
        REQUIRE(c12 == other_c12);
        REQUIRE_FALSE(c12 != other_c12);

        // Different anchor
        Cap c42(4, 2, nucleus0);
        REQUIRE(c42 != c12);
        REQUIRE_FALSE(c42 == c12);

        // Different replaced
        Cap c14(1, 4, nucleus0);
        REQUIRE(c14 != c12);
        REQUIRE_FALSE(c14 == c12);

        // Different nucleus
        other_c12.at(0) = nucleus1;
        REQUIRE(c12 != other_c12);
        REQUIRE_FALSE(c12 == other_c12);

        // Different nucleus order
        Cap other_c23(2, 3, nucleus1, nucleus0);
        REQUIRE(c23 != other_c23);
        REQUIRE_FALSE(c23 == other_c23);
    }
}
