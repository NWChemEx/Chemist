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
#include <chemist/basis_set/primitive/detail_/primitive_pimpl.hpp>

using namespace chemist::basis_set::detail_;

TEMPLATE_TEST_CASE("PrimitivePIMPL", "", float, double) {
    using pimpl_type = PrimitivePIMPL<TestType>;

    using center_type = typename pimpl_type::prim_traits::center_type;
    center_type origin;
    center_type r0{1.0, 2.0, 3.0};

    pimpl_type defaulted;
    pimpl_type values(r0, 4.0, 5.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.m_coefficient == 0.0);
            REQUIRE(defaulted.m_exponent == 0.0);
            REQUIRE(defaulted.m_center == origin);
        }

        SECTION("Value") {
            REQUIRE(values.m_coefficient == 4.0);
            REQUIRE(values.m_exponent == 5.0);
            REQUIRE(values.m_center == r0);
        }

        SECTION("copy") {
            pimpl_type default_copy(defaulted);
            REQUIRE(default_copy.m_coefficient == 0.0);
            REQUIRE(default_copy.m_exponent == 0.0);
            REQUIRE(default_copy.m_center == origin);

            pimpl_type values_copy(values);
            REQUIRE(values_copy.m_coefficient == 4.0);
            REQUIRE(values_copy.m_exponent == 5.0);
            REQUIRE(values_copy.m_center == r0);
        }

        SECTION("move") {
            pimpl_type default_moved(std::move(defaulted));
            REQUIRE(default_moved.m_coefficient == 0.0);
            REQUIRE(default_moved.m_exponent == 0.0);
            REQUIRE(default_moved.m_center == origin);

            pimpl_type values_moved(std::move(values));
            REQUIRE(values_moved.m_coefficient == 4.0);
            REQUIRE(values_moved.m_exponent == 5.0);
            REQUIRE(values_moved.m_center == r0);
        }

        SECTION("copy assignment") {
            pimpl_type default_copy;
            auto pdefault_copy = &(default_copy = defaulted);
            REQUIRE(default_copy.m_coefficient == 0.0);
            REQUIRE(default_copy.m_exponent == 0.0);
            REQUIRE(default_copy.m_center == origin);
            REQUIRE(pdefault_copy == &default_copy);

            pimpl_type values_copy;
            auto pvalues_copy = &(values_copy = values);
            REQUIRE(values_copy.m_coefficient == 4.0);
            REQUIRE(values_copy.m_exponent == 5.0);
            REQUIRE(values_copy.m_center == r0);
            REQUIRE(pvalues_copy == &values_copy);
        }

        SECTION("move assignment") {
            pimpl_type default_moved;
            auto pdefault_moved = &(default_moved = std::move(defaulted));
            REQUIRE(default_moved.m_coefficient == 0.0);
            REQUIRE(default_moved.m_exponent == 0.0);
            REQUIRE(default_moved.m_center == origin);
            REQUIRE(pdefault_moved == &default_moved);

            pimpl_type values_moved;
            auto pvalues_moved = &(values_moved = std::move(values));
            REQUIRE(values_moved.m_coefficient == 4.0);
            REQUIRE(values_moved.m_exponent == 5.0);
            REQUIRE(values_moved.m_center == r0);
            REQUIRE(pvalues_moved == &values_moved);
        }
    }

    SECTION("operator==") {
        // Default vs default
        REQUIRE(defaulted == pimpl_type{});

        // Default vs non-default
        REQUIRE_FALSE(defaulted == values);

        // non-default: same values
        REQUIRE(values == pimpl_type(r0, 4.0, 5.0));

        // non-default: different origin
        REQUIRE_FALSE(values == pimpl_type(origin, 4.0, 5.0));

        // non-default: different coefficient
        REQUIRE_FALSE(values == pimpl_type(r0, 6.0, 5.0));

        // non-default: different exponent
        REQUIRE_FALSE(values == pimpl_type(r0, 4.0, 6.0));
    }
}
