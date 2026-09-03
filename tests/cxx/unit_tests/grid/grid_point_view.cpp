/*
 * Copyright 2025 NWChemEx-Project
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

#include "../test_helpers.hpp"
#include <chemist/grid/grid_point_view.hpp>
#include <utility>
#include <wtf/wtf.hpp>

using namespace chemist;
using types2test = std::pair<GridPoint, const GridPoint>;

namespace {

/// Verifies that @p view is a live alias of @p aliased (i.e., NOT a copy) by
/// mutating @p aliased directly and confirming the change is visible through
/// @p view.
template<typename ViewType>
void check_aliases(ViewType&& view, GridPoint& aliased) {
    // N.b. GridPoint::set_weight/set_x/set_y/set_z mutate in place (via a
    // FloatView) rather than assigning a new wtf::fp::Float outright, so
    // they do not invalidate `view`'s alias into `aliased`.
    aliased.set_weight(99.0);
    aliased.set_x(98.0);
    aliased.set_y(97.0);
    aliased.set_z(96.0);
    REQUIRE(wtf::fp::float_cast<double>(view.get_weight()) == 99.0);
    REQUIRE(wtf::fp::float_cast<double>(view.get_x()) == 98.0);
    REQUIRE(wtf::fp::float_cast<double>(view.get_y()) == 97.0);
    REQUIRE(wtf::fp::float_cast<double>(view.get_z()) == 96.0);
}

} // namespace

TEMPLATE_LIST_TEST_CASE("GridPointView", "", types2test) {
    GridPoint defaulted;
    GridPoint origin(1.2, 0.0, 0.0, 0.0);
    GridPoint non_origin(2.3, 3.4, 4.5, 5.6);

    using view_type = GridPointView<TestType>;
    view_type defaulted_view(defaulted);
    view_type origin_view(origin);
    view_type non_origin_view(non_origin);

    constexpr bool is_const = std::is_same_v<TestType, const GridPoint>;

    SECTION("Ctors and assignment") {
        SECTION("Alias GridPoint object") {
            REQUIRE(defaulted_view.get_weight() == 0.0);
            REQUIRE(defaulted_view.get_x() == 0.0);
            REQUIRE(defaulted_view.get_y() == 0.0);
            REQUIRE(defaulted_view.get_z() == 0.0);
        }

        SECTION("Alias state") {
            REQUIRE(origin_view.get_weight() == 1.2);
            REQUIRE(origin_view.get_x() == 0.0);
            REQUIRE(origin_view.get_y() == 0.0);
            REQUIRE(origin_view.get_z() == 0.0);
        }

        SECTION("Assign from value") {
            if constexpr(!is_const) {
                non_origin_view = origin;
                REQUIRE(non_origin_view.get_weight() == 1.2);
                REQUIRE(non_origin_view.get_x() == 0.0);
                REQUIRE(non_origin_view.get_y() == 0.0);
                REQUIRE(non_origin_view.get_z() == 0.0);
                // Assignment writes through to the aliased state
                REQUIRE(non_origin.get_weight() == 1.2);
            }
        }

        SECTION("copy") {
            view_type other(origin_view);
            REQUIRE(other.get_weight() == 1.2);
            check_aliases(other, origin);
        }

        SECTION("copy assignment") {
            auto pnon_origin = &(non_origin_view = origin_view);
            REQUIRE(pnon_origin == &non_origin_view);
            REQUIRE(non_origin_view.get_weight() == 1.2);
            check_aliases(non_origin_view, origin);
        }

        SECTION("move") {
            view_type copy(origin_view);
            view_type other(std::move(copy));
            REQUIRE(other.get_weight() == 1.2);
            check_aliases(other, origin);
        }

        SECTION("move assignment") {
            view_type copy(origin_view);
            auto pnon_origin_view = &(non_origin_view = std::move(copy));
            REQUIRE(pnon_origin_view == &non_origin_view);
            REQUIRE(non_origin_view.get_weight() == 1.2);
            check_aliases(non_origin_view, origin);
        }
    }

    SECTION("get_weight()") {
        REQUIRE(defaulted_view.get_weight() == 0.0);
        REQUIRE(origin_view.get_weight() == 1.2);
        REQUIRE(non_origin_view.get_weight() == 2.3);
    }

    SECTION("get_x()/get_y()/get_z()") {
        REQUIRE(defaulted_view.get_x() == 0.0);
        REQUIRE(defaulted_view.get_y() == 0.0);
        REQUIRE(defaulted_view.get_z() == 0.0);

        REQUIRE(non_origin_view.get_x() == 3.4);
        REQUIRE(non_origin_view.get_y() == 4.5);
        REQUIRE(non_origin_view.get_z() == 5.6);
    }

    SECTION("set_weight()/set_x()/set_y()/set_z()") {
        if constexpr(!is_const) {
            non_origin_view.set_weight(10.0);
            non_origin_view.set_x(11.0);
            non_origin_view.set_y(12.0);
            non_origin_view.set_z(13.0);
            REQUIRE(non_origin_view.get_weight() == 10.0);
            REQUIRE(non_origin_view.get_x() == 11.0);
            REQUIRE(non_origin_view.get_y() == 12.0);
            REQUIRE(non_origin_view.get_z() == 13.0);

            // Setting writes through to the aliased GridPoint
            REQUIRE(wtf::fp::float_cast<double>(non_origin.get_weight()) ==
                    10.0);
            REQUIRE(wtf::fp::float_cast<double>(non_origin.get_x()) == 11.0);
            REQUIRE(wtf::fp::float_cast<double>(non_origin.get_y()) == 12.0);
            REQUIRE(wtf::fp::float_cast<double>(non_origin.get_z()) == 13.0);
        }
    }

    SECTION("operator==") {
        // Default == Default
        REQUIRE(defaulted_view == view_type(defaulted));

        // Default == zero-initialized
        REQUIRE(defaulted_view == GridPoint(0.0, 0.0, 0.0, 0.0));

        // Different weights
        REQUIRE_FALSE(GridPoint(2.3, 0.0, 0.0, 0.0) == origin_view);

        // Different points
        REQUIRE_FALSE(non_origin_view == GridPoint(2.3, 0.0, 0.0, 0.0));
    }

    SECTION("operator!=") {
        // N.b., just negates operator== so spot checking is fine

        // Check on objects with equal state
        REQUIRE_FALSE(defaulted_view != GridPoint());
        REQUIRE_FALSE(GridPoint() != defaulted_view);

        // Check on objects with different state
        REQUIRE(origin_view != non_origin_view);
    }
}
