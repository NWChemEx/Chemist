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

#include "../../test_helpers.hpp"
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/wavefunction/cmos.hpp>

/* Testing strategy.
 *
 * CMOs is basically a strong type. In must override clone and are_equal to
 * account for the new most derived type. We test that the ctors from the base
 * class still work and that it interacts properly with Natural<AOs>
 * objects.
 */

using namespace chemist::wavefunction;

TEST_CASE("CMOs") {
    using to_space_type   = CMOs;
    using from_space_type = typename to_space_type::from_space_type;
    using tensor_type     = typename to_space_type::transform_type;
    using natural_type    = Natural<from_space_type>;

    from_space_type aos_defaulted;
    from_space_type aos(test_chemist::h2_basis());

    tensor_type c_default;
    tensor_type c({{1.0, 0.0}, {0.0, 1.0}});

    tensor_type e_default;
    tensor_type e({-0.5, 0.5});

    to_space_type defaulted;
    to_space_type value_defaulted(e_default, aos_defaulted, c_default);
    to_space_type value(e, aos, c);

    SECTION("CTors and assignment") {
        SECTION("Defaulted") {
            REQUIRE(defaulted.diagonalized_matrix() == e_default);
            REQUIRE(defaulted.from_space() == aos_defaulted);
            REQUIRE(defaulted.transform() == c_default);
        }

        SECTION("Value") {
            REQUIRE(value_defaulted.diagonalized_matrix() == e_default);
            REQUIRE(value_defaulted.from_space() == aos_defaulted);
            REQUIRE(value_defaulted.transform() == c_default);

            REQUIRE(value.diagonalized_matrix() == e);
            REQUIRE(value.from_space() == aos);
            REQUIRE(value.transform() == c);
        }

        test_chemist::test_copy_and_move(defaulted, value_defaulted, value);
    }

    SECTION("clone") {
        REQUIRE(defaulted.clone()->are_equal(defaulted));
        REQUIRE(value_defaulted.clone()->are_equal(value_defaulted));
        REQUIRE(value.clone()->are_equal(value));
    }

    SECTION("compare with Natural<AOs>") {
        // operator== and operator!= should go through Transformed and not
        // compare polymorphically

        natural_type other_defaulted(e_default, aos_defaulted, c_default);

        REQUIRE(defaulted == other_defaulted);
        REQUIRE(other_defaulted == defaulted);
        REQUIRE(defaulted != natural_type(e, aos, c_default));
        REQUIRE(natural_type(e, aos, c_default) != defaulted);

        // are_equal should fail because not the same most derived
        REQUIRE_FALSE(other_defaulted.are_equal(defaulted));
        REQUIRE_FALSE(defaulted.are_equal(other_defaulted));
    }
}
