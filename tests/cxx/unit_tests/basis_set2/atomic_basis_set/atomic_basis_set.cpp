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
    std::vector<shell_type> shells{shell0, shell1};

    abs_type abs0;
    abs_type abs1(name1, z1, r1);
    abs1.add_shell(cart, l1, cg1);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(abs0.is_null());
            REQUIRE(abs0.size() == 0);

            REQUIRE_FALSE(abs1.is_null());
            REQUIRE(abs1.size() == 1);
            REQUIRE(abs1.basis_set_name().value() == name1);
            REQUIRE(abs1.atomic_number().value() == z1);
            REQUIRE(abs1.center() == r1);
        }
        SECTION("Copy") {
            abs_type abs0_copy(abs0);
            REQUIRE(abs0_copy.is_null());
            REQUIRE(abs0_copy.size() == 0);

            abs_type abs1_copy(abs1);
            REQUIRE_FALSE(abs1_copy.is_null());
            REQUIRE(abs1_copy.size() == 1);
            REQUIRE(abs1_copy.basis_set_name().value() == name1);
            REQUIRE(abs1_copy.atomic_number().value() == z1);
            REQUIRE(abs1_copy.center() == r1);
        }
        SECTION("Copy Assignment") {
            abs_type abs0_copy;
            auto pabs0_copy = &(abs0_copy = abs0);
            REQUIRE(abs0_copy.is_null());
            REQUIRE(abs0_copy.size() == 0);
            REQUIRE(pabs0_copy == &abs0_copy);

            abs_type abs1_copy;
            auto pabs1_copy = &(abs1_copy = abs1);
            REQUIRE_FALSE(abs1_copy.is_null());
            REQUIRE(abs1_copy.size() == 1);
            REQUIRE(abs1_copy.basis_set_name().value() == name1);
            REQUIRE(abs1_copy.atomic_number().value() == z1);
            REQUIRE(abs1_copy.center() == r1);
            REQUIRE(pabs1_copy == &abs1_copy);
        }
        SECTION("Move") {
            abs_type abs0_move(std::move(abs0));
            REQUIRE(abs0_move.is_null());
            REQUIRE(abs0_move.size() == 0);
            REQUIRE(abs0.is_null());

            abs_type abs1_move(std::move(abs1));
            REQUIRE_FALSE(abs1_move.is_null());
            REQUIRE(abs1_move.size() == 1);
            REQUIRE(abs1_move.basis_set_name().value() == name1);
            REQUIRE(abs1_move.atomic_number().value() == z1);
            REQUIRE(abs1_move.center() == r1);
            REQUIRE(abs1.is_null());
        }
        SECTION("Move Assignment") {
            abs_type abs0_copy;
            auto pabs0_copy = &(abs0_copy = std::move(abs0));
            REQUIRE(abs0_copy.is_null());
            REQUIRE(abs0_copy.size() == 0);
            REQUIRE(pabs0_copy == &abs0_copy);

            abs_type abs1_copy;
            auto pabs1_copy = &(abs1_copy = std::move(abs1));
            REQUIRE_FALSE(abs1_copy.is_null());
            REQUIRE(abs1_copy.size() == 1);
            REQUIRE(abs1_copy.basis_set_name().value() == name1);
            REQUIRE(abs1_copy.atomic_number().value() == z1);
            REQUIRE(abs1_copy.center() == r1);
            REQUIRE(pabs1_copy == &abs1_copy);
        }
        SECTION("With name, z, and coords") {
            abs_type name_z_coords(name1, z1, 7.0, 8.0, 9.0);
            REQUIRE_FALSE(name_z_coords.is_null());
            REQUIRE(name_z_coords.basis_set_name().value() == name1);
            REQUIRE(name_z_coords.atomic_number().value() == z1);
            REQUIRE(name_z_coords.center() == r1);
        }
        SECTION("With name, z, and center") {
            abs_type with_name_z_center(name1, z1, r1);
            REQUIRE_FALSE(with_name_z_center.is_null());
            REQUIRE(with_name_z_center.basis_set_name().value() == name1);
            REQUIRE(with_name_z_center.atomic_number().value() == z1);
            REQUIRE(with_name_z_center.center() == r1);
        }
        SECTION("With coords") {
            abs_type with_coords(7.0, 8.0, 9.0);
            REQUIRE_FALSE(with_coords.is_null());
            REQUIRE_FALSE(with_coords.basis_set_name().has_value());
            REQUIRE_FALSE(with_coords.atomic_number().has_value());
            REQUIRE(with_coords.center() == r1);
        }
        SECTION("With center") {
            abs_type with_center(r1);
            REQUIRE_FALSE(with_center.is_null());
            REQUIRE_FALSE(with_center.basis_set_name().has_value());
            REQUIRE_FALSE(with_center.atomic_number().has_value());
            REQUIRE(with_center.center() == r1);
        }
        SECTION("With name and z") {
            abs_type with_name_z(name1, z1);
            REQUIRE_FALSE(with_name_z.is_null());
            REQUIRE(with_name_z.basis_set_name().value() == name1);
            REQUIRE(with_name_z.atomic_number().value() == z1);
            REQUIRE(with_name_z.center() == r0);
        }
        SECTION("With center and shells") {
            abs_type with_inputs(r1, shells.begin(), shells.end());
            REQUIRE_FALSE(with_inputs.is_null());
            REQUIRE_FALSE(with_inputs.basis_set_name().has_value());
            REQUIRE_FALSE(with_inputs.atomic_number().has_value());
            REQUIRE(with_inputs.center() == r1);
            REQUIRE(with_inputs.center() == r1);
            REQUIRE(with_inputs.size() == 2);
        }
        SECTION("With name, z, center, and shells") {
            abs_type with_inputs(name1, z1, r1, shells.begin(), shells.end());
            REQUIRE_FALSE(with_inputs.is_null());
            REQUIRE(with_inputs.basis_set_name().value() == name1);
            REQUIRE(with_inputs.atomic_number().value() == z1);
            REQUIRE(with_inputs.center() == r1);
            REQUIRE(with_inputs.size() == 2);
        }
    }
    SECTION("Getters/setters") {
        SECTION("basis_set_name") {
            REQUIRE(abs0.basis_set_name() == std::optional<name_type>());
            REQUIRE(abs1.basis_set_name() == std::make_optional(name1));
        }
        SECTION("basis_set_name const") {
            REQUIRE_THROWS_AS(std::as_const(abs0).basis_set_name(),
                              std::runtime_error);
            REQUIRE(std::as_const(abs1).basis_set_name() ==
                    std::make_optional(name1));
        }
        SECTION("atomic_number") {
            REQUIRE(abs0.atomic_number() ==
                    std::optional<atomic_number_type>());
            REQUIRE(abs1.atomic_number() == std::make_optional(z1));
        }
        SECTION("atomic_number const") {
            REQUIRE_THROWS_AS(std::as_const(abs0).atomic_number(),
                              std::runtime_error);
            REQUIRE(std::as_const(abs1).atomic_number() ==
                    std::make_optional(z1));
        }
        SECTION("center") {
            REQUIRE(abs0.center() == r0);
            REQUIRE(abs1.center() == r1);
        }
        SECTION("center const") {
            REQUIRE_THROWS_AS(std::as_const(abs0).center(), std::runtime_error);
            REQUIRE(std::as_const(abs1).center() == r1);
        }
        SECTION("add_shell") {
            REQUIRE(abs0.is_null());
            REQUIRE(abs0.size() == 0);
            abs0.add_shell(cart, l1, cg1);
            REQUIRE(abs0.size() == 1);
            REQUIRE_FALSE(abs0.is_null());

            REQUIRE(abs1.size() == 1);
            abs1.add_shell(cart, l1, cg1);
            REQUIRE(abs1.size() == 2);
        }
        SECTION("n_aos") {
            REQUIRE(abs0.n_aos() == 0);
            REQUIRE(abs1.n_aos() == 3);
        }
        SECTION("n_primitives") {
            REQUIRE(abs0.n_primitives() == 0);
            REQUIRE(abs1.n_primitives() == 3);
        }
        SECTION("primitive_range") {
            REQUIRE_THROWS_AS(abs0.primitive_range(0), std::out_of_range);
            REQUIRE(abs1.primitive_range(0) == range_type{0, 3});
            REQUIRE_THROWS_AS(abs1.primitive_range(1), std::out_of_range);
        }
        SECTION("primitive_to_shell") {
            REQUIRE_THROWS_AS(abs0.primitive_to_shell(0), std::out_of_range);
            REQUIRE(abs1.primitive_to_shell(0) == 0);
            REQUIRE(abs1.primitive_to_shell(1) == 0);
            REQUIRE(abs1.primitive_to_shell(2) == 0);
            REQUIRE_THROWS_AS(abs1.primitive_to_shell(3), std::out_of_range);
        }
        SECTION("primitive") {
            REQUIRE_THROWS_AS(abs0.primitive(0), std::out_of_range);
            REQUIRE(abs1.primitive(0) == p0);
            REQUIRE(abs1.primitive(1) == p1);
            REQUIRE(abs1.primitive(2) == p2);
            REQUIRE_THROWS_AS(abs1.primitive(4), std::out_of_range);
        }
        SECTION("primitive const") {
            REQUIRE_THROWS_AS(std::as_const(abs0).primitive(0),
                              std::out_of_range);
            REQUIRE(std::as_const(abs1).primitive(0) == p0);
            REQUIRE(std::as_const(abs1).primitive(1) == p1);
            REQUIRE(std::as_const(abs1).primitive(2) == p2);
            REQUIRE_THROWS_AS(std::as_const(abs1).primitive(4),
                              std::out_of_range);
        }
    }
    SECTION("Utility") {
        SECTION("swap") {
            abs_type abs0_copy(abs0);
            abs_type abs1_copy(abs1);
            abs0.swap(abs1);
            REQUIRE(abs0_copy == abs1);
            REQUIRE(abs1_copy == abs0);
        }
        SECTION("is_null") {
            REQUIRE(abs0.is_null());
            REQUIRE_FALSE(abs1.is_null());
        }
        SECTION("operator==") {
            SECTION("equivalent") {
                abs_type abs2(abs1);
                REQUIRE(abs0 == abs_type{});
                REQUIRE(abs1 == abs2);
                // Rest of the state is handled by the comparison of the PIMPLS
            }
            SECTION("null and non-null") { REQUIRE_FALSE(abs0 == abs1); }
        }
        SECTION("operator!=") { REQUIRE(abs0 != abs1); }
    }
}