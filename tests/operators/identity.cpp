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

#include "chemist/operators/identity.hpp"
#include "test_operator.hpp"

using namespace chemist;
using namespace chemist::operators;

// Using ManyElectrons for distinguishability in != cases
using type_list = std::tuple<Identity<ManyElectrons>>;

TEMPLATE_LIST_TEST_CASE("Identity", "", type_list) {
    using identity_type  = TestType;
    using particle_types = typename identity_type::particle_type;
    using particle0_type = std::tuple_element_t<0, particle_types>;

    identity_type defaulted;

    auto p0 = testing::non_default_parameter<particle0_type>();
    identity_type non_default(p0);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.template at<0>() == particle0_type{});
        }
        SECTION("Value") { REQUIRE(non_default.template at<0>() == p0); }
        SECTION("Copy") {
            identity_type other(non_default);
            REQUIRE(other.template at<0>() == p0);
        }
        SECTION("Move") {
            identity_type other(std::move(non_default));
            REQUIRE(other.template at<0>() == p0);
        }
        SECTION("Copy assignment") {
            identity_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
        }
        SECTION("Move assignment") {
            identity_type copy;
            auto pcopy = &(copy = std::move(non_default));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                identity_type rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != non_default);
                REQUIRE_FALSE(defaulted == non_default);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Î"); }
}
