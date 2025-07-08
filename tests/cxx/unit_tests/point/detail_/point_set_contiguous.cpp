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

#include "../../catch.hpp"
#include <chemist/point/detail_/point_set_contiguous.hpp>
#include <utility>
#include <vector>

template<typename PointSetType>
void test_point_set_contiguous_guts() {
    using point_set_type = PointSetType;
    using coord_type     = typename point_set_type::value_type::coord_type;
    using pimpl_type     = chemist::detail_::PointSetContiguous<point_set_type>;
    using reference      = typename pimpl_type::reference;
    using const_reference = typename pimpl_type::const_reference;

    std::vector<coord_type> x{1.1, 1.2, 1.3};
    std::vector<coord_type> y{2.1, 2.2, 2.3};
    std::vector<coord_type> z{3.1, 3.2, 3.3};

    pimpl_type defaulted;
    pimpl_type no_points(0, x.data(), y.data(), z.data());
    pimpl_type one_point(1, x.data(), y.data(), z.data());
    pimpl_type two_points(2, x.data(), y.data(), z.data());
    pimpl_type three_points(3, x.data(), y.data(), z.data());

    reference p0(x.data()[0], y.data()[0], z.data()[0]);
    reference p1(x.data()[1], y.data()[1], z.data()[1]);
    reference p2(x.data()[2], y.data()[2], z.data()[2]);

    const_reference cp0(x.data()[0], y.data()[0], z.data()[0]);
    const_reference cp1(x.data()[1], y.data()[1], z.data()[1]);
    const_reference cp2(x.data()[2], y.data()[2], z.data()[2]);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("value") {
            REQUIRE(no_points.size() == 0);

            REQUIRE(one_point.size() == 1);
            REQUIRE(one_point[0] == p0);

            REQUIRE(two_points.size() == 2);
            REQUIRE(two_points[0] == p0);
            REQUIRE(two_points[1] == p1);

            REQUIRE(three_points.size() == 3);
            REQUIRE(three_points[0] == p0);
            REQUIRE(three_points[1] == p1);
            REQUIRE(three_points[2] == p2);
        }
        SECTION("copy") {
            // N.b. operator== is checked by setting values directly and the
            //      the "value" section has confirmed it works
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            pimpl_type no_points_copy(no_points);
            REQUIRE(no_points_copy == no_points);

            pimpl_type one_point_copy(one_point);
            REQUIRE(one_point_copy == one_point);

            pimpl_type two_point_copy(two_points);
            REQUIRE(two_point_copy == two_points);

            pimpl_type three_point_copy(three_points);
            REQUIRE(three_point_copy == three_points);
        }
    }

    SECTION("clone") {
        auto defaulted_copy = defaulted.clone();
        REQUIRE(defaulted_copy->are_equal(defaulted));

        auto no_points_copy = no_points.clone();
        REQUIRE(no_points_copy->are_equal(no_points));

        auto one_point_copy = one_point.clone();
        REQUIRE(one_point_copy->are_equal(one_point));

        auto two_points_copy = two_points.clone();
        REQUIRE(two_points_copy->are_equal(two_points));

        auto three_points_copy = three_points.clone();
        REQUIRE(three_points_copy->are_equal(three_points));
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(no_points.size() == 0);
        REQUIRE(one_point.size() == 1);
        REQUIRE(two_points.size() == 2);
        REQUIRE(three_points.size() == 3);
    }

    SECTION("operator[]") {
        REQUIRE(one_point[0] == p0);

        REQUIRE(two_points[0] == p0);
        REQUIRE(two_points[1] == p1);

        REQUIRE(three_points[0] == p0);
        REQUIRE(three_points[1] == p1);
        REQUIRE(three_points[2] == p2);
    }

    SECTION("operator[] const") {
        REQUIRE(std::as_const(one_point)[0] == cp0);

        REQUIRE(std::as_const(two_points)[0] == cp0);
        REQUIRE(std::as_const(two_points)[1] == cp1);

        REQUIRE(std::as_const(three_points)[0] == p0);
        REQUIRE(std::as_const(three_points)[1] == p1);
        REQUIRE(std::as_const(three_points)[2] == p2);
    }

    SECTION("operator==") {
        // N.b. for logical consistency all checks in this section must prepare
        //      the value to compare to via the value ctor
        SECTION("Default vs default") {
            pimpl_type other_defaulted;
            REQUIRE(other_defaulted == defaulted);
        }

        SECTION("Default vs empty") { REQUIRE(defaulted == no_points); }

        SECTION("Default vs. non-empty") {
            REQUIRE_FALSE(defaulted == one_point);
        }

        SECTION("Same points, same addresses") {
            pimpl_type other_three(3, x.data(), y.data(), z.data());
            REQUIRE(three_points == other_three);
        }

        SECTION("Same points, different addresses") {
            // For good measure we also shorten the unpacked coordinates
            std::vector<coord_type> x_copy{1.1, 1.2};
            std::vector<coord_type> y_copy{2.1, 2.2};
            std::vector<coord_type> z_copy{3.1, 3.2};

            pimpl_type other_two(2, x_copy.data(), y_copy.data(),
                                 z_copy.data());
            REQUIRE(two_points == other_two);
        }

        SECTION("Different points") {
            // N.b. we swap x and y to get different coordinates
            pimpl_type other_two(2, y.data(), x.data(), z.data());
            REQUIRE_FALSE(two_points == other_two);
        }
    }
}

TEMPLATE_TEST_CASE("PointSetContiguous<T>", "", float, double) {
    using point_type     = TestType;
    using point_set_type = chemist::PointSet<point_type>;
    test_point_set_contiguous_guts<point_set_type>();
}

TEMPLATE_TEST_CASE("PointSetContiguous<const T>", "", float, double) {
    using point_type     = TestType;
    using point_set_type = chemist::PointSet<point_type>;
    test_point_set_contiguous_guts<const point_set_type>();
}
