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

#include "test_dsl.hpp"
#include <chemist/dsl/add.hpp>

/* Testing Strategy.
 *
 * The classes which derive from BinaryOp are strong types. We thus only need
 * to test the BinaryOp infrastructure for one derived class (we must test
 * through the derived class because of the CRTP usage).
 */

TEMPLATE_LIST_TEST_CASE("BinaryOp", "", test_chemist::binary_types) {
    using lhs_type = std::tuple_element_t<0, TestType>;
    using rhs_type = std::tuple_element_t<1, TestType>;

    auto values     = test_chemist::binary_values();
    auto [lhs, rhs] = std::get<TestType>(values);

    chemist::dsl::Add<lhs_type, rhs_type> a_xx(lhs, rhs);
    chemist::dsl::Add<const lhs_type, rhs_type> a_cx(lhs, rhs);
    chemist::dsl::Add<lhs_type, const rhs_type> a_xc(lhs, rhs);
    chemist::dsl::Add<const lhs_type, const rhs_type> a_cc(lhs, rhs);

    SECTION("CTors") {
        REQUIRE(a_xx.lhs() == lhs);
        REQUIRE(a_xx.rhs() == rhs);

        REQUIRE(a_cx.lhs() == lhs);
        REQUIRE(a_cx.rhs() == rhs);

        REQUIRE(a_xc.lhs() == lhs);
        REQUIRE(a_xc.rhs() == rhs);

        REQUIRE(a_cc.lhs() == lhs);
        REQUIRE(a_cc.rhs() == rhs);
    }

    SECTION("lhs()") {
        REQUIRE(a_xx.lhs() == lhs);
        REQUIRE(a_cx.lhs() == lhs);
        REQUIRE(a_xc.lhs() == lhs);
        REQUIRE(a_cc.lhs() == lhs);
    }

    SECTION("lhs() const") {
        REQUIRE(std::as_const(a_xx).lhs() == lhs);
        REQUIRE(std::as_const(a_cx).lhs() == lhs);
        REQUIRE(std::as_const(a_xc).lhs() == lhs);
        REQUIRE(std::as_const(a_cc).lhs() == lhs);
    }

    SECTION("rhs()") {
        REQUIRE(a_xx.rhs() == rhs);
        REQUIRE(a_cx.rhs() == rhs);
        REQUIRE(a_xc.rhs() == rhs);
        REQUIRE(a_cc.rhs() == rhs);
    }

    SECTION("rhs() const") {
        REQUIRE(std::as_const(a_xx).rhs() == rhs);
        REQUIRE(std::as_const(a_cx).rhs() == rhs);
        REQUIRE(std::as_const(a_xc).rhs() == rhs);
        REQUIRE(std::as_const(a_cc).rhs() == rhs);
    }

    SECTION("operator==") {
        SECTION("Same values") {
            chemist::dsl::Add<lhs_type, rhs_type> add2(lhs, rhs);
            REQUIRE(a_xx == add2);
        }

        SECTION("Different const-ness") {
            REQUIRE(a_xx == a_cx);
            REQUIRE(a_xx == a_xc);
            REQUIRE(a_xx == a_cc);
            REQUIRE(a_cx == a_xc);
            REQUIRE(a_cx == a_cc);
            REQUIRE(a_xc == a_cc);
        }

        SECTION("Different values") {
            lhs_type lhs2;
            rhs_type rhs2;
            chemist::dsl::Add<lhs_type, rhs_type> add_l(lhs2, rhs);
            chemist::dsl::Add<lhs_type, rhs_type> add_r(lhs, rhs2);
            REQUIRE_FALSE(a_xx == add_l);
            REQUIRE_FALSE(a_xx == add_r);
        }

        SECTION("Different type") {
            char a = 'a';
            chemist::dsl::Add<char, rhs_type> add_l(a, rhs);
            chemist::dsl::Add<lhs_type, char> add_r(lhs, a);
            REQUIRE_FALSE(a_xx == add_l);
            REQUIRE_FALSE(a_xx == add_r);
        }
    }

    SECTION("operator!=") {
        // Just negates operator== so spot check
        lhs_type lhs2;
        chemist::dsl::Add<lhs_type, rhs_type> add_r(lhs2, rhs);
        REQUIRE_FALSE(a_xx != a_cx);
        REQUIRE(a_xx != add_r);
    }
}