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
#include <chemist/chemical_system/point_charge/charges_view/charges_view.hpp>

template<typename ChargesType>
void test_charges_view_guts() {
    using charges_type      = ChargesType;
    using view_type         = chemist::ChargesView<charges_type>;
    using point_charge_type = typename view_type::value_type;

    // Make some Point objects and put them in a PointSet
    point_charge_type q0{0.0, 0.0, 0.0, 0.0}, q1{-1.1, 1.0, 2.0, 3.0},
      q2{-2.2, 4.0, 5.0, 6.0};
    charges_type defaulted_qs;
    charges_type charges_qs{q0, q1, q2};

    view_type defaulted;
    view_type no_charges(defaulted_qs);
    view_type charges(charges_qs);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }

        SECTION("Value") {
            REQUIRE(no_charges.size() == 0);
            REQUIRE(charges.size() == 3);
            REQUIRE(charges[0] == q0);
            REQUIRE(charges[1] == q1);
            REQUIRE(charges[2] == q2);
        }

        SECTION("Copy") {
            view_type defaulted_copy(defaulted);
            REQUIRE(defaulted == defaulted_copy);

            view_type no_charges_copy(no_charges);
            REQUIRE(no_charges_copy == no_charges);

            view_type charges_copy(charges);
            REQUIRE(charges_copy == charges);
        }

        SECTION("Copy assignment") {
            view_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(defaulted == defaulted_copy);
            REQUIRE(pdefaulted_copy == &defaulted_copy);

            view_type no_charges_copy;
            auto pno_charges_copy = &(no_charges_copy = no_charges);
            REQUIRE(no_charges_copy == no_charges);
            REQUIRE(pno_charges_copy == &no_charges_copy);

            view_type charges_copy;
            auto pcharges_copy = &(charges_copy = charges);
            REQUIRE(charges_copy == charges);
            REQUIRE(pcharges_copy == &charges_copy);
        }

        SECTION("Move") {
            view_type defaulted_copy(defaulted);
            view_type defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_move == defaulted_copy);

            view_type no_charges_copy(no_charges);
            view_type no_charges_move(std::move(no_charges));
            REQUIRE(no_charges_copy == no_charges_move);

            view_type charges_copy(charges);
            view_type charges_move(std::move(charges));
            REQUIRE(charges_copy == charges_move);
        }

        SECTION("Move assignment") {
            view_type defaulted_copy(defaulted);
            view_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(defaulted_move == defaulted_copy);
            REQUIRE(pdefaulted_move == &defaulted_move);

            view_type no_charges_copy(no_charges);
            view_type no_charges_move;
            auto pno_charges_move = &(no_charges_move = std::move(no_charges));
            REQUIRE(no_charges_copy == no_charges_move);
            REQUIRE(pno_charges_move == &no_charges_move);

            view_type charges_copy(charges);
            view_type charges_move;
            auto pcharges_move = &(charges_move = std::move(charges));
            REQUIRE(charges_copy == charges_move);
            REQUIRE(pcharges_move == &charges_move);
        }
    }

    SECTION("at_()") {
        REQUIRE(charges[0] == q0);
        REQUIRE(charges[1] == q1);
        REQUIRE(charges[2] == q2);
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(charges)[0] == q0);
        REQUIRE(std::as_const(charges)[1] == q1);
        REQUIRE(std::as_const(charges)[2] == q2);
    }

    SECTION("size_()") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(no_charges.size() == 0);
        REQUIRE(charges.size() == 3);
    }

    SECTION("operator==") {
        SECTION("Default vs default") { REQUIRE(defaulted == view_type{}); }
        SECTION("Default vs. empty") { REQUIRE(defaulted == no_charges); }
        SECTION("Default vs. non-empty") {
            REQUIRE_FALSE(defaulted == charges);
        }
        SECTION("Empty vs. empty") {
            view_type no_charges2(defaulted_qs);
            REQUIRE(no_charges == no_charges2);
        }
        SECTION("Empty vs. non-empty") { REQUIRE_FALSE(no_charges == charges); }
        SECTION("Same non-empty state") {
            view_type charges2(charges_qs);
            REQUIRE(charges == charges2);
        }
        SECTION("Different charges") {
            // n.b. we swapped the order
            charges_type qs2{q1, q0, q2};
            view_type charges2(qs2);
            REQUIRE_FALSE(charges == charges2);
        }

        SECTION("ChargesView vs. Charges") {
            REQUIRE(charges == charges_qs);
            REQUIRE(charges_qs == charges);
            REQUIRE(charges != defaulted_qs);
            REQUIRE(defaulted_qs != charges);
        }
    }
}

TEMPLATE_TEST_CASE("ChargesView<T>", "", float, double) {
    using charges_type = chemist::Charges<TestType>;
    test_charges_view_guts<charges_type>();
}

TEMPLATE_TEST_CASE("ChargesView<const T>", "", float, double) {
    using charges_type = chemist::Charges<TestType>;
    test_charges_view_guts<const charges_type>();
}
