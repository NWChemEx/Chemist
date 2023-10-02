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
#include <chemist/basis_set/atomic_basis_set/detail_/atomic_basis_set_pimpl.hpp>
#include <utility>

using namespace chemist::basis_set;
using namespace chemist::basis_set::detail_;

TEMPLATE_TEST_CASE("AtomicBasisSetPIMPL", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using shell_type = Shell<cg_type>;
    using abs_type   = AtomicBasisSet<shell_type>;
    using abs_traits = AtomicBasisSetTraits<abs_type>;
    using pimpl_type = AtomicBasisSetPIMPL<shell_type>;

    using pure_type          = typename abs_traits::pure_type;
    using l_type             = typename abs_traits::angular_momentum_type;
    using center_type        = typename abs_traits::center_type;
    using name_type          = typename abs_traits::name_type;
    using atomic_number_type = typename abs_traits::atomic_number_type;
    using range_type         = typename abs_traits::range_type;
    using shell_view_type    = typename abs_traits::const_shell_reference;
    using coeff_vector = std::vector<typename abs_traits::coefficient_type>;
    using exp_vector   = std::vector<typename abs_traits::exponent_type>;

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
    shell_type shell0{cart, l0, cg0}, shell1{pure, l1, cg1};
    std::vector<shell_view_type> shell_views{shell0, shell1};

    pimpl_type pimpl0;
    pimpl_type pimpl1(r1);
    pimpl_type pimpl2(name1, z1, r1);
    pimpl_type has_shells(name1, z1, r1);
    has_shells.add_shell(cart, l0, cg0);
    has_shells.add_shell(pure, l1, cg1);

    SECTION("Ctors") {
        SECTION("Defaulted") {
            REQUIRE_FALSE(pimpl0.basis_set_name().has_value());
            REQUIRE_FALSE(pimpl0.atomic_number().has_value());
            REQUIRE(pimpl0.center() == r0);
            REQUIRE(pimpl0.size() == 0);
            REQUIRE(pimpl0.n_primitives() == 0);
        }
        SECTION("With center") {
            REQUIRE_FALSE(pimpl1.basis_set_name().has_value());
            REQUIRE_FALSE(pimpl1.atomic_number().has_value());
            REQUIRE(pimpl1.center() == r1);
            REQUIRE(pimpl1.size() == 0);
            REQUIRE(pimpl1.n_primitives() == 0);
        }
        SECTION("With name, z, and center") {
            REQUIRE(pimpl2.basis_set_name().value() == name1);
            REQUIRE(pimpl2.atomic_number().value() == z1);
            REQUIRE(pimpl2.center() == r1);
            REQUIRE(pimpl2.size() == 0);
            REQUIRE(pimpl2.n_primitives() == 0);
        }
        SECTION("With center and shells") {
            pimpl_type from_inputs(r1, shell_views);
            REQUIRE_FALSE(from_inputs.basis_set_name().has_value());
            REQUIRE_FALSE(from_inputs.atomic_number().has_value());
            REQUIRE(from_inputs.center() == r1);
            REQUIRE(from_inputs.size() == 2);
            REQUIRE(from_inputs.n_primitives() == 4);
            REQUIRE(from_inputs[0] == shell0);
            REQUIRE(from_inputs[1] == shell1);
        }
        SECTION("With name, z, and shells") {
            pimpl_type from_inputs(name1, z1, r1, shell_views);
            REQUIRE(from_inputs.basis_set_name().value() == name1);
            REQUIRE(from_inputs.atomic_number().value() == z1);
            REQUIRE(from_inputs.center() == r1);
            REQUIRE(from_inputs.size() == 2);
            REQUIRE(from_inputs.n_primitives() == 4);
            REQUIRE(from_inputs[0] == shell0);
            REQUIRE(from_inputs[1] == shell1);
        }
    }
    SECTION("Getters/setters") {
        SECTION("add_shell") {
            pimpl0.add_shell(cart, l1, cg1);
            REQUIRE(pimpl0.size() == 1);
            REQUIRE(pimpl0.n_primitives() == 3);
        }
        SECTION("basis_set_name") {
            REQUIRE(has_shells.basis_set_name() == std::make_optional(name1));
        }
        SECTION("basis_set_name const") {
            REQUIRE(std::as_const(has_shells).basis_set_name() ==
                    std::make_optional(name1));
        }
        SECTION("atomic_number") {
            REQUIRE(has_shells.atomic_number() == std::make_optional(z1));
        }
        SECTION("atomic_number const") {
            REQUIRE(std::as_const(has_shells).atomic_number() ==
                    std::make_optional(z1));
        }
        SECTION("center") { REQUIRE(has_shells.center() == r1); }
        SECTION("center const") {
            REQUIRE(std::as_const(has_shells).center() == r1);
        }
        SECTION("operator[]") {
            REQUIRE(has_shells[0] == shell0);
            REQUIRE(has_shells[1] == shell1);
        }
        SECTION("operator[] const") {
            REQUIRE(std::as_const(has_shells)[0] == shell0);
            REQUIRE(std::as_const(has_shells)[1] == shell1);
        }
        SECTION("l") {
            REQUIRE(has_shells.l(0) == l0);
            REQUIRE(has_shells.l(1) == l1);
        }
        SECTION("l const") {
            REQUIRE(std::as_const(has_shells).l(0) == l0);
            REQUIRE(std::as_const(has_shells).l(1) == l1);
        }
        SECTION("pure") {
            REQUIRE(has_shells.pure(0) == cart);
            REQUIRE(has_shells.pure(1) == pure);
        }
        SECTION("pure const") {
            REQUIRE(std::as_const(has_shells).pure(0) == cart);
            REQUIRE(std::as_const(has_shells).pure(1) == pure);
        }
        SECTION("cg") {
            REQUIRE(has_shells.cg(0) == cg0);
            REQUIRE(has_shells.cg(1) == cg1);
        }
        SECTION("cg const") {
            REQUIRE(std::as_const(has_shells).cg(0) == cg0);
            REQUIRE(std::as_const(has_shells).cg(1) == cg1);
        }
        SECTION("primitive_range") {
            REQUIRE(has_shells.primitive_range(0) == range_type{0, 1});
            REQUIRE(has_shells.primitive_range(1) == range_type{1, 4});
            REQUIRE_THROWS_AS(has_shells.primitive_range(2),
                              std::runtime_error);
        }
        SECTION("prim2shell") {
            REQUIRE(has_shells.prim2shell(0) == 0);
            REQUIRE(has_shells.prim2shell(1) == 1);
            REQUIRE(has_shells.prim2shell(2) == 1);
            REQUIRE(has_shells.prim2shell(3) == 1);
        }
        SECTION("primitive") {
            REQUIRE(has_shells.primitive(0) == p0);
            REQUIRE(has_shells.primitive(1) == p0);
            REQUIRE(has_shells.primitive(2) == p1);
            REQUIRE(has_shells.primitive(3) == p2);
        }
        SECTION("primitive const") {
            REQUIRE(std::as_const(has_shells).primitive(0) == p0);
            REQUIRE(std::as_const(has_shells).primitive(1) == p0);
            REQUIRE(std::as_const(has_shells).primitive(2) == p1);
            REQUIRE(std::as_const(has_shells).primitive(3) == p2);
        }
        SECTION("size") { REQUIRE(has_shells.size() == 2); }
        SECTION("n_primitives") { REQUIRE(has_shells.n_primitives() == 4); }
    }
    SECTION("Utility") {
        SECTION("operator==") {
            SECTION("are equal") {
                pimpl_type has_shells2(name1, z1, r1);
                has_shells2.add_shell(cart, l0, cg0);
                has_shells2.add_shell(pure, l1, cg1);

                REQUIRE(pimpl0 == pimpl_type{});
                REQUIRE(has_shells == has_shells2);
            }
            SECTION("Different name") {
                REQUIRE_FALSE(pimpl2 == pimpl_type{name0, z1, r1});
            }
            SECTION("Different z") {
                REQUIRE_FALSE(pimpl2 == pimpl_type{name1, z0, r1});
            }
            SECTION("Different center") {
                REQUIRE_FALSE(pimpl2 == pimpl_type{name1, z1, r0});
            }
            SECTION("Different primitives on contracted gaussian") {
                pimpl_type diff_prims_per_cg(name1, z1, r1);
                diff_prims_per_cg.add_shell(cart, l0, cg1);
                diff_prims_per_cg.add_shell(pure, l1, cg1);
                REQUIRE_FALSE(has_shells == diff_prims_per_cg);
            }
            SECTION("Different coeffs") {
                cg_type cg(cs.begin() + 1, cs.begin() + 2, es.begin(),
                           es.begin() + 1, r1);
                pimpl_type diff_coeffs(name1, z1, r1);
                diff_coeffs.add_shell(cart, l0, cg);
                diff_coeffs.add_shell(pure, l1, cg1);
                REQUIRE_FALSE(has_shells == diff_coeffs);
            }
            SECTION("Different exponents") {
                cg_type cg(cs.begin(), cs.begin() + 1, es.begin() + 1,
                           es.begin() + 2, r1);
                pimpl_type diff_exps(name1, z1, r1);
                diff_exps.add_shell(cart, l0, cg);
                diff_exps.add_shell(pure, l1, cg1);
                REQUIRE_FALSE(has_shells == diff_exps);
            }
            SECTION("Different purity") {
                pimpl_type diff_pure(name1, z1, r1);
                diff_pure.add_shell(pure, l0, cg0);
                diff_pure.add_shell(pure, l1, cg1);
                REQUIRE_FALSE(has_shells == diff_pure);
            }
            SECTION("Different l") {
                pimpl_type diff_pure(name1, z1, r1);
                diff_pure.add_shell(cart, l1, cg0);
                diff_pure.add_shell(pure, l1, cg1);
                REQUIRE_FALSE(has_shells == diff_pure);
            }
        }
    }
}