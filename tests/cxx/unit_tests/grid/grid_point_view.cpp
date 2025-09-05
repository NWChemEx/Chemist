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

using namespace chemist;
using point_type = GridPoint::point_type;
using types2test = std::pair<GridPoint, const GridPoint>;

namespace {
template<typename LHSType, typename RHSType>
void check_addresses(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.weight() == &rhs.weight());
    REQUIRE(&lhs.point().x() == &rhs.point().x());
    REQUIRE(&lhs.point().y() == &rhs.point().y());
    REQUIRE(&lhs.point().z() == &rhs.point().z());
}

} // namespace
TEMPLATE_LIST_TEST_CASE("GridPointView", "", types2test) {
    point_type pdefaulted;
    point_type porigin(0.0, 0.0, 0.0);
    point_type pnon_origin(3.4, 4.5, 5.6);

    GridPoint defaulted;
    GridPoint origin(1.2, 0.0, 0.0, 0.0);
    GridPoint non_origin(2.3, pnon_origin);

    using view_type = GridPointView<TestType>;
    view_type defaulted_view(defaulted);
    view_type origin_view(origin.weight(), origin.point());
    view_type non_origin_view(non_origin);

    constexpr bool is_const = std::is_same_v<TestType, const GridPoint>;

    SECTION("Ctors and assignment") {
        SECTION("Alias GridPoint object") {
            REQUIRE(defaulted_view.weight() == 0.0);
            REQUIRE(defaulted_view.point() == pdefaulted);
        }

        SECTION("Alias state") {
            REQUIRE(origin_view.weight() == 1.2);
            REQUIRE(origin_view.point() == porigin);
        }

        SECTION("Assign from value") {
            if constexpr(!is_const) {
                non_origin_view = origin;
                REQUIRE(non_origin_view.weight() == 1.2);
                REQUIRE(non_origin_view.point() == porigin);
            }
        }

        SECTION("copy") {
            view_type other(origin_view);
            REQUIRE(other.weight() == 1.2);
            REQUIRE(other.point() == porigin);
            check_addresses(other, origin_view);
        }

        SECTION("copy assignment") {
            auto pnon_origin = &(non_origin_view = origin_view);
            REQUIRE(pnon_origin == &non_origin_view);
            REQUIRE(non_origin_view.weight() == 1.2);
            REQUIRE(non_origin_view.point() == porigin);
            check_addresses(non_origin_view, origin_view);
        }

        SECTION("move") {
            view_type copy(origin_view);
            view_type other(std::move(copy));
            REQUIRE(other.weight() == 1.2);
            REQUIRE(other.point() == porigin);
            check_addresses(other, origin_view);
        }

        SECTION("move assignment") {
            view_type copy(origin_view);
            auto pnon_origin = &(non_origin_view = std::move(copy));
            REQUIRE(non_origin_view.weight() == 1.2);
            REQUIRE(non_origin_view.point() == porigin);
            check_addresses(non_origin_view, origin_view);
        }
    }

    SECTION("weight()") {
        REQUIRE(defaulted_view.weight() == 0.0);
        REQUIRE(origin_view.weight() == 1.2);
        REQUIRE(non_origin_view.weight() == 2.3);
    }

    SECTION("weight() const") {
        REQUIRE(std::as_const(defaulted_view).weight() == 0.0);
        REQUIRE(std::as_const(origin_view).weight() == 1.2);
        REQUIRE(std::as_const(non_origin_view).weight() == 2.3);
    }

    SECTION("point()") {
        REQUIRE(defaulted_view.point() == pdefaulted);
        REQUIRE(origin_view.point() == porigin);
        REQUIRE(non_origin_view.point() == pnon_origin);
    }

    SECTION("point() const") {
        REQUIRE(std::as_const(defaulted_view).point() == pdefaulted);
        REQUIRE(std::as_const(origin_view).point() == porigin);
        REQUIRE(std::as_const(non_origin_view).point() == pnon_origin);
    }

    SECTION("operator==") {
        // Default == Default
        REQUIRE(defaulted_view == view_type(defaulted));

        // Default == zero-initialized
        REQUIRE(defaulted_view == GridPoint(0.0, 0.0, 0.0, 0.0));

        // Different weights
        REQUIRE_FALSE(GridPoint(2.3, pdefaulted) == origin_view);

        // Different points
        REQUIRE_FALSE(non_origin_view == GridPoint(2.3, porigin));
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
