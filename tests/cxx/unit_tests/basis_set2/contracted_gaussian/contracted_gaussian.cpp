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
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian.hpp>
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_traits.hpp>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("ContractedGaussian", "", float, double) {
    using prim_type = Primitive<TestType>;
    using cg_type   = ContractedGaussian<prim_type>;
    using cg_traits = ContractedGaussianTraits<cg_type>;

    using center_type = typename cg_traits::center_type;
    using coefficient_vector =
      std::vector<typename cg_traits::coefficient_type>;
    using exponent_vector = std::vector<typename cg_traits::exponent_type>;

    coefficient_vector cs{1.0, 2.0, 3.0};
    exponent_vector es{4.0, 5.0, 6.0};
    center_type r0{7.0, 8.0, 9.0};

    cg_type cg0;
    cg_type cg1(cs.begin(), cs.end(), es.begin(), es.end(), r0);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(cg0.size() == 0);
            REQUIRE(cg0.is_null());
        }

        SECTION("coefs, exps, x, y, z") {
            cg_type cg2(cs.begin(), cs.end(), es.begin(), es.end(), 7.0, 8.0,
                        9.0);

            REQUIRE(cg2.size() == 3);
            REQUIRE(cg2.center() == r0);
            REQUIRE(cg2[0].coefficient() == 1.0);
            REQUIRE(cg2[1].coefficient() == 2.0);
            REQUIRE(cg2[2].coefficient() == 3.0);
            REQUIRE(cg2[0].exponent() == 4.0);
            REQUIRE(cg2[1].exponent() == 5.0);
            REQUIRE(cg2[2].exponent() == 6.0);
        }

        SECTION("coefs, exps, Point") {
            REQUIRE(cg1.size() == 3);
            REQUIRE(cg1.center() == r0);
            REQUIRE(cg1[0].coefficient() == 1.0);
            REQUIRE(cg1[1].coefficient() == 2.0);
            REQUIRE(cg1[2].coefficient() == 3.0);
            REQUIRE(cg1[0].exponent() == 4.0);
            REQUIRE(cg1[1].exponent() == 5.0);
            REQUIRE(cg1[2].exponent() == 6.0);
        }

        SECTION("Copy") {
            cg_type cg0_copy(cg0);
            REQUIRE(cg0_copy.size() == 0);
            REQUIRE(cg0_copy.center() == center_type{});

            cg_type cg1_copy(cg1);
            REQUIRE(cg1_copy.size() == 3);
            REQUIRE(cg1_copy.center() == r0);
            REQUIRE(cg1_copy[0].coefficient() == 1.0);
            REQUIRE(cg1_copy[1].coefficient() == 2.0);
            REQUIRE(cg1_copy[2].coefficient() == 3.0);
            REQUIRE(cg1_copy[0].exponent() == 4.0);
            REQUIRE(cg1_copy[1].exponent() == 5.0);
            REQUIRE(cg1_copy[2].exponent() == 6.0);
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
            REQUIRE(cg1_copy[0].coefficient() == 1.0);
            REQUIRE(cg1_copy[1].coefficient() == 2.0);
            REQUIRE(cg1_copy[2].coefficient() == 3.0);
            REQUIRE(cg1_copy[0].exponent() == 4.0);
            REQUIRE(cg1_copy[1].exponent() == 5.0);
            REQUIRE(cg1_copy[2].exponent() == 6.0);
            REQUIRE(pcg1_copy == &cg1_copy);
        }

        SECTION("Move") {
            cg_type cg0_move(std::move(cg0));
            REQUIRE(cg0_move.size() == 0);
            REQUIRE(cg0_move.center() == center_type{});

            cg_type cg1_move(std::move(cg1));
            REQUIRE(cg1_move.size() == 3);
            REQUIRE(cg1_move.center() == r0);
            REQUIRE(cg1_move[0].coefficient() == 1.0);
            REQUIRE(cg1_move[1].coefficient() == 2.0);
            REQUIRE(cg1_move[2].coefficient() == 3.0);
            REQUIRE(cg1_move[0].exponent() == 4.0);
            REQUIRE(cg1_move[1].exponent() == 5.0);
            REQUIRE(cg1_move[2].exponent() == 6.0);
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
            REQUIRE(cg1_move[0].coefficient() == 1.0);
            REQUIRE(cg1_move[1].coefficient() == 2.0);
            REQUIRE(cg1_move[2].coefficient() == 3.0);
            REQUIRE(cg1_move[0].exponent() == 4.0);
            REQUIRE(cg1_move[1].exponent() == 5.0);
            REQUIRE(cg1_move[2].exponent() == 6.0);
            REQUIRE(pcg1_move == &cg1_move);
        }
    }

    SECTION("Getters/setters") {
        SECTION("center()") {
            REQUIRE(cg0.center() == center_type{});
            REQUIRE(cg1.center() == r0);

            // Is writable?
            cg0.center() = r0;
            REQUIRE(cg0.center() == r0);
        }

        SECTION("center() const") {
            REQUIRE_THROWS_AS(std::as_const(cg0).center(), std::runtime_error);
            REQUIRE(std::as_const(cg1).center() == r0);
        }

        SECTION("operator[]") {
            REQUIRE(cg1[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1[2] == prim_type(3.0, 6.0, r0));

            // Is writeable?
            cg1[0] = prim_type(4.0, 7.0, center_type());
            REQUIRE(cg1[0].coefficient() == 4.0);
            REQUIRE(cg1[0].exponent() == 7.0);
            REQUIRE(cg1.center() == center_type{});
        }

        SECTION("operator[] const") {
            const auto& const_cg1 = std::as_const(cg1);
            REQUIRE(const_cg1[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1[2] == prim_type(3.0, 6.0, r0));
        }

        SECTION("size") {
            REQUIRE(cg0.size() == 0);
            REQUIRE(cg1.size() == 3);
        }
    }

    SECTION("utility") {
        SECTION("swap") {
            cg_type cg0_copy(cg0);
            cg_type cg1_copy(cg1);

            cg0.swap(cg1);

            REQUIRE(cg0_copy == cg1);
            REQUIRE(cg1_copy == cg0);
        }

        SECTION("is_null") {
            REQUIRE(cg0.is_null());
            REQUIRE_FALSE(cg1.is_null());
        }

        SECTION("operator==") {
            // Default v default
            REQUIRE(cg0 == cg_type{});

            // Default v non-default
            REQUIRE_FALSE(cg0 == cg1);

            auto cbegin = cs.begin();
            auto cend   = cs.end();
            auto ebegin = es.begin();
            auto eend   = es.end();

            // Non-default: same value
            REQUIRE(cg1 == cg_type(cbegin, cend, ebegin, eend, r0));

            // Non-default: different coefficients
            REQUIRE_FALSE(cg1 == cg_type(ebegin, eend, ebegin, eend, r0));

            // Non-default: different exponents
            REQUIRE_FALSE(cg1 == cg_type(cbegin, cend, cbegin, cend, r0));

            // Non-default: different center
            center_type r1;
            REQUIRE_FALSE(cg1 == cg_type(cbegin, cend, ebegin, eend, r1));
        }

        SECTION("operator!=") {
            // Just negates operator== so okay to spot check
            REQUIRE(cg0 != cg1);
        }
    }
}
