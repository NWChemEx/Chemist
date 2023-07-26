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
#include <chemist/basis_set2/atomic_basis_set/detail_/atomic_basis_set_pimpl.hpp>

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
    using coeff_vector = std::vector<typename abs_traits::coefficient_type>;
    using exp_vector   = std::vector<typename abs_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type l0{0}, l1{1};
    center_type r0, r1{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    cg_type cg0, cg1(cs.begin(), cs.end(), es.begin(), es.end(), r1);
    name_type name0, name1{"name1"};
    atomic_number_type z0{0}, z1{1};

    pimpl_type pimpl0;
    pimpl_type pimpl1(r1);
    pimpl_type pimpl2(name1, z1);
    pimpl_type pimpl3(name1, z1, r1);

    SECTION("Ctors") {
        SECTION("Defaulted") {
            REQUIRE(pimpl0.m_name.has_value() == false);
            REQUIRE(pimpl0.m_z.has_value() == false);
            REQUIRE(pimpl0.m_center.has_value() == false);
            REQUIRE(pimpl0.size() == 0);
            REQUIRE(pimpl0.n_primitives() == 0);
        }
        SECTION("With center") {
            REQUIRE(pimpl1.m_name.has_value() == false);
            REQUIRE(pimpl1.m_z.has_value() == false);
            REQUIRE(pimpl1.m_center.value() == r1);
            REQUIRE(pimpl1.size() == 0);
            REQUIRE(pimpl1.n_primitives() == 0);
        }
        SECTION("With name and z") {
            REQUIRE(pimpl2.m_name.value() == name1);
            REQUIRE(pimpl2.m_z.value() == z1);
            REQUIRE(pimpl2.m_center.has_value() == false);
            REQUIRE(pimpl2.size() == 0);
            REQUIRE(pimpl2.n_primitives() == 0);
        }
        SECTION("With name, z, and center") {
            REQUIRE(pimpl3.m_name.value() == name1);
            REQUIRE(pimpl3.m_z.value() == z1);
            REQUIRE(pimpl3.m_center.value() == r1);
            REQUIRE(pimpl3.size() == 0);
            REQUIRE(pimpl3.n_primitives() == 0);
        }
    }
    SECTION("Getters/setters") {
        SECTION("add_shell") {
            pimpl0.add_shell(cart, l1, cg1);
            REQUIRE(pimpl0.m_center.value() == r1);
            REQUIRE(pimpl0.size() == 1);
            REQUIRE(pimpl0.n_primitives() == 3);

            pimpl1.add_shell(cart, l1, cg1);
            REQUIRE(pimpl0.size() == 1);
            REQUIRE(pimpl0.n_primitives() == 3);

            pimpl_type different_center(r0);
            REQUIRE_THROWS_AS(different_center.add_shell(cart, l1, cg1),
                              std::runtime_error);
        }
    }
    SECTION("Utility") {}
}