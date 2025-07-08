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

#include "../catch.hpp"
#include <chemist/point/point_set_view.hpp>
#include <vector>
#include <utility>

template<typename PointSetType>
void test_point_set_view_guts() {
    using point_set_type = PointSetType;
    using point_type     = typename point_set_type::value_type;
    using view_type      = chemist::PointSetView<point_set_type>;

    point_type p0{1.1, 2.1, 3.1};
    point_type p1{1.2, 2.2, 3.2};
    point_type p2{1.3, 2.3, 3.3};

    point_set_type defaulted_ps;
    point_set_type one_point_ps{p0};
    point_set_type two_points_ps{p0, p1};
    point_set_type three_points_ps{p0, p1, p2};

    view_type defaulted;
    view_type no_points(defaulted_ps);
    view_type one_point(one_point_ps);
    view_type two_points(two_points_ps);
    view_type three_points(three_points_ps);

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
        SECTION("pointers") {
            view_type empty(0, nullptr, nullptr, nullptr);
            REQUIRE(empty.size() == 0);

            view_type v1(1, one_point_ps.x_data(), one_point_ps.y_data(),
                         one_point_ps.z_data());
            REQUIRE(v1.size() == 1);
            REQUIRE(v1[0] == one_point_ps[0]);
        }
        SECTION("mutable to read-only") {
            if constexpr(!std::is_const_v<point_set_type>) {
                using const_type = chemist::PointSetView<const point_set_type>;

                const_type const_defaulted(defaulted);
                REQUIRE(const_defaulted.size() == 0);

                const_type const_no_points(no_points);
                REQUIRE(const_no_points.size() == 0);

                const_type const_one_point(one_point);
                REQUIRE(const_one_point.size() == 1);
                REQUIRE(const_one_point[0] == one_point[0]);

                const_type const_two_points(two_points);
                REQUIRE(const_two_points.size() == 2);
                REQUIRE(const_two_points[0] == two_points[0]);
                REQUIRE(const_two_points[1] == two_points[1]);
            }
        }

        SECTION("copy") {
            // N.b. operator== is checked by setting values directly and the
            //      the "value" section has confirmed it works
            view_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            view_type no_points_copy(no_points);
            REQUIRE(no_points_copy == no_points);

            view_type one_point_copy(one_point);
            REQUIRE(one_point_copy == one_point);

            view_type two_point_copy(two_points);
            REQUIRE(two_point_copy == two_points);

            view_type three_point_copy(three_points);
            REQUIRE(three_point_copy == three_points);
        }
        SECTION("copy assignment") {
            view_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(defaulted_copy == defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);

            view_type no_points_copy;
            auto pno_points_copy = &(no_points_copy = no_points);
            REQUIRE(no_points_copy == no_points);
            REQUIRE(pno_points_copy == &no_points_copy);

            view_type one_point_copy;
            auto pone_point_copy = &(one_point_copy = one_point);
            REQUIRE(one_point_copy == one_point);
            REQUIRE(pone_point_copy == &one_point_copy);

            view_type two_point_copy;
            auto ptwo_point_copy = &(two_point_copy = two_points);
            REQUIRE(two_point_copy == two_points);
            REQUIRE(ptwo_point_copy == &two_point_copy);

            view_type three_point_copy;
            auto pthree_point_copy = &(three_point_copy = three_points);
            REQUIRE(three_point_copy == three_points);
            REQUIRE(pthree_point_copy == &three_point_copy);
        }
        SECTION("move") {
            view_type defaulted_copy(defaulted);
            view_type defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);

            view_type no_points_copy(no_points);
            view_type no_points_move(std::move(no_points));
            REQUIRE(no_points_copy == no_points_move);

            view_type one_point_copy(one_point);
            view_type one_point_move(std::move(one_point));
            REQUIRE(one_point_copy == one_point_move);

            view_type two_point_copy(two_points);
            view_type two_point_move(std::move(two_points));
            REQUIRE(two_point_copy == two_point_move);

            view_type three_point_copy(three_points);
            view_type three_point_move(std::move(three_points));
            REQUIRE(three_point_copy == three_point_move);
        }
        SECTION("move assignment") {
            view_type defaulted_copy(defaulted);
            view_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);
            REQUIRE(pdefaulted_move == &defaulted_move);

            view_type no_points_copy(no_points);
            view_type no_points_move;
            auto pno_points_move = &(no_points_move = std::move(no_points));
            REQUIRE(no_points_copy == no_points_move);
            REQUIRE(pno_points_move == &no_points_move);

            view_type one_point_copy(one_point);
            view_type one_point_move;
            auto pone_point_move = &(one_point_move = std::move(one_point));
            REQUIRE(one_point_copy == one_point_move);
            REQUIRE(pone_point_move == &one_point_move);

            view_type two_point_copy(two_points);
            view_type two_point_move;
            auto ptwo_point_move = &(two_point_move = std::move(two_points));
            REQUIRE(two_point_copy == two_point_move);
            REQUIRE(ptwo_point_move == &two_point_move);

            view_type three_point_copy(three_points);
            view_type three_point_move;
            auto pthree_point_move =
              &(three_point_move = std::move(three_points));
            REQUIRE(three_point_copy == three_point_move);
            REQUIRE(pthree_point_move == &three_point_move);
        }
    }

    SECTION("swap") {
        view_type one_point_copy(one_point);
        view_type two_point_copy(two_points);

        one_point.swap(two_points);
        REQUIRE(one_point == two_point_copy);
        REQUIRE(two_points == one_point_copy);
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
        REQUIRE(std::as_const(one_point)[0] == p0);

        REQUIRE(std::as_const(two_points)[0] == p0);
        REQUIRE(std::as_const(two_points)[1] == p1);

        REQUIRE(std::as_const(three_points)[0] == p0);
        REQUIRE(std::as_const(three_points)[1] == p1);
        REQUIRE(std::as_const(three_points)[2] == p2);
    }

    SECTION("operator==") {
        // N.b. for logical consistency all checks in this section must prepare
        //      the value to compare to via the value ctor
        SECTION("Default vs default") {
            view_type other_defaulted;
            REQUIRE(other_defaulted == defaulted);
        }

        SECTION("Default vs empty") { REQUIRE(defaulted == no_points); }

        SECTION("Default vs. non-empty") {
            REQUIRE_FALSE(defaulted == one_point);
        }

        SECTION("Same literal PointSet") {
            view_type other_three(three_points_ps);
            REQUIRE(three_points == other_three);
        }

        SECTION("Different PointSets, but same points") {
            point_set_type other_three_ps{p0, p1, p2};
            view_type other_three(other_three_ps);
            REQUIRE(three_points == other_three);
        }

        SECTION("Different points") {
            // N.b. we swap points 0 and 1
            point_set_type other_two_ps{p1, p0};
            view_type other_two(other_two_ps);
            REQUIRE_FALSE(two_points == other_two);
        }
    }

    SECTION("operator!=") {
        // n.b. we know that operator== works, so just spot check that this
        // operator negates operator== for one true and one false case

        REQUIRE(defaulted != one_point);
        REQUIRE_FALSE(defaulted != no_points);
    }

    SECTION("as_points") {
        REQUIRE(defaulted.as_point_set() == defaulted_ps);
        REQUIRE(one_point.as_point_set() == one_point_ps);
        REQUIRE(two_points.as_point_set() == two_points_ps);
        REQUIRE(three_points.as_point_set() == three_points_ps);
    }
}

TEMPLATE_TEST_CASE("PointSetView<T>", "", float, double) {
    using point_type     = TestType;
    using point_set_type = chemist::PointSet<point_type>;
    test_point_set_view_guts<point_set_type>();

    SECTION("Can mutate elements") {
        using view_type = chemist::PointSetView<point_set_type>;

        chemist::Point<point_type> p0{1.1, 2.1, 3.1};
        point_set_type one_point_ps{p0};
        view_type one_point(one_point_ps);

        REQUIRE(one_point[0].x() == point_type{1.1});
        REQUIRE(one_point_ps[0].x() == point_type{1.1});
        one_point[0].x() = point_type{42.0};
        REQUIRE(one_point[0].x() == point_type{42.0});
        REQUIRE(one_point_ps[0].x() == point_type{42.0});
    }
}

TEMPLATE_TEST_CASE("PointSetView<const T>", "", float, double) {
    using point_type     = TestType;
    using point_set_type = chemist::PointSet<point_type>;
    test_point_set_view_guts<const point_set_type>();
}
