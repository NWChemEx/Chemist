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
    using shell_ref_type     = typename abs_type::const_reference;
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
    shell_type shell0{cart, l0, cg0}, shell1{pure, l1, cg1};
    abs_type abs1(name1, z1, r1);
    abs1.add_shell(cart, l1, cg1);

    pimpl_type pimpl1{shell_ref_vector{abs1.begin(), abs1.end()}};
    pimpl_type pimpl2{abs1.basis_set_name(), abs1.atomic_number(),
                      abs1.center(),
                      shell_ref_vector{abs1.begin(), abs1.end()}};

    SECTION("Ctors") {
        SECTION("Values 1") {}
        SECTION("Values 2") {}
    }
    SECTION("Getters/setters") {
        SECTION("has_name") {}
        SECTION("name") {}
        SECTION("name const") {}
        SECTION("has_atomic_number") {}
        SECTION("atomic_number") {}
        SECTION("atomic_number const") {}
        SECTION("has_center") {}
        SECTION("center") {}
        SECTION("center const") {}
        SECTION("pure") {}
        SECTION("pure const") {}
        SECTION("l") {}
        SECTION("l const") {}
        SECTION("size") {}
        SECTION("n_aos") {}
        SECTION("n_primitives") {}
        SECTION("primitive_range") {}
        SECTION("primitive_to_shell") {}
        SECTION("operator[]") {}
        SECTION("operator[] const") {}
        SECTION("cg") {}
        SECTION("cg const") {}
        SECTION("primitive") {}
        SECTION("primitive const") {}
    }
    SECTION("Utility") {
        SECTION("operator==") {
            SECTION("are equal") {}
            SECTION("Different name") {}
            SECTION("Different z") {}
            SECTION("Different center") {}
            SECTION("Different primitives on contracted gaussian") {}
            SECTION("Different coeffs") {}
            SECTION("Different exponents") {}
            SECTION("Different purity") {}
            SECTION("Different l") {}
        }
    }
}