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
#include <chemist/basis_set2/shell/detail_/shell_view_pimpl.hpp>

using namespace chemist::basis_set;
using namespace chemist::basis_set::detail_;

TEMPLATE_TEST_CASE("ShellViewPIMPL", "", float, double) {
    using prim_type    = Primitive<TestType>;
    using cg_type      = ContractedGaussian<prim_type>;
    using shell_type   = Shell<cg_type>;
    using shell_traits = ShellTraits<shell_type>;
    using pimpl_type   = ShellViewPIMPL<shell_type>;

    using pure_type    = typename shell_traits::pure_type;
    using l_type       = typename shell_traits::angular_momentum_type;
    using center_type  = typename shell_traits::center_type;
    using coeff_vector = std::vector<typename shell_traits::coefficient_type>;
    using exp_vector   = std::vector<typename shell_traits::exponent_type>;

    // Inputs for ctors
    pure_type cart{0}, pure{1};
    l_type l0{0}, l1{1};
    center_type r0{7.0, 8.0, 9.0};
    coeff_vector cs{1.0, 2.0, 3.0};
    exp_vector es{4.0, 5.0, 6.0};
    cg_type cg0, cg1(cs.begin(), cs.end(), es.begin(), es.end(), r0);

    // Construct test instances
    pimpl_type cartesian(cart, l1, cg1);

    SECTION("Ctor") {
        REQUIRE(cartesian.m_cg == cg1);
        REQUIRE(cartesian.m_l == l1);
        REQUIRE(cartesian.m_pure == cart);
    }

    SECTION("operator==") {
        SECTION("eqvuivalent") {
            REQUIRE(cartesian == pimpl_type{cart, l1, cg1});
        }
        SECTION("Different contracted Gaussian") {
            REQUIRE_FALSE(cartesian == pimpl_type{cart, l1, cg0});
        }
        SECTION("Different purity") {
            REQUIRE_FALSE(cartesian == pimpl_type{pure, l1, cg1});
        }
        SECTION("Different angular momentum") {
            REQUIRE_FALSE(cartesian == pimpl_type{cart, l0, cg1});
        }
    }
}
