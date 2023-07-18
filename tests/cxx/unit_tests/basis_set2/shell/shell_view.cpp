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
#include <chemist/basis_set2/shell/shell_traits.hpp>
#include <chemist/basis_set2/shell/shell_view.hpp>

using namespace chemist::basis_set;

template<typename LHSType, typename RHSType>
void compare_addresses(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.pure() == &rhs.pure());
    REQUIRE(&lhs.l() == &rhs.l());
    REQUIRE(&lhs.contracted_gaussian() == &rhs.contracted_gaussian());
}

TEMPLATE_TEST_CASE("ShellView", "", float, double) {
    using prim_type    = Primitive<TestType>;
    using cg_type      = ContractedGaussian<prim_type>;
    using shell_type   = Shell<cg_type>;
    using shell_traits = ShellTraits<shell_type>;
    using view_type    = ShellView<shell_type>;
    using const_view   = ShellView<const shell_type>;

    using pure_type    = typename shell_traits::pure_type;
    using l_type       = typename shell_traits::angular_momentum_type;
    using coord_type   = typename shell_traits::coord_type;
    using center_type  = typename shell_traits::center_type;
    using coeff_vector = std::vector<typename shell_traits::coefficient_type>;
    using exp_vector   = std::vector<typename shell_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type l{1};
    coord_type x{7.0}, y{8.0}, z{9.0};
    center_type r0{x, y, z};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    cg_type cg(cs.begin(), cs.end(), es.begin(), es.end(), r0);
    shell_type shell1(cart, l, cg);

    // Construct test instances
    view_type view0;
    view_type view1(shell1);
    const_view const_view0;
    const_view const_view1(shell1);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(view0.size() == 0);
            REQUIRE(view0.is_null());

            REQUIRE(const_view0.size() == 0);
            REQUIRE(const_view0.is_null());
        }
        SECTION("From Shell") {
            view_type from_shell(shell1);
            REQUIRE(from_shell.pure() == cart);
            REQUIRE(from_shell.l() == l);
            REQUIRE(from_shell.contracted_gaussian() == cg);
            //compare_addresses(from_shell, shell1);

            const_view const_from_shell(shell1);
            REQUIRE(const_from_shell.pure() == cart);
            REQUIRE(const_from_shell.l() == l);
            REQUIRE(const_from_shell.contracted_gaussian() == cg);
            //compare_addresses(const_from_shell, shell1);
        }
        SECTION("With contracted Gaussian") {
            view_type with_cg(cart, l, cg);
            REQUIRE(with_cg.pure() == cart);
            REQUIRE(with_cg.l() == l);
            REQUIRE(with_cg.contracted_gaussian() == cg);
        }
        SECTION("Copy") {
            view_type view0_copy(view0);
            REQUIRE(view0_copy.size() == 0);
            REQUIRE(view0_copy.is_null());

            view_type view1_copy(view1);
            REQUIRE(view1_copy.pure() == cart);
            REQUIRE(view1_copy.l() == l);
            REQUIRE(view1_copy.contracted_gaussian() == cg);
        }
        SECTION("Copy Assignment") {
            view_type view0_copy;
            auto pview0_copy = &(view0_copy = view0);
            REQUIRE(view0_copy.size() == 0);
            REQUIRE(view0_copy.is_null());
            REQUIRE(pview0_copy == &view0_copy);

            view_type view1_copy;
            auto pview1_copy = &(view1_copy = view1);
            REQUIRE(view1_copy.pure() == cart);
            REQUIRE(view1_copy.l() == l);
            REQUIRE(view1_copy.contracted_gaussian() == cg);
            REQUIRE(pview1_copy == &view1_copy);
        }
        SECTION("Move") {
            view_type view0_move(std::move(view0));
            REQUIRE(view0_move.size() == 0);
            REQUIRE(view0_move.is_null());
            REQUIRE(view0.is_null());

            view_type view1_move(std::move(view1));
            REQUIRE(view1_move.pure() == cart);
            REQUIRE(view1_move.l() == l);
            REQUIRE(view1_move.contracted_gaussian() == cg);
            REQUIRE(view1.is_null());
        }
        SECTION("Move Assignment") {
            view_type view0_move;
            auto pview0_move = &(view0_move = std::move(view0));
            REQUIRE(view0_move.size() == 0);
            REQUIRE(view0_move.is_null());
            REQUIRE(pview0_move == &view0_move);
            REQUIRE(view0.is_null());

            view_type view1_move;
            auto pview1_move = &(view1_move = std::move(view1));
            REQUIRE(view1_move.pure() == cart);
            REQUIRE(view1_move.l() == l);
            REQUIRE(view1_move.contracted_gaussian() == cg);
            REQUIRE(pview1_move == &view1_move);
            REQUIRE(view1.is_null());
        }
    }

    SECTION("Getters/setters") {
        SECTION("pure") {
            REQUIRE_THROWS_AS(view0.pure(), std::runtime_error);
            REQUIRE(view1.pure() == cart);
        }
        SECTION("pure const") {
            REQUIRE_THROWS_AS(std::as_const(view0).pure(), std::runtime_error);
            REQUIRE(std::as_const(view1).pure() == cart);
        }
        SECTION("l") {
            REQUIRE_THROWS_AS(view0.l(), std::runtime_error);
            REQUIRE(view1.l() == l);
        }
        SECTION("l const") {
            REQUIRE_THROWS_AS(std::as_const(view0).l(), std::runtime_error);
            REQUIRE(std::as_const(view1).l() == l);
        }
        SECTION("contracted_gaussian") {
            REQUIRE_THROWS_AS(view0.contracted_gaussian(), std::runtime_error);
            REQUIRE(view1.contracted_gaussian() == cg);
        }
        SECTION("contracted_gaussian const") {
            REQUIRE_THROWS_AS(std::as_const(view0).contracted_gaussian(),
                              std::runtime_error);
            REQUIRE(std::as_const(view1).contracted_gaussian() == cg);
        }
        SECTION("n_primitives") {
            REQUIRE(view0.n_primitives() == 0);
            REQUIRE(view1.n_primitives() == 3);
        }
        SECTION("primitive") {
            REQUIRE_THROWS_AS(view0.primitive(0), std::out_of_range);
            REQUIRE(view1.primitive(0) == cg.at(0));
            REQUIRE(view1.primitive(1) == cg.at(1));
            REQUIRE(view1.primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(view1.primitive(3), std::out_of_range);
        }
        SECTION("primitive const") {
            REQUIRE_THROWS_AS(std::as_const(view0).primitive(0),
                              std::out_of_range);
            REQUIRE(std::as_const(view1).primitive(0) == cg.at(0));
            REQUIRE(std::as_const(view1).primitive(1) == cg.at(1));
            REQUIRE(std::as_const(view1).primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(std::as_const(view1).primitive(3),
                              std::out_of_range);
        }
        SECTION("size") {
            REQUIRE(view0.size() == 0);
            REQUIRE(view1.size() == 3);
        }
    }
    SECTION("Utility") {
        SECTION("swap") {
            view_type view0_copy(view0);
            view_type view1_copy(view1);
            view0.swap(view1);
            REQUIRE(view0_copy == view1);
            REQUIRE(view1_copy == view0);
        }
        SECTION("is_null") {
            REQUIRE(view0.is_null());
            REQUIRE_FALSE(view1.is_null());
        }
        SECTION("operator==") {
            SECTION("equivalent") {
                REQUIRE(view0 == view_type{});
                REQUIRE(view1 == view_type{cart, l, cg});
            }
            SECTION("Different contracted Gaussian") {
                cg_type cg2;
                REQUIRE_FALSE(view1 == view_type{cart, l, cg2});
            }
            SECTION("Different purity") {
                REQUIRE_FALSE(view1 == view_type{pure, l, cg});
            }
            SECTION("Different angular momentum") {
                l_type zero{0};
                REQUIRE_FALSE(view1 == view_type{cart, zero, cg});
            }
        }
        SECTION("operator!=") { REQUIRE(view0 != view1); }
    }
}
