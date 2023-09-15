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
#include <chemist/basis_set/contracted_gaussian/detail_/contracted_gaussian_pimpl.hpp>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("ContractedGaussianPIMPL", "", float, double) {
    using prim_type = Primitive<TestType>;
    using cg_type   = detail_::ContractedGaussianPIMPL<prim_type>;

    using center_type        = typename cg_type::cg_traits::center_type;
    using coefficient_vector = typename cg_type::coefficient_vector;
    using exponent_vector    = typename cg_type::exponent_vector;

    coefficient_vector cs{1.0, 2.0, 3.0};
    exponent_vector es{4.0, 5.0, 6.0};
    center_type r0{7.0, 8.0, 9.0};

    cg_type cg0;
    cg_type cg1(cs, es, r0);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(cg0.size() == 0);
            REQUIRE(cg0.center() == center_type{});
        }

        SECTION("Value") {
            REQUIRE(cg1.size() == 3);
            REQUIRE(cg1.center() == r0);
            REQUIRE(cg1.coefficient(0) == 1.0);
            REQUIRE(cg1.coefficient(1) == 2.0);
            REQUIRE(cg1.coefficient(2) == 3.0);
            REQUIRE(cg1.exponent(0) == 4.0);
            REQUIRE(cg1.exponent(1) == 5.0);
            REQUIRE(cg1.exponent(2) == 6.0);
        }

        SECTION("Copy") {
            cg_type cg0_copy(cg0);
            REQUIRE(cg0_copy.size() == 0);
            REQUIRE(cg0_copy.center() == center_type{});

            cg_type cg1_copy(cg1);
            REQUIRE(cg1_copy.size() == 3);
            REQUIRE(cg1_copy.center() == r0);
            REQUIRE(cg1_copy.coefficient(0) == 1.0);
            REQUIRE(cg1_copy.coefficient(1) == 2.0);
            REQUIRE(cg1_copy.coefficient(2) == 3.0);
            REQUIRE(cg1_copy.exponent(0) == 4.0);
            REQUIRE(cg1_copy.exponent(1) == 5.0);
            REQUIRE(cg1_copy.exponent(2) == 6.0);
        }

        SECTION("Copy assignment") {
            cg_type cg0_copy;
            auto pcg0_copy = &(cg0_copy = cg0);
            REQUIRE(cg0_copy.size() == 0);
            REQUIRE(cg0_copy.center() == center_type{});
            REQUIRE(pcg0_copy == &cg0_copy);

            cg_type cg1_copy;
            auto pcg1_copy = &(cg1_copy = cg1);
            REQUIRE(cg1_copy.size() == 3);
            REQUIRE(cg1_copy.center() == r0);
            REQUIRE(cg1_copy.coefficient(0) == 1.0);
            REQUIRE(cg1_copy.coefficient(1) == 2.0);
            REQUIRE(cg1_copy.coefficient(2) == 3.0);
            REQUIRE(cg1_copy.exponent(0) == 4.0);
            REQUIRE(cg1_copy.exponent(1) == 5.0);
            REQUIRE(cg1_copy.exponent(2) == 6.0);
            REQUIRE(pcg1_copy == &cg1_copy);
        }

        SECTION("Move") {
            cg_type cg0_move(std::move(cg0));
            REQUIRE(cg0_move.size() == 0);
            REQUIRE(cg0_move.center() == center_type{});

            cg_type cg1_move(std::move(cg1));
            REQUIRE(cg1_move.size() == 3);
            REQUIRE(cg1_move.center() == r0);
            REQUIRE(cg1_move.coefficient(0) == 1.0);
            REQUIRE(cg1_move.coefficient(1) == 2.0);
            REQUIRE(cg1_move.coefficient(2) == 3.0);
            REQUIRE(cg1_move.exponent(0) == 4.0);
            REQUIRE(cg1_move.exponent(1) == 5.0);
            REQUIRE(cg1_move.exponent(2) == 6.0);
        }

        SECTION("Move assignment") {
            cg_type cg0_move;
            auto pcg0_move = &(cg0_move = std::move(cg0));
            REQUIRE(cg0_move.size() == 0);
            REQUIRE(cg0_move.center() == center_type{});
            REQUIRE(pcg0_move == &cg0_move);

            cg_type cg1_move;
            auto pcg1_move = &(cg1_move = std::move(cg1));
            REQUIRE(cg1_move.size() == 3);
            REQUIRE(cg1_move.center() == r0);
            REQUIRE(cg1_move.coefficient(0) == 1.0);
            REQUIRE(cg1_move.coefficient(1) == 2.0);
            REQUIRE(cg1_move.coefficient(2) == 3.0);
            REQUIRE(cg1_move.exponent(0) == 4.0);
            REQUIRE(cg1_move.exponent(1) == 5.0);
            REQUIRE(cg1_move.exponent(2) == 6.0);
            REQUIRE(pcg1_move == &cg1_move);
        }
    }

    SECTION("Getters/setters") {
        // N.B. Bounds checking is left to the CG class itself, so calling
        //      for coefficients or exponents of cg0 will likely cause segfaults

        SECTION("add_primitive") {
            cg0.add_primitive(1.0, 4.0);
            REQUIRE(cg0.size() == 1);
            REQUIRE(cg0.coefficient(0) == 1.0);
            REQUIRE(cg0.exponent(0) == 4.0);

            cg1.add_primitive(4.0, 7.0);
            REQUIRE(cg1.size() == 4);
            REQUIRE(cg1.coefficient(3) == 4.0);
            REQUIRE(cg1.exponent(3) == 7.0);
        }

        SECTION("center()") {
            REQUIRE(cg0.center() == center_type{});
            REQUIRE(cg1.center() == r0);

            // Is writable?
            cg0.center() = r0;
            REQUIRE(cg0.center() == r0);
        }

        SECTION("center() const") {
            REQUIRE(std::as_const(cg0).center() == center_type{});
            REQUIRE(std::as_const(cg1).center() == r0);
        }

        SECTION("coefficient()") {
            REQUIRE(cg1.coefficient(0) == 1.0);
            REQUIRE(cg1.coefficient(1) == 2.0);
            REQUIRE(cg1.coefficient(2) == 3.0);

            // Is writeable?
            cg1.coefficient(0) = 4.0;
            REQUIRE(cg1.coefficient(0) == 4.0);
        }

        SECTION("coefficient() const") {
            REQUIRE(std::as_const(cg1).coefficient(0) == 1.0);
            REQUIRE(std::as_const(cg1).coefficient(1) == 2.0);
            REQUIRE(std::as_const(cg1).coefficient(2) == 3.0);
        }

        SECTION("exponent()") {
            REQUIRE(cg1.exponent(0) == 4.0);
            REQUIRE(cg1.exponent(1) == 5.0);
            REQUIRE(cg1.exponent(2) == 6.0);

            // Is writeable?
            cg1.exponent(0) = 7.0;
            REQUIRE(cg1.exponent(0) == 7.0);
        }

        SECTION("exponent() const") {
            REQUIRE(std::as_const(cg1).exponent(0) == 4.0);
            REQUIRE(std::as_const(cg1).exponent(1) == 5.0);
            REQUIRE(std::as_const(cg1).exponent(2) == 6.0);
        }

        SECTION("operator[]") {
            REQUIRE(cg1[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(&cg1[0].coefficient() == &cg1.coefficient(0));
            REQUIRE(&cg1[0].exponent() == &cg1.exponent(0));
            REQUIRE(cg1[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(&cg1[1].coefficient() == &cg1.coefficient(1));
            REQUIRE(&cg1[1].exponent() == &cg1.exponent(1));
            REQUIRE(cg1[2] == prim_type(3.0, 6.0, r0));
            REQUIRE(&cg1[2].coefficient() == &cg1.coefficient(2));
            REQUIRE(&cg1[2].exponent() == &cg1.exponent(2));

            // Is writeable?
            cg1[0] = prim_type(4.0, 7.0, center_type());
            REQUIRE(cg1.coefficient(0) == 4.0);
            REQUIRE(cg1.exponent(0) == 7.0);
            REQUIRE(cg1.center() == center_type{});
        }

        SECTION("operator[] const") {
            const auto& const_cg1 = std::as_const(cg1);
            REQUIRE(const_cg1[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(&const_cg1[0].coefficient() == &const_cg1.coefficient(0));
            REQUIRE(&const_cg1[0].exponent() == &const_cg1.exponent(0));
            REQUIRE(const_cg1[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(&const_cg1[1].coefficient() == &const_cg1.coefficient(1));
            REQUIRE(&const_cg1[1].exponent() == &const_cg1.exponent(1));
            REQUIRE(const_cg1[2] == prim_type(3.0, 6.0, r0));
            REQUIRE(&const_cg1[2].coefficient() == &const_cg1.coefficient(2));
            REQUIRE(&const_cg1[2].exponent() == &const_cg1.exponent(2));
        }

        SECTION("size") {
            REQUIRE(cg0.size() == 0);
            REQUIRE(cg1.size() == 3);
        }
    }

    SECTION("operator==") {
        // Default v default
        REQUIRE(cg0 == cg_type{});

        // Default v non-default
        REQUIRE_FALSE(cg0 == cg1);

        // Non-default: same values
        REQUIRE(cg1 == cg_type(cs, es, r0));

        // Non-default: different coefficients
        REQUIRE_FALSE(cg1 == cg_type(es, cs, r0));

        // Non-default: different exponents
        REQUIRE_FALSE(cg1 == cg_type(cs, cs, r0));

        // Non-default: different center
        REQUIRE_FALSE(cg1 == cg_type(cs, es, center_type{}));
    }
}
