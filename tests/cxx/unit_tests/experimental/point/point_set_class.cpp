/*
 * Copyright 2026 NWChemEx-Project
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

// Only what PointSet itself implements is tested here: its ctors, assignment,
// push_back, swap, and load. Everything PointSet inherits from
// PointSetCommon --- get_buffer and friends, size/empty, indexing, iteration,
// the comparison operators, and save --- is tested once in
// point_set_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/point/point_set_class.hpp>
#include <sstream>
#include <utility>

using namespace chemist::experimental;

TEST_CASE("experimental::PointSet") {
    Point p0(0.0, 1.0, 2.0);
    Point p1(3.0, 4.0, 5.0);
    Point p2(6.0, 7.0, 8.0);

    PointSet defaulted;
    PointSet ps{p0, p1, p2};

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.empty());
        }

        SECTION("Initializer list") {
            REQUIRE(ps.size() == 3);
            REQUIRE_FALSE(ps.empty());
            REQUIRE(ps[0] == p0);
            REQUIRE(ps[1] == p1);
            REQUIRE(ps[2] == p2);
        }

        SECTION("Iterator range") {
            std::vector<Point> points{p0, p1, p2};
            PointSet from_range(points.begin(), points.end());
            REQUIRE(from_range == ps);

            // An empty range gives an empty set
            PointSet from_empty(points.begin(), points.begin());
            REQUIRE(from_empty.size() == 0);
        }

        SECTION("From coordinate arrays") {
            using buffer_type = PointSet::buffer_type;
            buffer_type x(std::vector<double>{0.0, 3.0, 6.0});
            buffer_type y(std::vector<double>{1.0, 4.0, 7.0});
            buffer_type z(std::vector<double>{2.0, 5.0, 8.0});
            PointSet from_buffers(std::move(x), std::move(y), std::move(z));
            REQUIRE(from_buffers == ps);
        }

        SECTION("From coordinate arrays of different lengths") {
            using buffer_type = PointSet::buffer_type;
            buffer_type x(std::vector<double>{0.0, 3.0});
            buffer_type y(std::vector<double>{1.0});
            buffer_type z(std::vector<double>{2.0});
            REQUIRE_THROWS_AS(
              PointSet(std::move(x), std::move(y), std::move(z)),
              std::invalid_argument);
        }

        test_chemist::test_copy_and_move(defaulted, ps);
    }

    SECTION("Copies are deep") {
        PointSet copy(ps);
        copy[0].set_x(42.0);
        REQUIRE(ps[0].get_x() == 0.0);
    }

    SECTION("swap") {
        PointSet lhs(ps);
        PointSet rhs;
        lhs.swap(rhs);
        REQUIRE(lhs.empty());
        REQUIRE(rhs == ps);
    }

    SECTION("push_back") {
        defaulted.push_back(p0);
        REQUIRE(defaulted.size() == 1);
        REQUIRE(defaulted[0] == p0);

        defaulted.push_back(p1);
        defaulted.push_back(p2);
        REQUIRE(defaulted == ps);

        // Views can be pushed back too
        PointSet from_views;
        from_views.push_back(ps[0]);
        from_views.push_back(ps[1]);
        from_views.push_back(ps[2]);
        REQUIRE(from_views == ps);

        // ... and it is a copy, not an alias
        from_views[0].set_x(42.0);
        REQUIRE(ps[0].get_x() == 0.0);
    }

    SECTION("push_back rejects a mismatched floating-point type") {
        REQUIRE_THROWS_AS(ps.push_back(Point(1.0f, 2.0f, 3.0f)),
                          std::runtime_error);
    }

    SECTION("load") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(ps);
        }
        PointSet deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == ps);
    }

    SECTION("load an empty set") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(defaulted);
        }
        PointSet deserialized{p0};
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized.empty());
    }
}
