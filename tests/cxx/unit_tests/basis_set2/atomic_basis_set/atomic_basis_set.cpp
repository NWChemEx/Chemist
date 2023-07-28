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
    abs1.add_shell(cart, l1, cg1);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(abs0.is_null());
            REQUIRE(abs0.size() == 0);
            REQUIRE_FALSE(abs0.has_name());
            REQUIRE_FALSE(abs0.has_atomic_number());
            REQUIRE_FALSE(abs0.has_center());

            REQUIRE_FALSE(abs1.is_null());
            REQUIRE(abs1.size() == 1);
            REQUIRE(abs1.basis_set_name() == name1);
            REQUIRE(abs1.atomic_number() == z1);
            REQUIRE(abs1.center() == r1);
        }
        SECTION("Copy") {
            abs_type abs0_copy(abs0);
            REQUIRE(abs0_copy.is_null());
            REQUIRE(abs0_copy.size() == 0);
            REQUIRE_FALSE(abs0_copy.has_name());
            REQUIRE_FALSE(abs0_copy.has_atomic_number());
            REQUIRE_FALSE(abs0_copy.has_center());

            abs_type abs1_copy(abs1);
            REQUIRE_FALSE(abs1_copy.is_null());
            REQUIRE(abs1_copy.size() == 1);
            REQUIRE(abs1_copy.basis_set_name() == name1);
            REQUIRE(abs1_copy.atomic_number() == z1);
            REQUIRE(abs1_copy.center() == r1);
        }
        SECTION("Copy Assignment") {
            abs_type abs0_copy;
            auto pabs0_copy = &(abs0_copy = abs0);
            REQUIRE(abs0_copy.is_null());
            REQUIRE(abs0_copy.size() == 0);
            REQUIRE_FALSE(abs0_copy.has_name());
            REQUIRE_FALSE(abs0_copy.has_atomic_number());
            REQUIRE_FALSE(abs0_copy.has_center());
            REQUIRE(pabs0_copy == &abs0_copy);

            abs_type abs1_copy;
            auto pabs1_copy = &(abs1_copy = abs1);
            REQUIRE_FALSE(abs1_copy.is_null());
            REQUIRE(abs1_copy.size() == 1);
            REQUIRE(abs1_copy.basis_set_name() == name1);
            REQUIRE(abs1_copy.atomic_number() == z1);
            REQUIRE(abs1_copy.center() == r1);
            REQUIRE(pabs1_copy == &abs1_copy);
        }
        SECTION("Move") {
            abs_type abs0_move(std::move(abs0));
            REQUIRE(abs0_move.is_null());
            REQUIRE(abs0_move.size() == 0);
            REQUIRE_FALSE(abs0_move.has_name());
            REQUIRE_FALSE(abs0_move.has_atomic_number());
            REQUIRE_FALSE(abs0_move.has_center());
            REQUIRE(abs0.is_null());

            abs_type abs1_move(std::move(abs1));
            REQUIRE_FALSE(abs1_move.is_null());
            REQUIRE(abs1_move.size() == 1);
            REQUIRE(abs1_move.basis_set_name() == name1);
            REQUIRE(abs1_move.atomic_number() == z1);
            REQUIRE(abs1_move.center() == r1);
            REQUIRE(abs1.is_null());
        }
        SECTION("Move Assignment") {
            abs_type abs0_copy;
            auto pabs0_copy = &(abs0_copy = std::move(abs0));
            REQUIRE(abs0_copy.is_null());
            REQUIRE(abs0_copy.size() == 0);
            REQUIRE_FALSE(abs0_copy.has_name());
            REQUIRE_FALSE(abs0_copy.has_atomic_number());
            REQUIRE_FALSE(abs0_copy.has_center());
            REQUIRE(pabs0_copy == &abs0_copy);

            abs_type abs1_copy;
            auto pabs1_copy = &(abs1_copy = std::move(abs1));
            REQUIRE_FALSE(abs1_copy.is_null());
            REQUIRE(abs1_copy.size() == 1);
            REQUIRE(abs1_copy.basis_set_name() == name1);
            REQUIRE(abs1_copy.atomic_number() == z1);
            REQUIRE(abs1_copy.center() == r1);
            REQUIRE(pabs1_copy == &abs1_copy);
        }
        SECTION("With name, z, and coords") {
            abs_type name_z_coords(name1, z1, 7.0, 8.0, 9.0);
            REQUIRE_FALSE(name_z_coords.is_null());
            REQUIRE(name_z_coords.basis_set_name() == name1);
            REQUIRE(name_z_coords.atomic_number() == z1);
            REQUIRE(name_z_coords.center() == r1);
        }
        SECTION("With name, z, and center") {
            abs_type with_name_z_center(name1, z1, r1);
            REQUIRE_FALSE(with_name_z_center.is_null());
            REQUIRE(with_name_z_center.basis_set_name() == name1);
            REQUIRE(with_name_z_center.atomic_number() == z1);
            REQUIRE(with_name_z_center.center() == r1);
        }
        SECTION("With coords") {
            abs_type with_coords(7.0, 8.0, 9.0);
            REQUIRE_FALSE(with_coords.is_null());
            REQUIRE_FALSE(with_coords.has_name());
            REQUIRE_FALSE(with_coords.has_atomic_number());
            REQUIRE(with_coords.center() == r1);
        }
        SECTION("With center") {
            abs_type with_center(r1);
            REQUIRE_FALSE(with_center.is_null());
            REQUIRE_FALSE(with_center.has_name());
            REQUIRE_FALSE(with_center.has_atomic_number());
            REQUIRE(with_center.center() == r1);
        }
        SECTION("With name and z") {
            abs_type with_name_z(name1, z1);
            REQUIRE_FALSE(with_name_z.is_null());
            REQUIRE(with_name_z.basis_set_name() == name1);
            REQUIRE(with_name_z.atomic_number() == z1);
            REQUIRE(with_name_z.center() == r0);
        }
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