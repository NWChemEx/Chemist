/*
 * Copyright 2024 NWChemEx-Project
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

#include "../../../test_helpers.hpp"
#include <chemist/electron/electron.hpp>
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <chemist/quantum_mechanics/operator/typedefs.hpp>

using namespace chemist;
using namespace chemist::qm_operator;
using namespace chemist::qm_operator::types;

/* Testing strategy
 *
 * For most of the derived classes representing operators, OperatorImpl
 * actually implements most of the functionality. Here we test those
 * implementations by ensuring the functionality works as intended for several
 * representative derived classes. The testing here should suffice for all
 * derived classes so long as the class does not override the implementation
 * supplied by OperatorImpl (in which case the derived class should also do
 * testing).
 *
 * - Note that visit_ is tested in the VisitorBase tests.
 * - We take this test as an opportunity to test the few functions actually
 *   implemented in OperatorBase as well.
 */

TEST_CASE("OperatorImpl") {
    t_e_type t;
    T_e_type T{ManyElectrons{2}};

    SECTION("n_electrons") {
        STATIC_REQUIRE(t.n_electrons() == 1);
        STATIC_REQUIRE(t.n_electrons() == 1);
    }

    SECTION("at()") {
        REQUIRE(t.at<0>() == Electron{});
        REQUIRE(T.at<0>() == ManyElectrons{2});
    }

    SECTION("at() const") {
        REQUIRE(std::as_const(t).at<0>() == Electron{});
        REQUIRE(std::as_const(T).at<0>() == ManyElectrons{2});
    }

    SECTION("operator==") {
        // n.b. Kinetic does not shadow operator== so this goes through
        // OperatorImpl::operator==. Also note it won't compile if the left and
        // right sides have different types
        REQUIRE(t == t_e_type{});
        REQUIRE(T == T_e_type{ManyElectrons{2}});
        REQUIRE_FALSE(T == T_e_type{ManyElectrons{1}});
    }

    SECTION("operator!=") {
        REQUIRE_FALSE(t != t_e_type{});
        REQUIRE_FALSE(T != T_e_type{ManyElectrons{2}});
        REQUIRE(T != T_e_type{ManyElectrons{1}});
    }

    SECTION("operator+") {
        chemist::dsl::Add<t_e_type, t_e_type> corr(t, t);
        REQUIRE((t + t) == corr);
    }

    SECTION("operator-") {
        chemist::dsl::Subtract<t_e_type, t_e_type> corr(t, t);
        REQUIRE((t - t) == corr);
    }

    SECTION("operator*") {
        chemist::dsl::Multiply<t_e_type, t_e_type> corr(t, t);
        REQUIRE((t * t) == corr);
    }

    SECTION("clone") {
        auto pt = t.clone();
        REQUIRE(pt->are_equal(t));

        auto pT = T.clone();
        REQUIRE(pT->are_equal(T));
    }

    SECTION("are_equal") {
        REQUIRE(t.are_equal(t_e_type{}));
        REQUIRE_FALSE(t.are_equal(T));
    }

    SECTION("are_different") {
        REQUIRE_FALSE(t.are_different(t_e_type{}));
        REQUIRE(t.are_different(T));
    }
}