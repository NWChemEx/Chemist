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
#include <chemist/basis_set2/atomic_basis_set/detail_/atomic_basis_set_view_pimpl.hpp>
#include <utility>

using namespace chemist::basis_set;
using namespace chemist::basis_set::detail_;

TEMPLATE_TEST_CASE("AtomicBasisSetViewPIMPL", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using shell_type = Shell<cg_type>;
    using abs_type   = AtomicBasisSet<shell_type>;
    using abs_traits = AtomicBasisSetTraits<abs_type>;
    using pimpl_type = AtomicBasisSetViewPIMPL<abs_type>;

    using pure_type          = typename abs_traits::pure_type;
    using l_type             = typename abs_traits::angular_momentum_type;
    using center_type        = typename abs_traits::center_type;
    using name_type          = typename abs_traits::name_type;
    using atomic_number_type = typename abs_traits::atomic_number_type;
    using range_type         = typename abs_traits::range_type;
    using shell_ref_type     = typename abs_type::reference;
    using coeff_vector     = std::vector<typename abs_traits::coefficient_type>;
    using exp_vector       = std::vector<typename abs_traits::exponent_type>;
    using shell_ref_vector = std::vector<shell_ref_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type l0{0}, l1{1};
    center_type r0, r1{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    prim_type p0{cs[0], es[0], r1}, p1{cs[1], es[1], r1}, p2{cs[2], es[2], r1};
    cg_type cg0(cs.begin(), cs.begin() + 1, es.begin(), es.begin() + 1, r1);
    cg_type cg1(cs.begin(), cs.end(), es.begin(), es.end(), r1);
    name_type name0, name1{"name1"};
    atomic_number_type z0{0}, z1{1};
    shell_type shell1{cart, l1, cg1};
    abs_type abs1(name1, z1, r1);
    abs1.add_shell(cart, l1, cg1);
    abs1.add_shell(pure, l0, cg0);

    pimpl_type pimpl1{abs1.basis_set_name(), abs1.atomic_number(),
                      abs1.center(),
                      shell_ref_vector{abs1.begin(), abs1.end()}};

    SECTION("Ctor") {
        REQUIRE(pimpl1.basis_set_name() == abs1.basis_set_name());
        REQUIRE(pimpl1.atomic_number() == abs1.atomic_number());
        REQUIRE(pimpl1.center() == abs1.center());
        REQUIRE(pimpl1.size() == abs1.size());
        REQUIRE(pimpl1[0] == abs1[0]);
    }
    SECTION("Getters/setters") {
        SECTION("basis_set_name") { REQUIRE(pimpl1.basis_set_name() == name1); }
        SECTION("basis_set_name const") {
            REQUIRE(std::as_const(pimpl1).basis_set_name() == name1);
        }
        SECTION("atomic_number") { REQUIRE(pimpl1.atomic_number() == z1); }
        SECTION("atomic_number const") {
            REQUIRE(std::as_const(pimpl1).atomic_number() == z1);
        }
        SECTION("center") { REQUIRE(pimpl1.center() == r1); }
        SECTION("center const") {
            REQUIRE(std::as_const(pimpl1).center() == r1);
        }
        SECTION("pure") { REQUIRE(pimpl1.pure(0) == cart); }
        SECTION("pure const") {
            REQUIRE(std::as_const(pimpl1).pure(0) == cart);
        }
        SECTION("l") { REQUIRE(pimpl1.l(0) == l1); }
        SECTION("l const") { REQUIRE(std::as_const(pimpl1).l(0) == l1); }
        SECTION("size") { REQUIRE(pimpl1.size() == 2); }
        SECTION("n_aos") { REQUIRE(pimpl1.n_aos() == 4); }
        SECTION("n_primitives") { REQUIRE(pimpl1.n_primitives() == 4); }
        SECTION("primitive_range") {
            REQUIRE(pimpl1.primitive_range(0) == range_type{0, 3});
        }
        SECTION("primitive_to_shell") {
            REQUIRE(pimpl1.primitive_to_shell(0) == 0);
            REQUIRE(pimpl1.primitive_to_shell(1) == 0);
            REQUIRE(pimpl1.primitive_to_shell(2) == 0);
            REQUIRE(pimpl1.primitive_to_shell(3) == 1);
            REQUIRE_THROWS_AS(pimpl1.primitive_to_shell(4), std::runtime_error);
        }
        SECTION("operator[]") { REQUIRE(pimpl1[0] == shell1); }
        SECTION("operator[] const") {
            REQUIRE(std::as_const(pimpl1)[0] == shell1);
        }
        SECTION("cg") { REQUIRE(pimpl1.cg(0) == cg1); }
        SECTION("cg const") { REQUIRE(std::as_const(pimpl1).cg(0) == cg1); }
        SECTION("primitive") { REQUIRE(pimpl1.primitive(0) == p0); }
        SECTION("primitive const") {
            REQUIRE(std::as_const(pimpl1).primitive(0) == p0);
        }
    }
    SECTION("Utility") {
        SECTION("operator==") {
            SECTION("are equal") {
                pimpl_type pimpl2{abs1.basis_set_name(), abs1.atomic_number(),
                                  abs1.center(),
                                  shell_ref_vector{abs1.begin(), abs1.end()}};
                REQUIRE(pimpl1 == pimpl2);
            }
            SECTION("Different name") {
                pimpl_type pimpl2{name0, abs1.atomic_number(), abs1.center(),
                                  shell_ref_vector{abs1.begin(), abs1.end()}};
                REQUIRE_FALSE(pimpl1 == pimpl2);
            }
            SECTION("Different z") {
                pimpl_type pimpl2{abs1.basis_set_name(), z0, abs1.center(),
                                  shell_ref_vector{abs1.begin(), abs1.end()}};
                REQUIRE_FALSE(pimpl1 == pimpl2);
            }
            SECTION("Different center") {
                pimpl_type pimpl2{abs1.basis_set_name(), abs1.atomic_number(),
                                  r0,
                                  shell_ref_vector{abs1.begin(), abs1.end()}};
                REQUIRE_FALSE(pimpl1 == pimpl2);
            }
            SECTION("Different shells") {
                pimpl_type pimpl2{
                  abs1.basis_set_name(), abs1.atomic_number(), abs1.center(),
                  shell_ref_vector{abs1.begin(), abs1.begin() + 1}};
                REQUIRE_FALSE(pimpl1 == pimpl2);
            }
        }
    }
}