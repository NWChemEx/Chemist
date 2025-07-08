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

#include "../catch.hpp"
#include <chemist/basis_set/shell_traits.hpp>
#include <chemist/basis_set/shell_view.hpp>
#include <utility>

using namespace chemist::basis_set;

template<typename LHSType, typename RHSType>
void compare_cgs(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.center().x() == &rhs.center().x());
    REQUIRE(&lhs.center().y() == &rhs.center().y());
    REQUIRE(&lhs.center().z() == &rhs.center().z());

    for(std::size_t prim_i = 0; prim_i < lhs.size(); ++prim_i) {
        REQUIRE(&lhs[prim_i].coefficient() == &rhs[prim_i].coefficient());
        REQUIRE(&lhs[prim_i].exponent() == &rhs[prim_i].exponent());
    }
}

template<typename LHSType, typename RHSType>
void compare_addresses(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.pure() == &rhs.pure());
    REQUIRE(&lhs.l() == &rhs.l());

    auto lcg = lhs.contracted_gaussian();
    auto rcg = rhs.contracted_gaussian();
    compare_cgs(lcg, rcg);
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
    using center_type  = typename shell_traits::center_type;
    using coeff_vector = std::vector<typename shell_traits::coefficient_type>;
    using exp_vector   = std::vector<typename shell_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type zero{0}, l{1};
    center_type r0{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    cg_type cg(cs.begin(), cs.end(), es.begin(), es.end(), r0);
    shell_type shell0, shell1(cart, l, cg);

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
            compare_addresses(from_shell, shell1);

            const_view const_from_shell(shell1);
            REQUIRE(const_from_shell.pure() == cart);
            REQUIRE(const_from_shell.l() == l);
            REQUIRE(const_from_shell.contracted_gaussian() == cg);
            compare_addresses(const_from_shell, shell1);
        }
        SECTION("With contracted Gaussian") {
            view_type with_cg(cart, l, cg);
            REQUIRE(with_cg.pure() == cart);
            REQUIRE(&with_cg.pure() == &cart);
            REQUIRE(with_cg.l() == l);
            REQUIRE(&with_cg.l() == &l);
            REQUIRE(with_cg.contracted_gaussian() == cg);
            compare_cgs(with_cg.contracted_gaussian(), cg);

            const_view const_with_cg(cart, l, cg);
            REQUIRE(const_with_cg.pure() == cart);
            REQUIRE(&const_with_cg.pure() == &cart);
            REQUIRE(const_with_cg.l() == l);
            REQUIRE(&const_with_cg.l() == &l);
            REQUIRE(const_with_cg.contracted_gaussian() == cg);
            compare_cgs(const_with_cg.contracted_gaussian(), cg);

            const_view const_with_const_cg(cart, l, std::as_const(cg));
            REQUIRE(const_with_const_cg.pure() == cart);
            REQUIRE(&const_with_const_cg.pure() == &cart);
            REQUIRE(const_with_const_cg.l() == l);
            REQUIRE(&const_with_const_cg.l() == &l);
            REQUIRE(const_with_const_cg.contracted_gaussian() == cg);
            compare_cgs(const_with_const_cg.contracted_gaussian(), cg);
        }
        SECTION("Assign Shell") {
            view_type null_view;
            REQUIRE_THROWS_AS(null_view = shell1, std::runtime_error);
            REQUIRE_THROWS_AS(view1 = shell0, std::runtime_error);

            cg_type cg2(es.begin(), es.end(), cs.begin(), cs.end(), r0);
            shell_type shell2(pure, zero, cg2);

            auto pshell2_view = &(view1 = shell2);
            REQUIRE(view1.pure() == pure);
            REQUIRE(view1.l() == zero);
            REQUIRE(view1.contracted_gaussian() == cg2);
            compare_addresses(view1, shell1);
            REQUIRE(pshell2_view == &view1);
        }
        SECTION("Const from non-const") {
            const_view const_copy0(view0);
            REQUIRE(const_copy0.size() == 0);
            REQUIRE(const_copy0.is_null());

            const_view const_copy1(view1);
            REQUIRE(const_copy1.pure() == cart);
            REQUIRE(const_copy1.l() == l);
            REQUIRE(const_copy1.contracted_gaussian() == cg);
            compare_addresses(const_copy1, shell1);
        }
        SECTION("Copy") {
            view_type view0_copy(view0);
            REQUIRE(view0_copy.size() == 0);
            REQUIRE(view0_copy.is_null());

            const_view const_view0_copy(const_view0);
            REQUIRE(const_view0_copy.size() == 0);
            REQUIRE(const_view0_copy.is_null());

            view_type view1_copy(view1);
            REQUIRE(view1_copy.pure() == cart);
            REQUIRE(view1_copy.l() == l);
            REQUIRE(view1_copy.contracted_gaussian() == cg);
            compare_addresses(view1_copy, shell1);

            const_view const_view1_copy(const_view1);
            REQUIRE(const_view1_copy.pure() == cart);
            REQUIRE(const_view1_copy.l() == l);
            REQUIRE(const_view1_copy.contracted_gaussian() == cg);
            compare_addresses(const_view1_copy, shell1);
        }
        SECTION("Copy Assignment") {
            view_type view0_copy;
            auto pview0_copy = &(view0_copy = view0);
            REQUIRE(view0_copy.size() == 0);
            REQUIRE(view0_copy.is_null());
            REQUIRE(pview0_copy == &view0_copy);

            const_view const_view0_copy;
            auto pconst_view0_copy = &(const_view0_copy = const_view0);
            REQUIRE(const_view0_copy.size() == 0);
            REQUIRE(const_view0_copy.is_null());
            REQUIRE(pconst_view0_copy == &const_view0_copy);

            view_type view1_copy;
            auto pview1_copy = &(view1_copy = view1);
            REQUIRE(view1_copy.pure() == cart);
            REQUIRE(view1_copy.l() == l);
            REQUIRE(view1_copy.contracted_gaussian() == cg);
            REQUIRE(pview1_copy == &view1_copy);
            compare_addresses(view1_copy, shell1);

            const_view const_view1_copy;
            auto pconst_view1_copy = &(const_view1_copy = const_view1);
            REQUIRE(const_view1_copy.pure() == cart);
            REQUIRE(const_view1_copy.l() == l);
            REQUIRE(const_view1_copy.contracted_gaussian() == cg);
            REQUIRE(pconst_view1_copy == &const_view1_copy);
            compare_addresses(const_view1_copy, shell1);
        }
        SECTION("Move") {
            view_type view0_move(std::move(view0));
            REQUIRE(view0_move.size() == 0);
            REQUIRE(view0_move.is_null());
            REQUIRE(view0.is_null());

            const_view const_view0_move(std::move(const_view0));
            REQUIRE(const_view0_move.size() == 0);
            REQUIRE(const_view0_move.is_null());
            REQUIRE(const_view0.is_null());

            view_type view1_move(std::move(view1));
            REQUIRE(view1_move.pure() == cart);
            REQUIRE(view1_move.l() == l);
            REQUIRE(view1_move.contracted_gaussian() == cg);
            REQUIRE(view1.is_null());
            compare_addresses(view1_move, shell1);

            const_view const_view1_move(std::move(const_view1));
            REQUIRE(const_view1_move.pure() == cart);
            REQUIRE(const_view1_move.l() == l);
            REQUIRE(const_view1_move.contracted_gaussian() == cg);
            REQUIRE(const_view1.is_null());
            compare_addresses(const_view1_move, shell1);
        }
        SECTION("Move Assignment") {
            view_type view0_move;
            auto pview0_move = &(view0_move = std::move(view0));
            REQUIRE(view0_move.size() == 0);
            REQUIRE(view0_move.is_null());
            REQUIRE(pview0_move == &view0_move);
            REQUIRE(view0.is_null());

            const_view const_view0_move;
            auto pconst_view0_move =
              &(const_view0_move = std::move(const_view0));
            REQUIRE(const_view0_move.size() == 0);
            REQUIRE(const_view0_move.is_null());
            REQUIRE(pconst_view0_move == &const_view0_move);
            REQUIRE(const_view0.is_null());

            view_type view1_move;
            auto pview1_move = &(view1_move = std::move(view1));
            REQUIRE(view1_move.pure() == cart);
            REQUIRE(view1_move.l() == l);
            REQUIRE(view1_move.contracted_gaussian() == cg);
            REQUIRE(pview1_move == &view1_move);
            REQUIRE(view1.is_null());
            compare_addresses(view1_move, shell1);

            const_view const_view1_move;
            auto pconst_view1_move =
              &(const_view1_move = std::move(const_view1));
            REQUIRE(const_view1_move.pure() == cart);
            REQUIRE(const_view1_move.l() == l);
            REQUIRE(const_view1_move.contracted_gaussian() == cg);
            REQUIRE(pconst_view1_move == &const_view1_move);
            REQUIRE(const_view1.is_null());
            compare_addresses(const_view1_move, shell1);
        }
    }

    SECTION("Getters/setters") {
        SECTION("pure") {
            REQUIRE_THROWS_AS(view0.pure(), std::runtime_error);
            REQUIRE_THROWS_AS(const_view0.pure(), std::runtime_error);
            REQUIRE(view1.pure() == cart);
            REQUIRE(const_view1.pure() == cart);
        }
        SECTION("pure const") {
            REQUIRE_THROWS_AS(std::as_const(view0).pure(), std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(const_view0).pure(),
                              std::runtime_error);
            REQUIRE(std::as_const(view1).pure() == cart);
            REQUIRE(std::as_const(const_view1).pure() == cart);
        }
        SECTION("l") {
            REQUIRE_THROWS_AS(view0.l(), std::runtime_error);
            REQUIRE_THROWS_AS(const_view0.l(), std::runtime_error);
            REQUIRE(view1.l() == l);
            REQUIRE(const_view1.l() == l);
        }
        SECTION("l const") {
            REQUIRE_THROWS_AS(std::as_const(view0).l(), std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(const_view0).l(),
                              std::runtime_error);
            REQUIRE(std::as_const(view1).l() == l);
            REQUIRE(std::as_const(const_view1).l() == l);
        }
        SECTION("contracted_gaussian") {
            REQUIRE_THROWS_AS(view0.contracted_gaussian(), std::runtime_error);
            REQUIRE_THROWS_AS(const_view0.contracted_gaussian(),
                              std::runtime_error);
            REQUIRE(view1.contracted_gaussian() == cg);
            REQUIRE(const_view1.contracted_gaussian() == cg);
        }
        SECTION("contracted_gaussian const") {
            REQUIRE_THROWS_AS(std::as_const(view0).contracted_gaussian(),
                              std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(const_view0).contracted_gaussian(),
                              std::runtime_error);
            REQUIRE(std::as_const(view1).contracted_gaussian() == cg);
            REQUIRE(std::as_const(const_view1).contracted_gaussian() == cg);
        }
        SECTION("center") {
            REQUIRE_THROWS_AS(view0.center(), std::runtime_error);
            REQUIRE_THROWS_AS(const_view0.center(), std::runtime_error);
            REQUIRE(view1.center() == r0);
            REQUIRE(const_view1.center() == r0);
        }
        SECTION("center const") {
            REQUIRE_THROWS_AS(std::as_const(view0).center(),
                              std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(const_view0).center(),
                              std::runtime_error);
            REQUIRE(std::as_const(view1).center() == r0);
            REQUIRE(std::as_const(const_view1).center() == r0);
        }
        SECTION("n_primitives") {
            REQUIRE(view0.n_primitives() == 0);
            REQUIRE(view0.n_primitives() == 0);
            REQUIRE(view1.n_primitives() == 3);
            REQUIRE(view1.n_primitives() == 3);
        }
        SECTION("primitive") {
            REQUIRE_THROWS_AS(view0.primitive(0), std::out_of_range);
            REQUIRE_THROWS_AS(const_view0.primitive(0), std::out_of_range);
            REQUIRE(view1.primitive(0) == cg.at(0));
            REQUIRE(view1.primitive(1) == cg.at(1));
            REQUIRE(view1.primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(const_view1.primitive(3), std::out_of_range);
            REQUIRE(const_view1.primitive(0) == cg.at(0));
            REQUIRE(const_view1.primitive(1) == cg.at(1));
            REQUIRE(const_view1.primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(const_view1.primitive(3), std::out_of_range);
        }
        SECTION("primitive const") {
            REQUIRE_THROWS_AS(std::as_const(view0).primitive(0),
                              std::out_of_range);
            REQUIRE_THROWS_AS(std::as_const(const_view0).primitive(0),
                              std::out_of_range);
            REQUIRE(std::as_const(view1).primitive(0) == cg.at(0));
            REQUIRE(std::as_const(view1).primitive(1) == cg.at(1));
            REQUIRE(std::as_const(view1).primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(std::as_const(view1).primitive(3),
                              std::out_of_range);
            REQUIRE(std::as_const(const_view1).primitive(0) == cg.at(0));
            REQUIRE(std::as_const(const_view1).primitive(1) == cg.at(1));
            REQUIRE(std::as_const(const_view1).primitive(2) == cg.at(2));
            REQUIRE_THROWS_AS(std::as_const(const_view1).primitive(3),
                              std::out_of_range);
        }
        SECTION("size") {
            REQUIRE(view0.size() == 0);
            REQUIRE(const_view0.size() == 0);
            REQUIRE(view1.size() == 3);
            REQUIRE(const_view1.size() == 3);
        }
    }
    SECTION("Utility") {
        SECTION("swap") {
            view_type view0_copy(view0);
            view_type view1_copy(view1);
            view0.swap(view1);
            REQUIRE(view0_copy == view1);
            REQUIRE(view1_copy == view0);

            const_view const_view0_copy(const_view0);
            const_view const_view1_copy(const_view1);
            const_view0.swap(const_view1);
            REQUIRE(const_view0_copy == const_view1);
            REQUIRE(const_view1_copy == const_view0);
        }
        SECTION("is_null") {
            REQUIRE(view0.is_null());
            REQUIRE(const_view0.is_null());
            REQUIRE_FALSE(view1.is_null());
            REQUIRE_FALSE(const_view1.is_null());
        }
        SECTION("operator==(ShellView)") {
            SECTION("equivalent") {
                REQUIRE(view0 == view_type{});
                REQUIRE(const_view0 == const_view{});
                REQUIRE(view1 == view_type{cart, l, cg});
                REQUIRE(const_view1 == const_view{cart, l, cg});
            }
            SECTION("Different contracted Gaussian") {
                cg_type cg2;
                REQUIRE_FALSE(view1 == view_type{cart, l, cg2});
                REQUIRE_FALSE(const_view1 == const_view{cart, l, cg2});
            }
            SECTION("Different purity") {
                REQUIRE_FALSE(view1 == view_type{pure, l, cg});
                REQUIRE_FALSE(const_view1 == const_view{pure, l, cg});
            }
            SECTION("Different angular momentum") {
                REQUIRE_FALSE(view1 == view_type{cart, zero, cg});
                REQUIRE_FALSE(const_view1 == const_view{cart, zero, cg});
            }
        }
        SECTION("operator==(Shell)") {
            SECTION("equivalent") {
                REQUIRE(view0 == shell0);
                REQUIRE(const_view0 == shell0);
                REQUIRE(view1 == shell1);
                REQUIRE(const_view1 == shell1);
                // Check symmetry
                REQUIRE(shell0 == view0);
                REQUIRE(shell0 == const_view0);
                REQUIRE(shell1 == view1);
                REQUIRE(shell1 == const_view1);
            }
            SECTION("Different contracted Gaussian") {
                cg_type cg2;
                REQUIRE_FALSE(shell1 == view_type{cart, l, cg2});
                REQUIRE_FALSE(shell1 == const_view{cart, l, cg2});
            }
            SECTION("Different purity") {
                REQUIRE_FALSE(shell1 == view_type{pure, l, cg});
                REQUIRE_FALSE(shell1 == const_view{pure, l, cg});
            }
            SECTION("Different angular momentum") {
                REQUIRE_FALSE(shell1 == view_type{cart, zero, cg});
                REQUIRE_FALSE(shell1 == const_view{cart, zero, cg});
            }
        }
        SECTION("operator!=(ShellView)") {
            REQUIRE(view0 != view1);
            REQUIRE(const_view0 != const_view1);
        }
        SECTION("operator!=(Shell)") {
            REQUIRE(view0 != shell1);
            REQUIRE(const_view0 != shell1);
        }
    }
}
