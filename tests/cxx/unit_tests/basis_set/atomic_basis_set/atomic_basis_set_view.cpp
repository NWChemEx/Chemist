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
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_traits.hpp>
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_view.hpp>
#include <utility>

using namespace chemist::basis_set;

template<typename LHSType, typename RHSType>
void compare_addresses(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.basis_set_name() == &rhs.basis_set_name());
    REQUIRE(&lhs.atomic_number() == &rhs.atomic_number());
    REQUIRE(&lhs.center().x() == &rhs.center().x());
    REQUIRE(&lhs.center().y() == &rhs.center().y());
    REQUIRE(&lhs.center().z() == &rhs.center().z());

    for(decltype(lhs.size()) i = 0; i < lhs.size(); ++i) {
        REQUIRE(&lhs[i].pure() == &rhs[i].pure());
        REQUIRE(&lhs[i].l() == &rhs[i].l());
    }

    for(decltype(lhs.n_primitives()) i = 0; i < lhs.n_primitives(); ++i) {
        REQUIRE(&lhs.primitive(i).exponent() == &rhs.primitive(i).exponent());
        REQUIRE(&lhs.primitive(i).coefficient() ==
                &rhs.primitive(i).coefficient());
    }
}

TEMPLATE_TEST_CASE("AtomicBasisSetView", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using shell_type = Shell<cg_type>;
    using abs_type   = AtomicBasisSet<shell_type>;
    using abs_traits = AtomicBasisSetTraits<abs_type>;
    using view_type  = AtomicBasisSetView<abs_type>;
    using const_view = AtomicBasisSetView<const abs_type>;

    using pure_type          = typename abs_traits::pure_type;
    using l_type             = typename abs_traits::angular_momentum_type;
    using center_type        = typename abs_traits::center_type;
    using name_type          = typename abs_traits::name_type;
    using atomic_number_type = typename abs_traits::atomic_number_type;
    using range_type         = typename abs_traits::range_type;
    using coeff_vector = std::vector<typename abs_traits::coefficient_type>;
    using exp_vector   = std::vector<typename abs_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type l0{0}, l1{1};
    center_type r0, r1{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    prim_type p0{cs[0], es[0], r1};
    cg_type cg0(es.begin(), es.end(), cs.begin(), cs.end(), r1);
    cg_type cg1(cs.begin(), cs.end(), es.begin(), es.end(), r1);
    name_type name0, name1{"name1"};
    atomic_number_type z0{0}, z1{1};
    shell_type shell0{cart, l1, cg1};
    abs_type abs0, abs1(name1, z1, r1);
    abs1.add_shell(cart, l1, cg1);

    view_type view0;
    const_view cview0;
    view_type view1(abs1);
    const_view cview1(abs1);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(view0.is_null());
            REQUIRE(view0.size() == 0);

            REQUIRE(cview0.is_null());
            REQUIRE(cview0.size() == 0);
        }
        SECTION("Copy") {
            view_type copy_view0(view0);
            REQUIRE(copy_view0.is_null());
            REQUIRE(copy_view0.size() == 0);

            const_view copy_cview0(cview0);
            REQUIRE(copy_cview0.is_null());
            REQUIRE(copy_cview0.size() == 0);

            view_type copy_view1(view1);
            REQUIRE(copy_view1.size() == 1);
            REQUIRE(copy_view1.basis_set_name() == name1);
            REQUIRE(copy_view1.atomic_number() == z1);
            REQUIRE(copy_view1.center() == r1);
            REQUIRE(copy_view1[0] == shell0);
            compare_addresses(copy_view1, abs1);

            const_view copy_cview1(cview1);
            REQUIRE(copy_cview1.size() == 1);
            REQUIRE(copy_cview1.basis_set_name() == name1);
            REQUIRE(copy_cview1.atomic_number() == z1);
            REQUIRE(copy_cview1.center() == r1);
            REQUIRE(copy_cview1[0] == shell0);
            compare_addresses(copy_cview1, abs1);
        }
        SECTION("Copy Assignment") {
            view_type copy_view0;
            auto pcopy_view0 = &(copy_view0 = view0);
            REQUIRE(copy_view0.is_null());
            REQUIRE(copy_view0.size() == 0);
            REQUIRE(pcopy_view0 == &copy_view0);

            const_view copy_cview0;
            auto pcopy_cview0 = &(copy_cview0 = cview0);
            REQUIRE(copy_cview0.is_null());
            REQUIRE(copy_cview0.size() == 0);
            REQUIRE(pcopy_cview0 == &copy_cview0);

            view_type copy_view1;
            auto pcopy_view1 = &(copy_view1 = view1);
            REQUIRE(copy_view1.size() == 1);
            REQUIRE(copy_view1.basis_set_name() == name1);
            REQUIRE(copy_view1.atomic_number() == z1);
            REQUIRE(copy_view1.center() == r1);
            REQUIRE(copy_view1[0] == shell0);
            compare_addresses(copy_view1, abs1);
            REQUIRE(pcopy_view1 == &copy_view1);

            const_view copy_cview1;
            auto pcopy_cview1 = &(copy_cview1 = cview1);
            REQUIRE(copy_cview1.size() == 1);
            REQUIRE(copy_cview1.basis_set_name() == name1);
            REQUIRE(copy_cview1.atomic_number() == z1);
            REQUIRE(copy_cview1.center() == r1);
            REQUIRE(copy_cview1[0] == shell0);
            compare_addresses(copy_cview1, abs1);
            REQUIRE(pcopy_cview1 == &copy_cview1);
        }
        SECTION("Move") {
            view_type move_view0(std::move(view0));
            REQUIRE(move_view0.is_null());
            REQUIRE(move_view0.size() == 0);

            const_view move_cview0(std::move(cview0));
            REQUIRE(move_cview0.is_null());
            REQUIRE(move_cview0.size() == 0);

            view_type move_view1(std::move(view1));
            REQUIRE(move_view1.size() == 1);
            REQUIRE(move_view1.basis_set_name() == name1);
            REQUIRE(move_view1.atomic_number() == z1);
            REQUIRE(move_view1.center() == r1);
            REQUIRE(move_view1[0] == shell0);
            compare_addresses(move_view1, abs1);
            REQUIRE(view1.is_null());

            const_view move_cview1(std::move(cview1));
            REQUIRE(move_cview1.size() == 1);
            REQUIRE(move_cview1.basis_set_name() == name1);
            REQUIRE(move_cview1.atomic_number() == z1);
            REQUIRE(move_cview1.center() == r1);
            REQUIRE(move_cview1[0] == shell0);
            compare_addresses(move_cview1, abs1);
            REQUIRE(cview1.is_null());
        }
        SECTION("Move Assignment") {
            view_type move_view0;
            auto pmove_view0 = &(move_view0 = std::move(view0));
            REQUIRE(move_view0.is_null());
            REQUIRE(move_view0.size() == 0);
            REQUIRE(pmove_view0 == &move_view0);

            const_view move_cview0;
            auto pmove_cview0 = &(move_cview0 = std::move(cview0));
            REQUIRE(move_cview0.is_null());
            REQUIRE(move_cview0.size() == 0);
            REQUIRE(pmove_cview0 == &move_cview0);

            view_type move_view1;
            auto pmove_view1 = &(move_view1 = std::move(view1));
            REQUIRE(move_view1.size() == 1);
            REQUIRE(move_view1.basis_set_name() == name1);
            REQUIRE(move_view1.atomic_number() == z1);
            REQUIRE(move_view1.center() == r1);
            REQUIRE(move_view1[0] == shell0);
            compare_addresses(move_view1, abs1);
            REQUIRE(pmove_view1 == &move_view1);
            REQUIRE(view1.is_null());

            const_view move_cview1;
            auto pmove_cview1 = &(move_cview1 = std::move(cview1));
            REQUIRE(move_cview1.size() == 1);
            REQUIRE(move_cview1.basis_set_name() == name1);
            REQUIRE(move_cview1.atomic_number() == z1);
            REQUIRE(move_cview1.center() == r1);
            REQUIRE(move_cview1[0] == shell0);
            compare_addresses(move_cview1, abs1);
            REQUIRE(pmove_cview1 == &move_cview1);
            REQUIRE(cview1.is_null());
        }
        SECTION("From input values") {
            view_type from_values(abs1.basis_set_name(), abs1.atomic_number(),
                                  abs1.center(),
                                  std::vector<typename view_type::reference>{
                                    abs1.begin(), abs1.end()});
            REQUIRE(from_values.size() == 1);
            REQUIRE(from_values.basis_set_name() == name1);
            REQUIRE(from_values.atomic_number() == z1);
            REQUIRE(from_values.center() == r1);
            REQUIRE(from_values[0] == shell0);
            compare_addresses(from_values, abs1);

            const_view cfrom_values(abs1.basis_set_name(), abs1.atomic_number(),
                                    abs1.center(),
                                    std::vector<typename const_view::reference>{
                                      abs1.begin(), abs1.end()});
            REQUIRE(cfrom_values.size() == 1);
            REQUIRE(cfrom_values.basis_set_name() == name1);
            REQUIRE(cfrom_values.atomic_number() == z1);
            REQUIRE(cfrom_values.center() == r1);
            REQUIRE(cfrom_values[0] == shell0);
            compare_addresses(cfrom_values, abs1);
        }
        SECTION("From AtomicBasisSet") {
            view_type from_abs(abs1);
            REQUIRE(from_abs.size() == 1);
            REQUIRE(from_abs.basis_set_name() == name1);
            REQUIRE(from_abs.atomic_number() == z1);
            REQUIRE(from_abs.center() == r1);
            REQUIRE(from_abs[0] == shell0);
            compare_addresses(from_abs, abs1);

            const_view cfrom_abs(abs1);
            REQUIRE(cfrom_abs.size() == 1);
            REQUIRE(cfrom_abs.basis_set_name() == name1);
            REQUIRE(cfrom_abs.atomic_number() == z1);
            REQUIRE(cfrom_abs.center() == r1);
            REQUIRE(cfrom_abs[0] == shell0);
            compare_addresses(cfrom_abs, abs1);
        }
        SECTION("Const from non-const") {
            const_view const_copy0(view0);
            REQUIRE(const_copy0.is_null());
            REQUIRE(const_copy0.size() == 0);

            const_view const_copy1(cview1);
            REQUIRE(const_copy1.size() == 1);
            REQUIRE(const_copy1.basis_set_name() == name1);
            REQUIRE(const_copy1.atomic_number() == z1);
            REQUIRE(const_copy1.center() == r1);
            REQUIRE(const_copy1[0] == shell0);
            compare_addresses(const_copy1, abs1);
        }
        SECTION("Assign AtomicBasisSet") {
            view_type null_view;
            REQUIRE_THROWS_AS(null_view = abs1, std::runtime_error);
            REQUIRE_THROWS_AS(view1 = abs0, std::runtime_error);

            abs_type diff_abs(name0, z0, r0);
            view_type diff_view1(diff_abs);
            REQUIRE_THROWS_AS(diff_view1 = abs1, std::runtime_error);

            diff_abs.add_shell(pure, l0, cg0);
            view_type diff_view2(diff_abs);
            auto pdiff_view2 = &(diff_view2 = abs1);
            REQUIRE(diff_view2.size() == 1);
            REQUIRE(diff_view2.basis_set_name() == name1);
            REQUIRE(diff_view2.atomic_number() == z1);
            REQUIRE(diff_view2.center() == r1);
            REQUIRE(diff_view2[0] == shell0);
            compare_addresses(diff_view2, diff_abs);
            REQUIRE(pdiff_view2 == &diff_view2);
        }
    }
    SECTION("Getters/setters") {
        SECTION("basis_set_name") {
            REQUIRE_THROWS_AS(view0.basis_set_name(), std::runtime_error);
            REQUIRE_THROWS_AS(cview0.basis_set_name(), std::runtime_error);

            REQUIRE(view1.basis_set_name() == name1);
            REQUIRE(cview1.basis_set_name() == name1);
        }
        SECTION("basis_set_name const") {
            REQUIRE_THROWS_AS(std::as_const(view0).basis_set_name(),
                              std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(cview0).basis_set_name(),
                              std::runtime_error);

            REQUIRE(std::as_const(view1).basis_set_name() == name1);
            REQUIRE(std::as_const(cview1).basis_set_name() == name1);
        }
        SECTION("atomic_number") {
            REQUIRE_THROWS_AS(view0.atomic_number(), std::runtime_error);
            REQUIRE_THROWS_AS(cview0.atomic_number(), std::runtime_error);

            REQUIRE(view1.atomic_number() == z1);
            REQUIRE(cview1.atomic_number() == z1);
        }
        SECTION("atomic_number const") {
            REQUIRE_THROWS_AS(std::as_const(view0).atomic_number(),
                              std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(cview0).atomic_number(),
                              std::runtime_error);

            REQUIRE(std::as_const(view1).atomic_number() == z1);
            REQUIRE(std::as_const(cview1).atomic_number() == z1);
        }
        SECTION("center") {
            REQUIRE_THROWS_AS(view0.center(), std::runtime_error);
            REQUIRE_THROWS_AS(cview0.center(), std::runtime_error);

            REQUIRE(view1.center() == r1);
            REQUIRE(cview1.center() == r1);
        }
        SECTION("center const") {
            REQUIRE_THROWS_AS(std::as_const(view0).center(),
                              std::runtime_error);
            REQUIRE_THROWS_AS(std::as_const(cview0).center(),
                              std::runtime_error);

            REQUIRE(std::as_const(view1).center() == r1);
            REQUIRE(std::as_const(cview1).center() == r1);
        }
        SECTION("n_aos") {
            REQUIRE(view0.n_aos() == 0);
            REQUIRE(cview0.n_aos() == 0);

            REQUIRE(view1.n_aos() == 3);
            REQUIRE(cview1.n_aos() == 3);
        }
        SECTION("n_primitives") {
            REQUIRE(view0.n_primitives() == 0);
            REQUIRE(cview0.n_primitives() == 0);

            REQUIRE(view1.n_primitives() == 3);
            REQUIRE(cview1.n_primitives() == 3);
        }
        SECTION("primitive_range") {
            REQUIRE_THROWS_AS(view0.primitive_range(0), std::out_of_range);
            REQUIRE_THROWS_AS(cview0.primitive_range(0), std::out_of_range);

            REQUIRE(view1.primitive_range(0) == range_type{0, 3});
            REQUIRE(cview1.primitive_range(0) == range_type{0, 3});
        }
        SECTION("primitive_to_shell") {
            REQUIRE_THROWS_AS(view0.primitive_to_shell(0), std::out_of_range);
            REQUIRE_THROWS_AS(cview0.primitive_to_shell(0), std::out_of_range);

            REQUIRE(view1.primitive_to_shell(0) == 0);
            REQUIRE(cview1.primitive_to_shell(0) == 0);
        }
        SECTION("primitive") {
            REQUIRE_THROWS_AS(view0.primitive(0), std::out_of_range);
            REQUIRE_THROWS_AS(cview0.primitive(0), std::out_of_range);

            REQUIRE(view1.primitive(0) == p0);
            REQUIRE(cview1.primitive(0) == p0);
        }
        SECTION("primitive const") {
            REQUIRE_THROWS_AS(std::as_const(view0).primitive(0),
                              std::out_of_range);
            REQUIRE_THROWS_AS(std::as_const(cview0).primitive(0),
                              std::out_of_range);

            REQUIRE(std::as_const(view1).primitive(0) == p0);
            REQUIRE(std::as_const(cview1).primitive(0) == p0);
        }
    }
    SECTION("Utility") {
        SECTION("swap") {
            view_type view0_copy(view0);
            view_type view1_copy(view1);
            view0.swap(view1);
            REQUIRE(view0_copy == view1);
            REQUIRE(view1_copy == view0);

            const_view cview0_copy(cview0);
            const_view cview1_copy(cview1);
            cview0.swap(cview1);
            REQUIRE(cview0_copy == cview1);
            REQUIRE(cview1_copy == cview0);
        }
        SECTION("is_null") {
            REQUIRE(view0.is_null());
            REQUIRE(cview0.is_null());
            REQUIRE_FALSE(view1.is_null());
            REQUIRE_FALSE(cview1.is_null());
        }
        SECTION("operator==(AtomicBasisSetView)") {
            SECTION("equivalent") {
                REQUIRE(view0 == view_type{});
                REQUIRE(view1 == view_type{abs1});
            }
            SECTION("null and non-null") { REQUIRE_FALSE(view0 == view1); }
        }
        SECTION("operator==(AtomicBasisSet)") {
            SECTION("equivalent") {
                REQUIRE(view0 == abs_type{});
                REQUIRE(view1 == abs1);
            }
            SECTION("null and non-null") {
                REQUIRE_FALSE(view0 == abs1);
                REQUIRE_FALSE(view1 == abs_type{});
            }
            SECTION("Different size") {
                abs_type abs2(name1, z1, r1);
                abs2.add_shell(cart, l1, cg1);
                abs2.add_shell(cart, l1, cg1);
                REQUIRE_FALSE(view1 == abs2);
            }
            SECTION("Different basis set name") {
                abs_type abs2(name0, z1, r1);
                abs2.add_shell(cart, l1, cg1);
                REQUIRE_FALSE(view1 == abs2);
            }
            SECTION("Different atomic number") {
                abs_type abs2(name1, z0, r1);
                abs2.add_shell(cart, l1, cg1);
                REQUIRE_FALSE(view1 == abs2);
            }
            SECTION("Different center") {
                abs_type abs2(name1, z1, r0);
                abs2.add_shell(cart, l1, cg1);
                REQUIRE_FALSE(view1 == abs2);
            }
            SECTION("Different shells") {
                abs_type abs2(name1, z1, r1);
                abs2.add_shell(pure, l1, cg1);
                REQUIRE_FALSE(view1 == abs2);

                abs_type abs3(name1, z1, r1);
                abs2.add_shell(cart, l0, cg1);
                REQUIRE_FALSE(view1 == abs3);

                abs_type abs4(name1, z1, r1);
                abs2.add_shell(cart, l1, cg0);
                REQUIRE_FALSE(view1 == abs4);
            }
        }
        SECTION("operator!=(AtomicBasisSetView)") {
            REQUIRE(view0 != view1);
            REQUIRE(cview0 != cview1);
        }
        SECTION("operator!=(AtomicBasisSet)") {
            REQUIRE(view0 != abs1);
            REQUIRE(cview0 != abs1);
        }
    }
}