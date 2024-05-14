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

#include <catch2/catch.hpp>
#include <chemist/basis_set/ao_basis_set/detail_/ao_basis_set_pimpl.hpp>
#include <utility>

using namespace chemist::basis_set;
using namespace chemist::basis_set::detail_;

TEMPLATE_TEST_CASE("AOBasisSetPIMPL", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using shell_type = Shell<cg_type>;
    using abs_type   = AtomicBasisSet<shell_type>;
    using abs_traits = AtomicBasisSetTraits<abs_type>;
    using pimpl_type = AOBasisSetPIMPL<abs_type>;

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

    pimpl_type defaulted, has_abs;
    has_abs.add_atomic_basis_set(abs);
    has_abs.add_atomic_basis_set(abs);

    SECTION("Getters/setters") {
        SECTION("add_atomic_basis_set") {
            REQUIRE(defaulted.size() == 0);
            defaulted.add_atomic_basis_set(abs);
            REQUIRE(defaulted.size() == 1);
            REQUIRE(defaulted.n_shells() == 1);
            REQUIRE(defaulted.n_primitives() == 3);
        }
        SECTION("size") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(has_abs.size() == 2);
        }
        SECTION("n_shells") {
            REQUIRE(defaulted.n_shells() == 0);
            REQUIRE(has_abs.n_shells() == 2);
        }
        SECTION("n_primitives") {
            REQUIRE(defaulted.n_primitives() == 0);
            REQUIRE(has_abs.n_primitives() == 6);
        }
        SECTION("shell_range") {
            REQUIRE(has_abs.shell_range(0) == range_type{0, 1});
            REQUIRE(has_abs.shell_range(1) == range_type{1, 2});
        }
        SECTION("primitive_range") {
            REQUIRE(has_abs.primitive_range(0) == range_type{0, 3});
            REQUIRE(has_abs.primitive_range(1) == range_type{3, 6});
        }
        SECTION("shell_to_center") {
            REQUIRE(has_abs.shell_to_center(0) == 0);
            REQUIRE(has_abs.shell_to_center(1) == 1);
        }
        SECTION("primitive_to_center") {
            REQUIRE(has_abs.primitive_to_center(0) == 0);
            REQUIRE(has_abs.primitive_to_center(3) == 1);
        }
        SECTION("primitive_to_shell") {
            REQUIRE(has_abs.primitive_to_shell(0) == 0);
            REQUIRE(has_abs.primitive_to_shell(3) == 1);
        }
        SECTION("at") { REQUIRE(has_abs.at(0) == abs); }
        SECTION("at const") { REQUIRE(std::as_const(has_abs).at(0) == abs); }
        SECTION("shell") { REQUIRE(has_abs.shell(0) == shell); }
        SECTION("shell const") {
            REQUIRE(std::as_const(has_abs).shell(0) == shell);
        }
        SECTION("cg") { REQUIRE(has_abs.cg(0) == cg); }
        SECTION("cg const") { REQUIRE(std::as_const(has_abs).cg(0) == cg); }
        SECTION("primitive") { REQUIRE(has_abs.primitive(0) == p0); }
        SECTION("primitive const") {
            REQUIRE(std::as_const(has_abs).primitive(0) == p0);
        }
        SECTION("center") { REQUIRE(has_abs.center(0) == r); }
        SECTION("center const") {
            REQUIRE(std::as_const(has_abs).center(0) == r);
        }
        SECTION("max_l") {
            REQUIRE(defaulted.max_l() == 0);
            REQUIRE(has_abs.max_l() == 1);
        }
    }
}