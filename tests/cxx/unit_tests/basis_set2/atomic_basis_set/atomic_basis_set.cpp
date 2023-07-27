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
#include <chemist/basis_set2/atomic_basis_set/atomic_basis_set.hpp>
#include <chemist/basis_set2/atomic_basis_set/atomic_basis_set_traits.hpp>
#include <sstream>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("AtomicBasisSet", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using shell_type = Shell<cg_type>;
    using abs_type   = AtomicBasisSet<shell_type>;
    using abs_traits = AtomicBasisSetTraits<abs_type>;

    using pure_type          = typename abs_traits::pure_type;
    using l_type             = typename abs_traits::angular_momentum_type;
    using coord_type         = typename abs_traits::coord_type;
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
    prim_type p0{cs[0], es[0], r1}, p1{cs[1], es[1], r1}, p2{cs[2], es[2], r1};
    cg_type cg0(cs.begin(), cs.begin() + 1, es.begin(), es.begin() + 1, r1);
    cg_type cg1(cs.begin(), cs.end(), es.begin(), es.end(), r1);
    name_type name0, name1{"name1"};
    atomic_number_type z0{0}, z1{1};
    shell_type shell0{cart, l0, cg0}, shell1{pure, l1, cg1};

    abs_type abs0;
    abs_type abs1(name1, z1, r1);
    abs1.add_shell(cart, l1, cs.begin(), cs.end(), es.begin(), es.end());

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(abs0.is_null());
            REQUIRE(abs0.size() == 0);
            REQUIRE_FALSE(abs0.has_name());
            REQUIRE_FALSE(abs0.has_atomic_number());
            REQUIRE_FALSE(abs0.has_center());
        }
        SECTION("Copy") {}
        SECTION("Copy Assignment") {}
        SECTION("Move") {}
        SECTION("Move Assignment") {}
        SECTION("With name, z, and coords") {}
        SECTION("With name, z, and center") {}
        SECTION("With coords") {}
        SECTION("With center") {}
        SECTION("With name and z") {}
    }
    SECTION("Getters/setters") {
        SECTION("has_name") {}
        SECTION("basis_set_name") {}
        SECTION("basis_set_name const") {}
        SECTION("has_atomic_number") {}
        SECTION("atomic_number") {}
        SECTION("atomic_number const") {}
        SECTION("add_shell") {}
        SECTION("n_aos") {}
        SECTION("n_primitives") {}
        SECTION("primitive_range") {}
        SECTION("primitive_to_shell") {}
        SECTION("primitive") {}
        SECTION("primitive const") {}
        SECTION("has_center") {}
        SECTION("center") {}
        SECTION("center const") {}
    }
    SECTION("Utility") {
        SECTION("swap") {}
        SECTION("is_null") {}
        SECTION("operator==") {
            SECTION("equivalent") {}
        }
        SECTION("operator!=") {}
    }
}