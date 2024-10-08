/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/basis_set/ao_basis_set.hpp>
#include <chemist/basis_set/atomic_basis_set_traits.hpp>
#include <utility>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("AOBasisSet", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using shell_type = Shell<cg_type>;
    using abs_type   = AtomicBasisSet<shell_type>;
    using abs_traits = AtomicBasisSetTraits<abs_type>;
    using aobs_type  = AOBasisSet<abs_type>;

    using pure_type          = typename abs_traits::pure_type;
    using l_type             = typename abs_traits::angular_momentum_type;
    using center_type        = typename abs_traits::center_type;
    using name_type          = typename abs_traits::name_type;
    using atomic_number_type = typename abs_traits::atomic_number_type;
    using range_type         = typename abs_traits::range_type;
    using coeff_vector = std::vector<typename abs_traits::coefficient_type>;
    using exp_vector   = std::vector<typename abs_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0};
    l_type l{1};
    center_type r{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    prim_type p0{cs[0], es[0], r};
    cg_type cg(cs.begin(), cs.end(), es.begin(), es.end(), r);
    name_type name{"name"};
    atomic_number_type z{1};
    shell_type shell{cart, l, cg};
    abs_type abs(name, z, r);
    abs.add_shell(cart, l, cg);

    aobs_type aobs0, aobs1;
    aobs1.add_center(abs);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            aobs_type defaulted;
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.n_shells() == 0);
            REQUIRE(defaulted.n_aos() == 0);
            REQUIRE(defaulted.n_primitives() == 0);
            REQUIRE(defaulted == aobs0);
            REQUIRE_FALSE(defaulted == aobs1);
        }
        SECTION("Copy") {
            aobs_type aobs0_copy(aobs0);
            REQUIRE(aobs0_copy.size() == 0);
            REQUIRE(aobs0_copy.n_shells() == 0);
            REQUIRE(aobs0_copy.n_aos() == 0);
            REQUIRE(aobs0_copy.n_primitives() == 0);

            aobs_type aobs1_copy(aobs1);
            REQUIRE(aobs1_copy.size() == 1);
            REQUIRE(aobs1_copy.n_shells() == 1);
            REQUIRE(aobs1_copy.n_aos() == 3);
            REQUIRE(aobs1_copy.n_primitives() == 3);
        }
        SECTION("Copy Assignment") {
            aobs_type aobs0_copy;
            auto paobs0_copy = &(aobs0_copy = aobs0);
            REQUIRE(aobs0_copy.size() == 0);
            REQUIRE(aobs0_copy.n_shells() == 0);
            REQUIRE(aobs0_copy.n_aos() == 0);
            REQUIRE(aobs0_copy.n_primitives() == 0);
            REQUIRE(paobs0_copy == &aobs0_copy);

            aobs_type aobs1_copy;
            auto paobs1_copy = &(aobs1_copy = aobs1);
            REQUIRE(aobs1_copy.size() == 1);
            REQUIRE(aobs1_copy.n_shells() == 1);
            REQUIRE(aobs1_copy.n_aos() == 3);
            REQUIRE(aobs1_copy.n_primitives() == 3);
            REQUIRE(paobs1_copy == &aobs1_copy);
        }
        SECTION("Move") {
            aobs_type aobs0_copy(std::move(aobs0));
            REQUIRE(aobs0_copy.size() == 0);
            REQUIRE(aobs0_copy.n_shells() == 0);
            REQUIRE(aobs0_copy.n_aos() == 0);
            REQUIRE(aobs0_copy.n_primitives() == 0);

            aobs_type aobs1_copy(std::move(aobs1));
            REQUIRE(aobs1_copy.size() == 1);
            REQUIRE(aobs1_copy.n_shells() == 1);
            REQUIRE(aobs1_copy.n_aos() == 3);
            REQUIRE(aobs1_copy.n_primitives() == 3);
            REQUIRE(aobs1.size() == 0);
        }
        SECTION("Move Assignment") {
            aobs_type aobs0_copy;
            auto paobs0_copy = &(aobs0_copy = std::move(aobs0));
            REQUIRE(aobs0_copy.size() == 0);
            REQUIRE(aobs0_copy.n_shells() == 0);
            REQUIRE(aobs0_copy.n_aos() == 0);
            REQUIRE(aobs0_copy.n_primitives() == 0);
            REQUIRE(paobs0_copy == &aobs0_copy);

            aobs_type aobs1_copy;
            auto paobs1_copy = &(aobs1_copy = std::move(aobs1));
            REQUIRE(aobs1_copy.size() == 1);
            REQUIRE(aobs1_copy.n_shells() == 1);
            REQUIRE(aobs1_copy.n_aos() == 3);
            REQUIRE(aobs1_copy.n_primitives() == 3);
            REQUIRE(paobs1_copy == &aobs1_copy);
            REQUIRE(aobs1.size() == 0);
        }
    }
    SECTION("Getters/setters") {
        SECTION("add_center") {
            aobs0.add_center(abs);
            REQUIRE(aobs0 == aobs1);
        }
        SECTION("shell_range") {
            REQUIRE_THROWS_AS(aobs0.shell_range(0), std::out_of_range);
            REQUIRE(aobs1.shell_range(0) == range_type{0, 1});
        }
        SECTION("max_l") {
            REQUIRE_THROWS_AS(aobs0.max_l(), std::runtime_error);
            REQUIRE(aobs1.max_l() == 1);
        }
        SECTION("n_shells") {
            REQUIRE(aobs0.n_shells() == 0);
            REQUIRE(aobs1.n_shells() == 1);
        }
        SECTION("shell") {
            REQUIRE_THROWS_AS(aobs0.shell(0), std::out_of_range);
            REQUIRE(aobs1.shell(0) == shell);
        }
        SECTION("shell const") {
            REQUIRE_THROWS_AS(std::as_const(aobs0).shell(0), std::out_of_range);
            REQUIRE(std::as_const(aobs1).shell(0) == shell);
        }
        SECTION("primitive_range") {
            REQUIRE_THROWS_AS(aobs0.primitive_range(0), std::out_of_range);
            REQUIRE(aobs1.primitive_range(0) == range_type{0, 3});
        }
        SECTION("n_aos") {
            REQUIRE(aobs0.n_aos() == 0);
            REQUIRE(aobs1.n_aos() == 3);
        }
        SECTION("n_primitives") {
            REQUIRE(aobs0.n_primitives() == 0);
            REQUIRE(aobs1.n_primitives() == 3);
        }
        SECTION("primitive") {
            REQUIRE_THROWS_AS(aobs0.primitive(0), std::out_of_range);
            REQUIRE(aobs1.primitive(0) == p0);
        }
        SECTION("primitive const") {
            REQUIRE_THROWS_AS(std::as_const(aobs0).primitive(0),
                              std::out_of_range);
            REQUIRE(std::as_const(aobs1).primitive(0) == p0);
        }
    }
    SECTION("Utility") {
        SECTION("swap") {
            aobs_type aobs0_copy(aobs0);
            aobs_type aobs1_copy(aobs1);
            aobs0.swap(aobs1);
            REQUIRE(aobs0_copy == aobs1);
            REQUIRE(aobs1_copy == aobs0);
        }
        SECTION("operator+=") {
            auto paobs0 = &(aobs0 += aobs1);
            REQUIRE(aobs0 == aobs1);
            REQUIRE(paobs0 == &aobs0);
        }
        SECTION("operator+") {
            auto aobs2 = aobs0 + aobs1;
            REQUIRE(aobs2 == aobs1);
        }
    }
}