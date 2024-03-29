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

#include "test_operator.hpp"
#include <chemist/operators/multipole.hpp>
#include <chemist/point/point.hpp>

using namespace chemist::operators;

// Tuple containing the known
using type_tuple =
  std::tuple<ElectricDipole, ElectricQuadrupole, ElectricOctupole>;

namespace {

template<typename T>
void check_string(const T& multipole) {
    if constexpr(std::is_same_v<T, ElectricDipole>) {
        REQUIRE(multipole.as_string() == "r");
    } else if constexpr(std::is_same_v<T, ElectricQuadrupole>) {
        REQUIRE(multipole.as_string() == "r²");
    } else if constexpr(std::is_same_v<T, ElectricOctupole>) {
        REQUIRE(multipole.as_string() == "r³");
    }
}

} // namespace

TEMPLATE_LIST_TEST_CASE("Multipole", "", type_tuple) {
    using multipole_type = TestType;
    using point_type     = typename multipole_type::point_type;

    SECTION("Typedefs") {
        SECTION("value_type") {
            using corr = chemist::Point<double>;
            STATIC_REQUIRE(std::is_same_v<point_type, corr>);
        }
    }

    multipole_type defaulted;
    auto p = testing::non_default_parameter<point_type>();
    multipole_type has_value(p);

    SECTION("CTors") {
        SECTION("Default") { REQUIRE(defaulted == multipole_type{}); }

        SECTION("With Value") { REQUIRE(has_value == multipole_type{p}); }

        SECTION("Copy") {
            multipole_type copy(has_value);
            REQUIRE(copy == multipole_type{p});
        }

        SECTION("Move") {
            multipole_type moved(std::move(has_value));
            REQUIRE(moved == multipole_type{p});
        }
    }

    SECTION("Assignment") {
        SECTION("Copy") {
            multipole_type copy;
            auto pcopy = &(copy = has_value);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == multipole_type{p});
        }

        SECTION("Move") {
            multipole_type moved;
            auto pmoved = &(moved = std::move(has_value));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved == multipole_type{p});
        }
    }

    SECTION("value") {
        SECTION("Default") {
            REQUIRE(defaulted.gauge_origin() == point_type{0., 0., 0.});
        }
        SECTION("With Value") { REQUIRE(has_value.gauge_origin() == p); }
    }

    SECTION("Comparison") {
        SECTION("Both default") {
            multipole_type rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        SECTION("Both have same value") {
            multipole_type rhs(p);
            REQUIRE(has_value == rhs);
            REQUIRE_FALSE(has_value != rhs);
        }

        SECTION("Different values") {
            REQUIRE(defaulted != has_value);
            REQUIRE_FALSE(defaulted == has_value);
        }
    }

    SECTION("as_string") { check_string(defaulted); }
}
