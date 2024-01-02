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
#include <chemist/capping/cap_set.hpp>

using namespace chemist;

TEST_CASE("CapSet") {
    CapSet defaulted;

    CapSet::atom_type atom0;

    Cap cap0;
    Cap cap1(1, 2);

    CapSet has_values{cap0, cap1};

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Initializer List") {
            REQUIRE(has_values.size() == 2);
            REQUIRE(has_values.at(0) == cap0);
            REQUIRE(has_values.at(1) == cap1);
        }

        SECTION("Copy") {
            CapSet other_defaulted(defaulted);
            REQUIRE(defaulted == other_defaulted);

            CapSet other_values(has_values);
            REQUIRE(has_values == other_values);
        }

        SECTION("Move") {
            CapSet other_defaulted(defaulted);
            CapSet moved_defaulted(std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);

            CapSet other_values(has_values);
            CapSet moved_values(std::move(has_values));
            REQUIRE(moved_values == other_values);
        }

        SECTION("Copy Assignment") {
            CapSet other_defaulted;
            auto pother_defaulted = &(other_defaulted = defaulted);
            REQUIRE(defaulted == other_defaulted);
            REQUIRE(pother_defaulted == &other_defaulted);

            CapSet other_values;
            auto pother_values = &(other_values = has_values);
            REQUIRE(has_values == other_values);
            REQUIRE(pother_values == &other_values);
        }

        SECTION("Move Assignment") {
            CapSet other_defaulted(defaulted);
            CapSet moved_defaulted;
            auto pmoved_defaulted = &(moved_defaulted = std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);
            REQUIRE(pmoved_defaulted == &moved_defaulted);

            CapSet other_values(has_values);
            CapSet moved_values;
            auto pmoved_values = &(moved_values = std::move(has_values));
            REQUIRE(moved_values == other_values);
            REQUIRE(pmoved_values == &moved_values);
        }
    }

    SECTION("push_back") {
        defaulted.push_back(cap0);
        defaulted.push_back(cap1);
        REQUIRE(defaulted == has_values);
    }

    SECTION("add_cap") {
        defaulted.push_back(cap0);

        // Cap with no atoms
        defaulted.add_cap(1, 2);
        REQUIRE(defaulted == has_values);

        // Cap with one atom
        defaulted.add_cap(2, 3, atom0);
        REQUIRE(defaulted[2] == Cap(2, 3, atom0));

        // Cap with two atoms
        defaulted.add_cap(3, 4, atom0, atom0);
        REQUIRE(defaulted[3] == Cap(3, 4, atom0, atom0));
    }

    SECTION("at_") {
        // N.B. at_ is used by the base class to implement element access. We
        // rely on the public API to test that we actually implemented it right.

        REQUIRE(has_values[0] == cap0);
        REQUIRE(has_values[1] == cap1);

        // Is writeable
        has_values[1] = cap0;
        REQUIRE(has_values[1] == cap0);
    }

    SECTION("at_() const") {
        // N.B. at_ is used by the base class to implement element access. We
        // rely on the public API to test that we actually implemented it right.

        REQUIRE(std::as_const(has_values)[0] == cap0);
        REQUIRE(std::as_const(has_values)[1] == cap1);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(has_values.size() == 2);
    }

    SECTION("Comparisons") {
        // Default v default
        CapSet other_default;
        REQUIRE(defaulted == other_default);
        REQUIRE_FALSE(defaulted != other_default);

        // Default vs non-default
        REQUIRE(defaulted != has_values);
        REQUIRE_FALSE(defaulted == has_values);

        // Same value
        CapSet other_values{cap0, cap1};
        REQUIRE(has_values == other_values);
        REQUIRE_FALSE(has_values != other_values);

        // Different order
        CapSet diff_order{cap1, cap0};
        REQUIRE(has_values != diff_order);
        REQUIRE_FALSE(has_values == diff_order);

        // Different caps
        CapSet cs0{cap0}, cs1{cap1};
        REQUIRE(cs0 != cs1);
        REQUIRE_FALSE(cs0 == cs1);
    }
}
