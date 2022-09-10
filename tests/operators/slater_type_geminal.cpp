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

#include "chemist/operators/slater_type_geminal.hpp"
#include "test_operator.hpp"

TEMPLATE_LIST_TEST_CASE("SlaterTypeGeminal", "", testing::stg_types) {
    using stg_type      = TestType;
    using coef_type     = typename stg_type::coefficient_type;
    using exponent_type = typename stg_type::exponent_type;

    SECTION("Typedefs") {
        SECTION("coefficient_type") {
            STATIC_REQUIRE(std::is_same_v<coef_type, double>);
        }

        SECTION("exponent_type") {
            STATIC_REQUIRE(std::is_same_v<exponent_type, double>);
        }
    }

    stg_type stg;
    stg_type diff_exponent(3.1);
    stg_type non_default(3.1, -1.0 / 3.1);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(stg.coefficient == -1.0 / 1.2);
            REQUIRE(stg.exponent == 1.2);
        }

        SECTION("Value") {
            REQUIRE(diff_exponent.coefficient == -1.0 / 1.2);
            REQUIRE(diff_exponent.exponent == 3.1);

            REQUIRE(non_default.coefficient == -1.0 / 3.1);
            REQUIRE(non_default.exponent == 3.1);
        }
    }

    SECTION("operator*=") {
        auto pstg = &(stg *= stg);
        REQUIRE(pstg == &stg);
        REQUIRE(stg.coefficient == (1.0 / 1.2) * (1.0 / 1.2));
        REQUIRE(stg.exponent == 2.4);
    }

    SECTION("operator*=") {
        auto rv = stg * stg;
        REQUIRE(rv.coefficient == (1.0 / 1.2) * (1.0 / 1.2));
        REQUIRE(rv.exponent == 2.4);
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(stg);
            SECTION("RHS == default") {
                REQUIRE(lhs == hash_objects(stg_type{}));
            }
            SECTION("RHS different exponent") {
                REQUIRE(lhs != hash_objects(diff_exponent));
            }
            SECTION("RHS different coefficient") {
                stg_type rhs(1.2, -1.0 / 3.1);
                REQUIRE(lhs != hash_objects(rhs));
            }
        }
    }

    SECTION("Comparisons") {
        SECTION("LHS == default") {
            SECTION("RHS == default") {
                stg_type rhs;
                REQUIRE(stg == rhs);
                REQUIRE_FALSE(stg != rhs);
            }
            SECTION("RHS different exponent") {
                REQUIRE(stg != diff_exponent);
                REQUIRE_FALSE(stg == diff_exponent);
            }
            SECTION("RHS different coefficient") {
                stg_type rhs(1.2, -1.0 / 3.1);
                REQUIRE(stg != rhs);
                REQUIRE_FALSE(stg == rhs);
            }
        }
    }
}
