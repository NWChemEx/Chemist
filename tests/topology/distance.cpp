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

#include "chemist/topology/distance.hpp"
#include <catch2/catch.hpp>

using namespace chemist::topology;

using value_type      = typename Distance::value_type;
using reference_type  = typename Distance::reference_type;
using const_reference = typename Distance::const_reference;

TEST_CASE("Distance") {
    SECTION("Typedefs") {
        STATIC_REQUIRE(std::is_same_v<value_type, double>);
        STATIC_REQUIRE(std::is_same_v<reference_type, double&>);
        STATIC_REQUIRE(std::is_same_v<const_reference, const double&>);
    }

    SECTION("Default CTor") {
        Distance d;
        REQUIRE(d.value() == value_type{0.0});
    }

    Distance d(1.23);

    SECTION("Value CTor") { REQUIRE(d.value() == value_type{1.23}); }

    SECTION("Copy CTor") {
        Distance CopyOfd(d);
        REQUIRE(CopyOfd.value() == value_type{1.23});
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        REQUIRE(hash_objects(d) == hash_objects(Distance{1.23}));
        REQUIRE(hash_objects(d) != hash_objects(Distance{}));
    }

    SECTION("Comparisons") {
        SECTION("LHS == RHS") {
            Distance d2(1.23);
            REQUIRE(d == d2);
            REQUIRE_FALSE(d != d2);
        }
        SECTION("LHS != RHS") {
            Distance d2;
            REQUIRE_FALSE(d == d2);
            REQUIRE(d != d2);
        }
    }

    SECTION("Printing") {
        std::stringstream ss;
        ss << d;
        REQUIRE(ss.str() == "1.2300000000000000 (a.u.)");
    }
}
