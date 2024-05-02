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

#include <catch2/catch.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>

template<typename DerivedType>
void test_fragmented_base() {
    using class_type = chemist::fragmenting::FragmentedBase<DerivedType>;

    DerivedType empty_supersystem;

    class_type defaulted;
    class_type empty(empty_supersystem);

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.is_null()); }

        SECTION("value") {
            REQUIRE_FALSE(empty.is_null());
            REQUIRE(empty.supersystem() == empty_supersystem);
        }
    }

    SECTION("is_null") {
        REQUIRE(defaulted.is_null());
        REQUIRE_FALSE(empty.is_null());
    }
}
