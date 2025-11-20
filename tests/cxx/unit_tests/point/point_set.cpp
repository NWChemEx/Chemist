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
#include <cereal/archives/binary.hpp>
#include <chemist/point/point_set.hpp>
#include <sstream>

using namespace chemist;

/* Testing Notes:
 *
 * We assume the base class has been tested and works correctly. Subject to
 * that assumption, we need to test the ability to access points by reference
 * (since we wrote the implementation), and obtaining the size, but do not need
 * to retest all the ways one can access the points (i.e., operator[], at, and
 * iterators).
 *
 * The PointSet class doesn't guarantee contiguous storage. Such a guarantee is
 * left up to the backend.
 */
TEMPLATE_TEST_CASE("PointSet", "", float, double) {
    using set_type   = PointSet<TestType>;
    using value_type = typename set_type::value_type;

    value_type p0(0.0, 1.0, 2.0);
    value_type p1(3.0, 4.0, 5.0);

    set_type defaulted;
    set_type points{p0, p1, p1}; // <- ensure we can add same point 2x

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }

        SECTION("initializer_list") {
            REQUIRE(points.size() == 3);
            REQUIRE(points[0] == p0);
            REQUIRE(points[1] == p1);
            REQUIRE(points[2] == p1);
        }

        SECTION("Copy") {
            set_type copy0(defaulted);
            set_type copy1(points);
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == points);
        }

        SECTION("Move") {
            set_type copy0(defaulted);
            set_type move0(std::move(defaulted));
            set_type copy1(points);

            // Do the move
            set_type move1(std::move(points));

            // Check the values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);
        }

        SECTION("Copy assignment") {
            set_type copy0;
            auto pcopy0 = &(copy0 = defaulted);
            set_type copy1;
            auto pcopy1 = &(copy1 = points);

            // Check values
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == points);

            // Check returns *this for chaining
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("Move assignment") {
            set_type copy0(defaulted);
            set_type move0;
            auto pmove0 = &(move0 = std::move(defaulted));
            set_type copy1(points);
            set_type move1;

            // Do the move
            auto pmove1 = &(move1 = std::move(points));

            // Check values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);

            // Check returns *this for chaining
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);
        }
    }

    SECTION("push_back") {
        defaulted.push_back(p0);
        defaulted.push_back(p1);
        defaulted.push_back(p1);
        REQUIRE(defaulted == points);
    }

    SECTION("x_data()") {
        REQUIRE(defaulted.x_data() == nullptr);
        REQUIRE(points.x_data() == &points[0].x());
    }

    SECTION("x_data() const") {
        REQUIRE(std::as_const(defaulted).x_data() == nullptr);
        const auto& cpoints = points;
        REQUIRE(cpoints.x_data() == &cpoints[0].x());
    }

    SECTION("y_data()") {
        REQUIRE(defaulted.y_data() == nullptr);
        REQUIRE(points.y_data() == &points[0].y());
    }

    SECTION("y_data() const") {
        REQUIRE(std::as_const(defaulted).y_data() == nullptr);
        const auto& cpoints = points;
        REQUIRE(cpoints.y_data() == &cpoints[0].y());
    }

    SECTION("z_data()") {
        REQUIRE(defaulted.z_data() == nullptr);
        REQUIRE(points.z_data() == &points[0].z());
    }

    SECTION("z_data() const") {
        REQUIRE(std::as_const(defaulted).z_data() == nullptr);
        const auto& cpoints = points;
        REQUIRE(cpoints.z_data() == &cpoints[0].z());
    }

    SECTION("at_()") {
        using rtype = decltype(points[0]);
        STATIC_REQUIRE(std::is_same_v<rtype, typename set_type::reference>);

        for(std::size_t point_i = 0; point_i < 3; ++point_i) {
            const auto corr = point_i == 0 ? p0 : p1;
            for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                REQUIRE(points[point_i].coord(coord_i) == corr.coord(coord_i));
            }
        }
    }

    SECTION("at_() const") {
        using rtype = decltype(std::as_const(points)[0]);
        using corr  = typename set_type::const_reference;
        STATIC_REQUIRE(std::is_same_v<rtype, corr>);

        for(std::size_t point_i = 0; point_i < 3; ++point_i) {
            const auto corr = point_i == 0 ? p0 : p1;
            for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                REQUIRE(std::as_const(points)[point_i].coord(coord_i) ==
                        corr.coord(coord_i));
            }
        }
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(points.size() == 3);
    }

    SECTION("serialization") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive oarchive(ss);
            oarchive(points);
        }
        {
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(defaulted);
        }
        REQUIRE(defaulted == points);
    }
}
