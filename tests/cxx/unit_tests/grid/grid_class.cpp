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
#include <chemist/grid/grid_class.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace chemist;

namespace {

// Makes a Point with the same coordinates as the GridPoint @p gp
experimental::Point to_point(const GridPoint& gp) {
    return experimental::Point(gp.get_x().value<double>(),
                               gp.get_y().value<double>(),
                               gp.get_z().value<double>());
}

} // namespace

TEST_CASE("Grid") {
    using point_set_type = Grid::point_set_type;
    using point_set_view = experimental::PointSetView<point_set_type>;
    using const_point_set_view =
      experimental::PointSetView<const point_set_type>;

    Grid defaulted;

    std::vector<GridPoint> points;
    points.push_back(GridPoint());
    points.push_back(GridPoint(0.1, 1.2, 2.3, 3.4));
    Grid range(points.begin(), points.end());

    SECTION("Ctors and assignment") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Range") {
            REQUIRE(range.size() == 2);
            REQUIRE(range.at(0) == points.at(0));
            REQUIRE(range.at(1) == points.at(1));
        }
        SECTION("Weights and points") {
            wtf::buffer::FloatBuffer weights(std::vector<double>{0.0, 0.1});
            point_set_type ps{to_point(points.at(0)), to_point(points.at(1))};
            Grid grid(std::move(weights), std::move(ps));
            REQUIRE(grid.size() == 2);
            REQUIRE(grid == range);
        }
        SECTION("Weights and points (size mismatch)") {
            wtf::buffer::FloatBuffer weights(std::vector<double>{0.0});
            point_set_type ps{to_point(points.at(0)), to_point(points.at(1))};
            REQUIRE_THROWS_AS(Grid(std::move(weights), std::move(ps)),
                              std::invalid_argument);
        }
        test_chemist::test_copy_and_move(defaulted, range);

        SECTION("Copy keeps the sizes") {
            range.set_radial_size(10);
            range.set_angular_size(14);
            Grid copy(range);
            REQUIRE(copy.get_radial_size() == 10);
            REQUIRE(copy.get_angular_size() == 14);
        }
    }

    SECTION("at_()") {
        REQUIRE(range.at(0) == points.at(0));
        REQUIRE(range.at(1) == points.at(1));
    }

    SECTION("ato_() const") {
        REQUIRE(std::as_const(range).at(0) == points.at(0));
        REQUIRE(std::as_const(range).at(1) == points.at(1));
    }

    SECTION("get_points()") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(range.get_points()), point_set_view>);
        REQUIRE(defaulted.get_points().empty());

        auto ps = range.get_points();
        REQUIRE(ps.size() == 2);
        REQUIRE(ps[0] == to_point(points.at(0)));
        REQUIRE(ps[1] == to_point(points.at(1)));

        // Aliases the grid: writing through the view moves the grid point
        ps[1] = experimental::Point(4.0, 5.0, 6.0);
        REQUIRE(range.at(1) == GridPoint(0.1, 4.0, 5.0, 6.0));
    }

    SECTION("get_points() const") {
        const auto& crange = std::as_const(range);
        STATIC_REQUIRE(
          std::is_same_v<decltype(crange.get_points()), const_point_set_view>);
        REQUIRE(std::as_const(defaulted).get_points().empty());

        auto ps = crange.get_points();
        REQUIRE(ps.size() == 2);
        REQUIRE(ps[0] == to_point(points.at(0)));
        REQUIRE(ps[1] == to_point(points.at(1)));
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(range.size() == 2);
    }

    SECTION("get_radial_size()") {
        REQUIRE_FALSE(defaulted.get_radial_size().has_value());
        REQUIRE_FALSE(range.get_radial_size().has_value());
    }

    SECTION("set_radial_size()") {
        range.set_radial_size(10);
        REQUIRE(range.get_radial_size() == 10);
        REQUIRE_FALSE(range.get_angular_size().has_value());
    }

    SECTION("get_angular_size()") {
        REQUIRE_FALSE(defaulted.get_angular_size().has_value());
        REQUIRE_FALSE(range.get_angular_size().has_value());
    }

    SECTION("set_angular_size()") {
        range.set_angular_size(14);
        REQUIRE(range.get_angular_size() == 14);
        REQUIRE_FALSE(range.get_radial_size().has_value());
    }

    SECTION("operator==") {
        // Default vs. default
        REQUIRE(defaulted == Grid{});

        // Same points, no sizes
        Grid other(points.begin(), points.end());
        REQUIRE(range == other);

        // Different points
        std::vector<GridPoint> points2{GridPoint(),
                                       GridPoint(0.1, 1.2, 2.3, 42.0)};
        REQUIRE_FALSE(range == Grid(points2.begin(), points2.end()));

        // Same points, only one has a radial size
        other.set_radial_size(10);
        REQUIRE_FALSE(range == other);

        // Same points, different radial sizes
        range.set_radial_size(20);
        REQUIRE_FALSE(range == other);

        // Same points and radial sizes, only one has an angular size
        range.set_radial_size(10);
        REQUIRE(range == other);
        other.set_angular_size(14);
        REQUIRE_FALSE(range == other);

        // Same points, different angular sizes
        range.set_angular_size(26);
        REQUIRE_FALSE(range == other);

        // Same points and sizes
        range.set_angular_size(14);
        REQUIRE(range == other);
    }

    SECTION("operator!=") {
        Grid other(points.begin(), points.end());
        REQUIRE_FALSE(range != other);
        other.set_radial_size(10);
        REQUIRE(range != other);
    }
}
