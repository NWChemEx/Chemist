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

#include "../catch.hpp"
#include <chemist/basis_set/primitive.hpp>
#include <utility>

TEMPLATE_TEST_CASE("Primitive", "", float, double) {
    using prim_type   = chemist::basis_set::Primitive<TestType>;
    using center_type = typename prim_type::center_type;

    center_type origin, r0(1.0, 2.0, 3.0);

    prim_type defaulted;
    prim_type values(4.0, 5.0, r0);

    SECTION("Ctors and assignment") {
        SECTION("default") { REQUIRE(defaulted.is_null()); }

        SECTION("T, T, T, T, T") {
            prim_type prim(4.0, 5.0, 1.0, 2.0, 3.0);
            REQUIRE(prim.coefficient() == 4.0);
            REQUIRE(prim.exponent() == 5.0);
            REQUIRE(prim.center() == r0);
        }

        SECTION("T, T, PointView") {
            REQUIRE(values.coefficient() == 4.0);
            REQUIRE(values.exponent() == 5.0);

            // Check center's value and ensure deep copy of center
            REQUIRE(values.center() == r0);
            REQUIRE(&values.center().x() != &r0.x());
            REQUIRE(&values.center().y() != &r0.y());
            REQUIRE(&values.center().z() != &r0.z());
        }

        SECTION("Copy") {
            prim_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy.is_null());

            prim_type values_copy(values);
            REQUIRE(values_copy.coefficient() == 4.0);
            REQUIRE(values_copy.exponent() == 5.0);
            REQUIRE(values_copy.center() == r0);
        }

        SECTION("Copy assignment") {
            prim_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(defaulted_copy.is_null());
            REQUIRE(pdefaulted_copy == &defaulted_copy);

            prim_type values_copy;
            auto pvalues_copy = &(values_copy = values);
            REQUIRE(values_copy.coefficient() == 4.0);
            REQUIRE(values_copy.exponent() == 5.0);
            REQUIRE(values_copy.center() == r0);
            REQUIRE(pvalues_copy == &values_copy);
        }

        SECTION("Move") {
            prim_type defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_move.is_null());

            prim_type values_move(std::move(values));
            REQUIRE(values_move.coefficient() == 4.0);
            REQUIRE(values_move.exponent() == 5.0);
            REQUIRE(values_move.center() == r0);
        }

        SECTION("Move assignment") {
            prim_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(defaulted_move.is_null());
            REQUIRE(pdefaulted_move == &defaulted_move);

            prim_type values_move;
            auto pvalues_move = &(values_move = std::move(values));
            REQUIRE(values_move.coefficient() == 4.0);
            REQUIRE(values_move.exponent() == 5.0);
            REQUIRE(values_move.center() == r0);
            REQUIRE(pvalues_move == &values_move);
        }
    }

    SECTION("center") {
        // For default objects, this should allocate a PIMPL and then return
        // the origin (it also should have set the coefficient and exponent to
        // 0)
        REQUIRE(defaulted.center() == origin);
        REQUIRE(defaulted.coefficient() == 0.0);
        REQUIRE(defaulted.exponent() == 0.0);

        // We should be able to write to the instance too
        defaulted.center().x() = 1.0;
        defaulted.center().y() = 2.0;
        defaulted.center().z() = 3.0;
        REQUIRE(defaulted.center() == r0);

        // Calling center on an already set object shouldn't change anythign
        REQUIRE(values.center() == r0);
        REQUIRE(values.coefficient() == 4.0);
        REQUIRE(values.exponent() == 5.0);
    }

    SECTION("center() const") {
        // For default objects, this should throw
        const auto& cdefaulted = std::as_const(defaulted);
        REQUIRE_THROWS_AS(cdefaulted.center(), std::runtime_error);
        REQUIRE(std::as_const(values).center() == r0);
    }

    SECTION("coefficient()") {
        // For default objects, this should allocate a PIMPL and then return 0
        // (it also should have set the exponent to 0 and the center to the
        // origin)
        REQUIRE(defaulted.coefficient() == 0.0);
        REQUIRE(defaulted.exponent() == 0.0);
        REQUIRE(defaulted.center() == origin);

        // We should be able to write to the instance too
        defaulted.coefficient() = 4.0;
        REQUIRE(defaulted.coefficient() == 4.0);

        // Calling center on an already set object shouldn't change anything
        REQUIRE(values.coefficient() == 4.0);
        REQUIRE(values.exponent() == 5.0);
        REQUIRE(values.center() == r0);
    }

    SECTION("center() const") {
        // For default objects, this should throw
        const auto& cdefaulted = std::as_const(defaulted);
        REQUIRE_THROWS_AS(cdefaulted.coefficient(), std::runtime_error);
        REQUIRE(std::as_const(values).coefficient() == 4.0);
    }

    SECTION("exponent()") {
        // For default objects, this should allocate a PIMPL and then return 0
        // (it also should have set the coefficient to 0 and the center to the
        // origin)
        REQUIRE(defaulted.exponent() == 0.0);
        REQUIRE(defaulted.center() == origin);
        REQUIRE(defaulted.coefficient() == 0.0);

        // We should be able to write to the instance too
        defaulted.exponent() = 5.0;
        REQUIRE(defaulted.exponent() == 5.0);

        // Calling center on an already set object shouldn't change anything
        REQUIRE(values.exponent() == 5.0);
        REQUIRE(values.center() == r0);
        REQUIRE(values.coefficient() == 4.0);
    }

    SECTION("center() const") {
        // For default objects, this should throw
        const auto& cdefaulted = std::as_const(defaulted);
        REQUIRE_THROWS_AS(cdefaulted.exponent(), std::runtime_error);
        REQUIRE(std::as_const(values).exponent() == 5.0);
    }

    SECTION("is_null") {
        REQUIRE(defaulted.is_null());

        SECTION("Calling center() on a null object makes it non-null") {
            defaulted.center();
            REQUIRE_FALSE(defaulted.is_null());
        }

        SECTION("Calling coefficient() on a null object makes it non-null") {
            defaulted.coefficient();
            REQUIRE_FALSE(defaulted.is_null());
        }

        SECTION("Calling exponent() on a null object makes it non-null") {
            defaulted.exponent();
            REQUIRE_FALSE(defaulted.is_null());
        }

        REQUIRE_FALSE(values.is_null());
    }

    SECTION("swap") {
        prim_type defaulted_copy(defaulted);
        prim_type values_copy(values);

        defaulted.swap(values);

        REQUIRE(defaulted == values_copy);
        REQUIRE(values == defaulted_copy);
    }

    SECTION("operator==") {
        // Default vs. default
        REQUIRE(defaulted == prim_type{});

        // Default vs. non-default
        REQUIRE_FALSE(defaulted == values);

        // Default vs. zero
        REQUIRE_FALSE(defaulted == prim_type(0.0, 0.0, 0.0, 0.0, 0.0));

        // Non-default: same values
        REQUIRE(values == prim_type(4.0, 5.0, 1.0, 2.0, 3.0));

        // Non-default: different coefficient
        REQUIRE_FALSE(values == prim_type(6.0, 5.0, 1.0, 2.0, 3.0));

        // Non-default: different exponent
        REQUIRE_FALSE(values == prim_type(4.0, 6.0, 1.0, 2.0, 3.0));

        // Non-default: different x
        REQUIRE_FALSE(values == prim_type(4.0, 5.0, 6.0, 2.0, 3.0));

        // Non-default: different y
        REQUIRE_FALSE(values == prim_type(4.0, 5.0, 1.0, 6.0, 3.0));

        // Non-default: different z
        REQUIRE_FALSE(values == prim_type(4.0, 5.0, 1.0, 2.0, 6.0));
    }

    SECTION("operator!=") {
        // This just negates operator==, so okay to spot check
        REQUIRE(defaulted != values);
    }
}
