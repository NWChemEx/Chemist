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
#include <chemist/basis_set/shell/shell.hpp>
#include <chemist/basis_set/shell/shell_traits.hpp>
#include <sstream>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("Shell", "", float, double) {
    using prim_type    = Primitive<TestType>;
    using cg_type      = ContractedGaussian<prim_type>;
    using shell_type   = Shell<cg_type>;
    using shell_traits = ShellTraits<shell_type>;

    using pure_type    = typename shell_traits::pure_type;
    using l_type       = typename shell_traits::angular_momentum_type;
    using center_type  = typename shell_traits::center_type;
    using coeff_vector = std::vector<typename shell_traits::coefficient_type>;
    using exp_vector   = std::vector<typename shell_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type l{1};
    center_type r0{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    cg_type cg(cs.begin(), cs.end(), es.begin(), es.end(), r0);

    // Construct test instances
    shell_type shell0;
    shell_type shell1(cart, l, cg);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(shell0.size() == 0);
            REQUIRE(shell0.is_null());
        }
        SECTION("With contracted Gaussian") {
            shell_type with_cg(cart, l, cg);
            REQUIRE(with_cg.pure() == cart);
            REQUIRE(with_cg.l() == l);
            REQUIRE(with_cg.contracted_gaussian() == cg);
        }
        SECTION("With coeffs, exponents, and center") {
            shell_type with_cg(cart, l, cs.begin(), cs.end(), es.begin(),
                               es.end(), r0);
            REQUIRE(with_cg.pure() == cart);
            REQUIRE(with_cg.l() == l);
            REQUIRE(with_cg.contracted_gaussian() == cg);
        }
        SECTION("With coeffs, exponents, and cartesian coordinates") {
            shell_type with_cg(cart, l, cs.begin(), cs.end(), es.begin(),
                               es.end(), 7.0, 8.0, 9.0);
            REQUIRE(with_cg.pure() == cart);
            REQUIRE(with_cg.l() == l);
            REQUIRE(with_cg.contracted_gaussian() == cg);
        }
        SECTION("Copy") {
            shell_type shell0_copy(shell0);
            REQUIRE(shell0_copy.size() == 0);
            REQUIRE(shell0_copy.is_null());

            shell_type shell1_copy(shell1);
            REQUIRE(shell1_copy.pure() == cart);
            REQUIRE(shell1_copy.l() == l);
            REQUIRE(shell1_copy.contracted_gaussian() == cg);
        }
        SECTION("Copy Assignment") {
            shell_type shell0_copy;
            auto pshell0_copy = &(shell0_copy = shell0);
            REQUIRE(shell0_copy.size() == 0);
            REQUIRE(shell0_copy.is_null());
            REQUIRE(pshell0_copy == &shell0_copy);

            shell_type shell1_copy;
            auto pshell1_copy = &(shell1_copy = shell1);
            REQUIRE(shell1_copy.pure() == cart);
            REQUIRE(shell1_copy.l() == l);
            REQUIRE(shell1_copy.contracted_gaussian() == cg);
            REQUIRE(pshell1_copy == &shell1_copy);
        }
        SECTION("Move") {
            shell_type shell0_move(std::move(shell0));
            REQUIRE(shell0_move.size() == 0);
            REQUIRE(shell0_move.is_null());
            REQUIRE(shell0.is_null());

            shell_type shell1_move(std::move(shell1));
            REQUIRE(shell1_move.pure() == cart);
            REQUIRE(shell1_move.l() == l);
            REQUIRE(shell1_move.contracted_gaussian() == cg);
            REQUIRE(shell1.is_null());
        }
        SECTION("Move Assignment") {
            shell_type shell0_move;
            auto pshell0_move = &(shell0_move = std::move(shell0));
            REQUIRE(shell0_move.size() == 0);
            REQUIRE(shell0_move.is_null());
            REQUIRE(pshell0_move == &shell0_move);
            REQUIRE(shell0.is_null());

            shell_type shell1_move;
            auto pshell1_move = &(shell1_move = std::move(shell1));
            REQUIRE(shell1_move.pure() == cart);
            REQUIRE(shell1_move.l() == l);
            REQUIRE(shell1_move.contracted_gaussian() == cg);
            REQUIRE(pshell1_move == &shell1_move);
            REQUIRE(shell1.is_null());
        }
    }

    SECTION("Getters/setters") {
        SECTION("pure") {
            REQUIRE(shell0.is_null());
            REQUIRE(shell0.pure() == pure);
            REQUIRE(shell1.pure() == cart);
            REQUIRE_FALSE(shell0.is_null());
        }
        SECTION("pure const") {
            REQUIRE_THROWS_AS(std::as_const(shell0).pure(), std::runtime_error);
            REQUIRE(std::as_const(shell1).pure() == cart);
        }
        SECTION("l") {
            REQUIRE(shell0.is_null());
            REQUIRE(shell0.l() == 0);
            REQUIRE(shell1.l() == l);
            REQUIRE_FALSE(shell0.is_null());
        }
        SECTION("l const") {
            REQUIRE_THROWS_AS(std::as_const(shell0).l(), std::runtime_error);
            REQUIRE(std::as_const(shell1).l() == l);
        }
        SECTION("contracted_gaussian") {
            REQUIRE(shell0.is_null());
            REQUIRE(shell0.contracted_gaussian() == cg_type{});
            REQUIRE(shell1.contracted_gaussian() == cg);
            REQUIRE_FALSE(shell0.is_null());
        }
        SECTION("contracted_gaussian const") {
            REQUIRE_THROWS_AS(std::as_const(shell0).contracted_gaussian(),
                              std::runtime_error);
            REQUIRE(std::as_const(shell1).contracted_gaussian() == cg);
        }
        SECTION("center") {
            REQUIRE(shell0.is_null());
            REQUIRE(shell0.center() == center_type{});
            REQUIRE(shell1.center() == r0);
            REQUIRE_FALSE(shell0.is_null());
        }
        SECTION("center const") {
            REQUIRE_THROWS_AS(std::as_const(shell0).center(),
                              std::runtime_error);
            REQUIRE(std::as_const(shell1).center() == r0);
        }
        SECTION("n_primitives") {
            REQUIRE(shell0.n_primitives() == 0);
            REQUIRE(shell1.n_primitives() == 3);
        }
        SECTION("primitive") {
            REQUIRE_THROWS_AS(shell0.primitive(0), std::out_of_range);
            REQUIRE(shell1.primitive(0) == cg.at(0));
            REQUIRE(shell1.primitive(1) == cg.at(1));
            REQUIRE(shell1.primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(shell0.primitive(3), std::out_of_range);
        }
        SECTION("primitive const") {
            REQUIRE_THROWS_AS(std::as_const(shell0).primitive(0),
                              std::out_of_range);
            REQUIRE(std::as_const(shell1).primitive(0) == cg.at(0));
            REQUIRE(std::as_const(shell1).primitive(1) == cg.at(1));
            REQUIRE(std::as_const(shell1).primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(std::as_const(shell1).primitive(3),
                              std::out_of_range);
        }
        SECTION("size") {
            REQUIRE(shell0.size() == 0);
            REQUIRE(shell1.size() == 3);
        }
    }
    SECTION("Utility") {
        SECTION("swap") {
            shell_type shell0_copy(shell0);
            shell_type shell1_copy(shell1);
            shell0.swap(shell1);
            REQUIRE(shell0_copy == shell1);
            REQUIRE(shell1_copy == shell0);
        }
        SECTION("is_null") {
            REQUIRE(shell0.is_null());
            REQUIRE_FALSE(shell1.is_null());
        }
        SECTION("operator==") {
            SECTION("equivalent") {
                REQUIRE(shell0 == shell_type{});
                REQUIRE(shell1 == shell_type{cart, l, cg});
            }
            SECTION("Different contracted Gaussian") {
                REQUIRE_FALSE(shell1 == shell_type{cart, l, cg_type{}});
            }
            SECTION("Different purity") {
                REQUIRE_FALSE(shell1 == shell_type{pure, l, cg});
            }
            SECTION("Different angular momentum") {
                REQUIRE_FALSE(shell1 == shell_type{cart, 0, cg});
            }
        }
        SECTION("operator!=") { REQUIRE(shell0 != shell1); }
    }
}
