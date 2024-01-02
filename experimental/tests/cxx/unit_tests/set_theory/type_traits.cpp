/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/set_theory/type_traits.hpp"
#include <catch2/catch.hpp>
#include <vector>

using namespace chemist::set_theory;

using type_list = std::tuple<int, double, float, std::string>;

TEMPLATE_LIST_TEST_CASE("IsFamilyOfSets", "", type_list) {
    using T   = TestType;
    using fos = FamilyOfSets<std::vector<T>>;
    SECTION("Not a family of sets") {
        STATIC_REQUIRE_FALSE(detail_::IsFamilyOfSets<T>::value);
    }
    SECTION("Is a family of sets") {
        STATIC_REQUIRE(detail_::IsFamilyOfSets<fos>::value);
    }
}

TEMPLATE_LIST_TEST_CASE("is_family_of_sets_v", "", type_list) {
    using T   = TestType;
    using fos = FamilyOfSets<std::vector<T>>;
    SECTION("Not a family") { STATIC_REQUIRE_FALSE(is_family_of_sets_v<T>); }
    SECTION("Is a family") { STATIC_REQUIRE(is_family_of_sets_v<fos>); }
}
