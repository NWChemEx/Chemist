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
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <chemist/quantum_mechanics/wavefunction/natural.hpp>

TEST_CASE("Natural") {
    using from_space_type = chemist::wavefunction::AOs;
    using natural_type    = chemist::wavefunction::Natural<from_space_type>;
    using tensor_type     = typename natural_type::transform_type;

    from_space_type aos_defaulted;
    from_space_type aos(test_chemist::h2_basis());

    tensor_type c_default;

    // Not real data, just for unit testing!
    tensor_type c({{1.0, 0.0}, {0.0, 1.0}});
    tensor_type e({-0.5, 0.5});

    natural_type defaulted;
    natural_type value_defaulted(c_default, aos_defaulted, c_default);
    natural_type value(e, aos, c);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.diagonalized_matrix() == c_default);
            REQUIRE(defaulted.from_space() == aos_defaulted);
            REQUIRE(defaulted.transform() == c_default);
        }

        SECTION("Value") {
            REQUIRE(value_defaulted.diagonalized_matrix() == c_default);
            REQUIRE(value_defaulted.from_space() == aos_defaulted);
            REQUIRE(value_defaulted.transform() == c_default);

            REQUIRE(value.diagonalized_matrix() == e);
            REQUIRE(value.from_space() == aos);
            REQUIRE(value.transform() == c);
        }

        test_chemist::test_copy_and_move(defaulted, value_defaulted, value);
    }

    SECTION("diagonalized_matrix()") {
        REQUIRE(defaulted.diagonalized_matrix() == c_default);
        REQUIRE(value_defaulted.diagonalized_matrix() == c_default);
        REQUIRE(value.diagonalized_matrix() == e);
    }

    SECTION("diagonalized_matrix() const") {
        REQUIRE(std::as_const(defaulted).diagonalized_matrix() == c_default);
        REQUIRE(std::as_const(value_defaulted).diagonalized_matrix() ==
                c_default);
        REQUIRE(std::as_const(value).diagonalized_matrix() == e);
    }

    SECTION("operator==") {
        SECTION("Default vs. default") { REQUIRE(defaulted == natural_type{}); }

        SECTION("Default vs. default value") {
            REQUIRE(defaulted == value_defaulted);
        }

        SECTION("Default vs. value") { REQUIRE_FALSE(defaulted == value); }

        SECTION("Value vs. same value") {
            REQUIRE(value == natural_type(e, aos, c));
        }

        SECTION("Value vs. different diagonal") {
            tensor_type e2({1.0, 1.0});
            REQUIRE_FALSE(value == natural_type(e2, aos, c));
        }

        SECTION("Value vs. different Transform base") {
            tensor_type c2({{1.0, 1.0}, {1.0, 1.0}});
            REQUIRE_FALSE(value == natural_type(e, aos, c2));
        }
    }

    SECTION("operator!=") {
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != value_defaulted);
    }
}
