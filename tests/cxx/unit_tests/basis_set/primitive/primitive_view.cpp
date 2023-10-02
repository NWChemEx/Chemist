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
#include <chemist/basis_set/primitive/primitive_view.hpp>

using namespace chemist::basis_set;

template<typename TypeBeingTested, typename CorrType>
void compare_addresses(TypeBeingTested&& lhs, CorrType&& rhs) {
    // Check exponent and coefficients are an alias
    REQUIRE(&lhs.coefficient() == &rhs.coefficient());
    REQUIRE(&lhs.exponent() == &rhs.exponent());

    // Ensure coordinates are aliases
    REQUIRE(&lhs.center().x() == &rhs.center().x());
    REQUIRE(&lhs.center().y() == &rhs.center().y());
    REQUIRE(&lhs.center().z() == &rhs.center().z());
}

TEMPLATE_TEST_CASE("PrimitiveView", "", float, double) {
    using prim_type   = Primitive<TestType>;
    using center_type = typename prim_type::center_type;
    using view_type   = PrimitiveView<prim_type>;
    using const_view  = PrimitiveView<const prim_type>;

    center_type r0(1.0, 2.0, 3.0);
    prim_type defaulted;
    prim_type values(4.0, 5.0, r0);

    view_type defaulted_view(defaulted);
    view_type values_view(values);

    const_view const_defaulted_view(defaulted);
    const_view const_values_view(values);

    SECTION("Ctors and assignment") {
        SECTION("coef, exp, x, y, z") {
            view_type p(values.coefficient(), values.exponent(),
                        values.center().x(), values.center().y(),
                        values.center().z());

            compare_addresses(p, values);

            // Check values as a sanity check
            REQUIRE(p.coefficient() == 4.0);
            REQUIRE(p.exponent() == 5.0);
            REQUIRE(p.center() == r0);

            const_view cp(values.coefficient(), values.exponent(),
                          values.center().x(), values.center().y(),
                          values.center().z());

            compare_addresses(cp, values);
            // Check values as a sanity check
            REQUIRE(cp.coefficient() == 4.0);
            REQUIRE(cp.exponent() == 5.0);
            REQUIRE(cp.center() == r0);
        }

        SECTION("coef, exp, r0") {
            view_type p(values.coefficient(), values.exponent(),
                        values.center());

            compare_addresses(p, values);

            // Sanity check values
            REQUIRE(p.coefficient() == 4.0);
            REQUIRE(p.exponent() == 5.0);
            REQUIRE(p.center() == r0);

            const_view cp(values.coefficient(), values.exponent(),
                          values.center());

            compare_addresses(cp, values);

            // Sanity check values
            REQUIRE(cp.coefficient() == 4.0);
            REQUIRE(cp.exponent() == 5.0);
            REQUIRE(cp.center() == r0);
        }

        SECTION("primitive") {
            // View of a mutable null Primitive
            REQUIRE(defaulted_view.is_null());

            // View of a mutable non-null Primitive
            compare_addresses(values_view, values);
            REQUIRE(values_view.coefficient() == 4.0);
            REQUIRE(values_view.exponent() == 5.0);
            REQUIRE(values_view.center() == r0);

            // View of a read-only null Primitive
            REQUIRE(const_defaulted_view.is_null());

            // View of a read-only non-null Primitive
            compare_addresses(const_values_view, values);
            REQUIRE(const_values_view.coefficient() == 4.0);
            REQUIRE(const_values_view.exponent() == 5.0);
            REQUIRE(const_values_view.center() == r0);
        }

        SECTION("assign Primitive") {
            REQUIRE_THROWS_AS(defaulted_view = values, std::runtime_error);
            REQUIRE_THROWS_AS(values_view = defaulted, std::runtime_error);

            values_view = prim_type(1.0, 2.0, 3.0, 4.0, 5.0);
            REQUIRE(values_view.coefficient() == 1.0);
            REQUIRE(values_view.exponent() == 2.0);
            REQUIRE(values_view.center() == center_type(3.0, 4.0, 5.0));
        }

        SECTION("mutable view to read-only") {
            const_view cp(values_view);

            compare_addresses(cp, values);
            REQUIRE(cp.coefficient() == 4.0);
            REQUIRE(cp.exponent() == 5.0);
            REQUIRE(cp.center() == r0);
        }

        SECTION("copy") {
            // Mutable view of a null Primitive
            view_type defaulted_copy(defaulted_view);
            REQUIRE(defaulted_copy.is_null());

            // Mutable view of a non-null Primitive
            view_type values_copy(values_view);
            compare_addresses(values_copy, values);
            REQUIRE(values_copy.coefficient() == 4.0);
            REQUIRE(values_copy.exponent() == 5.0);
            REQUIRE(values_copy.center() == r0);

            // Read-only view of a null primitive
            const_view const_defaulted_copy(const_defaulted_view);
            REQUIRE(const_defaulted_copy.is_null());

            // Read-only view of a non-null Primitive
            const_view const_values_copy(const_values_view);
            compare_addresses(const_values_copy, values);
            REQUIRE(const_values_copy.coefficient() == 4.0);
            REQUIRE(const_values_copy.exponent() == 5.0);
            REQUIRE(const_values_copy.center() == r0);
        }

        SECTION("copy assignment") {
            // Mutable view of a null Primitive
            view_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted_view);
            REQUIRE(defaulted_copy.is_null());
            REQUIRE(pdefaulted_copy == &defaulted_copy);

            // Mutable view of a non-null Primitive
            view_type values_copy;
            auto pvalues_copy = &(values_copy = values_view);
            compare_addresses(values_copy, values);
            REQUIRE(values_copy.coefficient() == 4.0);
            REQUIRE(values_copy.exponent() == 5.0);
            REQUIRE(values_copy.center() == r0);
            REQUIRE(pvalues_copy == &values_copy);

            // Read-only view of a null primitive
            const_view const_defaulted_copy;
            auto pconst_defaulted_copy =
              &(const_defaulted_copy = const_defaulted_view);
            REQUIRE(const_defaulted_copy.is_null());
            REQUIRE(pconst_defaulted_copy == &const_defaulted_copy);

            // Read-only view of a non-null Primitive
            const_view const_values_copy;
            auto pconst_values_copy = &(const_values_copy = const_values_view);
            compare_addresses(const_values_copy, values);
            REQUIRE(const_values_copy.coefficient() == 4.0);
            REQUIRE(const_values_copy.exponent() == 5.0);
            REQUIRE(const_values_copy.center() == r0);
            REQUIRE(pconst_values_copy == &const_values_copy);
        }

        SECTION("move") {
            // Mutable view of a null Primitive
            view_type defaulted_move(std::move(defaulted_view));
            REQUIRE(defaulted_move.is_null());

            // Mutable view of a non-null Primitive
            view_type values_move(std::move(values_view));
            compare_addresses(values_move, values);
            REQUIRE(values_move.coefficient() == 4.0);
            REQUIRE(values_move.exponent() == 5.0);
            REQUIRE(values_move.center() == r0);

            // Read-only view of a null primitive
            const_view const_defaulted_move(std::move(const_defaulted_view));
            REQUIRE(const_defaulted_move.is_null());

            // Read-only view of a non-null Primitive
            const_view const_values_move(std::move(const_values_view));
            compare_addresses(const_values_move, values);
            REQUIRE(const_values_move.coefficient() == 4.0);
            REQUIRE(const_values_move.exponent() == 5.0);
            REQUIRE(const_values_move.center() == r0);
        }

        SECTION("move assignment") {
            // Mutable view of a null Primitive
            view_type defaulted_move;
            auto pdefaulted_move =
              &(defaulted_move = std::move(defaulted_view));
            REQUIRE(defaulted_move.is_null());
            REQUIRE(pdefaulted_move == &defaulted_move);

            // Mutable view of a non-null Primitive
            view_type values_move;
            auto pvalues_move = &(values_move = std::move(values_view));
            compare_addresses(values_move, values);
            REQUIRE(values_move.coefficient() == 4.0);
            REQUIRE(values_move.exponent() == 5.0);
            REQUIRE(values_move.center() == r0);
            REQUIRE(pvalues_move == &values_move);

            // Read-only view of a null primitive
            const_view const_defaulted_move;
            auto pconst_defaulted_move =
              &(const_defaulted_move = std::move(const_defaulted_view));
            REQUIRE(const_defaulted_move.is_null());
            REQUIRE(pconst_defaulted_move == &const_defaulted_move);

            // Read-only view of a non-null Primitive
            const_view const_values_move;
            auto pconst_values_move =
              &(const_values_move = std::move(const_values_view));
            compare_addresses(const_values_move, values);
            REQUIRE(const_values_move.coefficient() == 4.0);
            REQUIRE(const_values_move.exponent() == 5.0);
            REQUIRE(const_values_move.center() == r0);
            REQUIRE(pconst_values_move == &const_values_move);
        }
    }

    SECTION("getters/setters") {
        using re = std::runtime_error;

        SECTION("center()") {
            REQUIRE_THROWS_AS(defaulted_view.center(), re);
            REQUIRE_THROWS_AS(const_defaulted_view.center(), re);

            REQUIRE(values_view.center() == r0);
            REQUIRE(const_values_view.center() == r0);
        }

        SECTION("center() const") {
            REQUIRE_THROWS_AS(std::as_const(defaulted_view).center(), re);
            REQUIRE_THROWS_AS(std::as_const(const_defaulted_view).center(), re);

            REQUIRE(std::as_const(values_view).center() == r0);
            REQUIRE(std::as_const(const_values_view).center() == r0);
        }

        SECTION("coefficient()") {
            REQUIRE_THROWS_AS(defaulted_view.coefficient(), re);
            REQUIRE_THROWS_AS(const_defaulted_view.coefficient(), re);

            REQUIRE(values_view.coefficient() == 4.0);
            REQUIRE(const_values_view.coefficient() == 4.0);
        }

        SECTION("coefficient() const") {
            REQUIRE_THROWS_AS(std::as_const(defaulted_view).coefficient(), re);
            REQUIRE_THROWS_AS(std::as_const(const_defaulted_view).coefficient(),
                              re);

            REQUIRE(std::as_const(values_view).coefficient() == 4.0);
            REQUIRE(std::as_const(const_values_view).coefficient() == 4.0);
        }

        SECTION("exponent()") {
            REQUIRE_THROWS_AS(defaulted_view.exponent(), re);
            REQUIRE_THROWS_AS(const_defaulted_view.exponent(), re);

            REQUIRE(values_view.exponent() == 5.0);
            REQUIRE(const_values_view.exponent() == 5.0);
        }

        SECTION("exponent() const") {
            REQUIRE_THROWS_AS(std::as_const(defaulted_view).exponent(), re);
            REQUIRE_THROWS_AS(std::as_const(const_defaulted_view).exponent(),
                              re);

            REQUIRE(std::as_const(values_view).exponent() == 5.0);
            REQUIRE(std::as_const(const_values_view).exponent() == 5.0);
        }
    }

    SECTION("utility functions") {
        SECTION("swap") {
            defaulted_view.swap(values_view);
            compare_addresses(defaulted_view, values);
            REQUIRE(values_view.is_null());
        }

        SECTION("is_null") {
            REQUIRE(defaulted_view.is_null());
            REQUIRE_FALSE(values_view.is_null());
            REQUIRE(const_defaulted_view.is_null());
            REQUIRE_FALSE(const_values_view.is_null());
        }

        SECTION("operator==(PrimitiveView)") {
            // null vs null
            REQUIRE(defaulted_view == view_type{});
            REQUIRE(const_defaulted_view == const_view{});

            // null vs non-null
            REQUIRE_FALSE(defaulted_view == values_view);
            REQUIRE_FALSE(const_defaulted_view == const_values_view);

            // non-null: same values
            REQUIRE(values_view == view_type(values));
            REQUIRE(const_values_view == const_view(values));

            // non-null: different coefficient
            prim_type diff_coef(6.0, 5.0, r0);
            REQUIRE_FALSE(values_view == view_type(diff_coef));
            REQUIRE_FALSE(const_values_view == const_view(diff_coef));

            // non-null: different exponent
            prim_type diff_exp(4.0, 6.0, r0);
            REQUIRE_FALSE(values_view == view_type(diff_exp));
            REQUIRE_FALSE(const_values_view == const_view(diff_exp));

            // non-null: different center
            prim_type diff_center(4.0, 5.0, center_type{});
            REQUIRE_FALSE(values_view == view_type(diff_center));
            REQUIRE_FALSE(const_values_view == const_view(diff_center));

            // Can mix mutable and read-only views symmetrically
            REQUIRE(values_view == const_values_view);
            REQUIRE(const_values_view == values_view);
        }

        SECTION("operator==(Primitive)") {
            // N.B. We need to check each operation for symmetry too

            // null vs null
            REQUIRE(defaulted_view == prim_type{});
            REQUIRE(prim_type{} == defaulted_view);
            REQUIRE(const_defaulted_view == prim_type{});
            REQUIRE(prim_type{} == const_defaulted_view);

            // null vs non-null
            REQUIRE_FALSE(defaulted_view == values);
            REQUIRE_FALSE(values == defaulted_view);
            REQUIRE_FALSE(const_defaulted_view == values);
            REQUIRE_FALSE(values == const_defaulted_view);

            // non-null: same values
            REQUIRE(values_view == values);
            REQUIRE(values == values_view);
            REQUIRE(const_values_view == values);
            REQUIRE(values == const_values_view);

            // non-null: different coefficient
            prim_type diff_coef(6.0, 5.0, r0);
            REQUIRE_FALSE(values_view == diff_coef);
            REQUIRE_FALSE(diff_coef == values_view);
            REQUIRE_FALSE(const_values_view == diff_coef);
            REQUIRE_FALSE(diff_coef == const_values_view);

            // non-null: different exponent
            prim_type diff_exp(4.0, 6.0, r0);
            REQUIRE_FALSE(values_view == diff_exp);
            REQUIRE_FALSE(diff_exp == values_view);
            REQUIRE_FALSE(const_values_view == diff_exp);
            REQUIRE_FALSE(diff_exp == const_values_view);

            // non-null: different center
            prim_type diff_center(4.0, 5.0, center_type{});
            REQUIRE_FALSE(values_view == diff_center);
            REQUIRE_FALSE(diff_center == values_view);
            REQUIRE_FALSE(const_values_view == diff_center);
            REQUIRE_FALSE(diff_center == const_values_view);

            // Can mix mutable and read-only views symmetrically
            REQUIRE(values == const_values_view);
            REQUIRE(const_values_view == values);
        }

        SECTION("operator!=(PrimitiveView)") {
            // Operator!= simply negates operator==, suffices to spot check
            REQUIRE_FALSE(values_view != const_values_view);
            REQUIRE_FALSE(const_values_view != values_view);
        }

        SECTION("operator!=(Primitive)") {
            // Operator!= simply negates operator==, suffices to spot check
            REQUIRE_FALSE(values != const_values_view);
            REQUIRE_FALSE(const_values_view != values);
        }
    }
}
