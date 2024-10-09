/*
 * Copyright 2024 NWChemEx-Project
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

#include "../test_helpers.hpp"
#include <chemist/electron/many_electrons.hpp>

TEST_CASE("ManyElectrons") {
    using chemist::ManyElectrons;
    using electron_type = typename ManyElectrons::value_type;

    ManyElectrons defaulted;
    ManyElectrons value(2);
    electron_type e;

    SECTION("Ctors and assignment") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }

        SECTION("Value") {
            REQUIRE(value.size() == 2);
            REQUIRE(value[0] == e);
            REQUIRE(value[1] == e);
        }

        SECTION("Copy/Move") {
            test_chemist::test_copy_and_move(defaulted, value);
        }
    }

    SECTION("at_") {
        REQUIRE(value[0] == e);
        REQUIRE(value[1] == e);
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(value)[0] == e);
        REQUIRE(std::as_const(value)[1] == e);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(value.size() == 2);
    }

    SECTION("operator==") {
        // Default vs. default
        REQUIRE(defaulted == ManyElectrons{});

        // Default vs. non-default
        REQUIRE_FALSE(defaulted == value);

        // Value vs same value
        REQUIRE(value == ManyElectrons{2});

        // Value vs. different value
        REQUIRE_FALSE(value == ManyElectrons{3});
    }

    SECTION("operator!=") {
        // Just negates operator== so spot checking is fine
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != ManyElectrons{});
    }
}