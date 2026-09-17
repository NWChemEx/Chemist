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

// Only what PointSetView itself implements is tested here: its ctors
// (including aliasing external buffers and the implicit conversions),
// assignment (which, unlike a normal copy, writes through the alias), swap,
// as_point_set, and the aliasing behavior of its own buffer_. Everything
// PointSetView inherits from PointSetCommon --- get_buffer and friends,
// size/empty, indexing, iteration, the comparison operators, and save --- is
// tested once, generically, in point_set_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/point/point_set_view.hpp>
#include <utility>

using namespace chemist::experimental;

namespace {

/** @brief Takes a read-only view of a set by value.
 *
 *  Used to prove that the PointSet -> PointSetView and mutable -> read-only
 *  conversions are implicit; if either were explicit, calling this with a
 *  PointSet or a mutable view would not compile.
 */
std::size_t count(const_point_set_view ps) { return ps.size(); }

} // namespace

TEST_CASE("experimental::PointSetView") {
    Point p0(0.0, 1.0, 2.0);
    Point p1(3.0, 4.0, 5.0);
    Point p2(6.0, 7.0, 8.0);

    PointSet ps{p0, p1, p2};
    PointSet empty;

    point_set_view psv(ps);
    const_point_set_view cpsv(ps);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            point_set_view defaulted;
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.empty());
        }

        SECTION("From a PointSet") {
            REQUIRE(psv.size() == 3);
            REQUIRE(psv[0] == p0);
            REQUIRE(psv[1] == p1);
            REQUIRE(psv[2] == p2);

            // *this aliases ps; it did not copy it
            ps[0].set_x(42.0);
            REQUIRE(psv[0].get_x() == 42.0);
        }

        SECTION("From a PointSet, implicitly") {
            // Would not compile if the ctor were explicit
            REQUIRE(count(ps) == 3);
        }

        SECTION("From an empty PointSet") {
            point_set_view of_empty(empty);
            REQUIRE(of_empty.empty());
        }

        SECTION("From three coordinate arrays") {
            using buffer_type = PointSet::buffer_type;
            buffer_type x(std::vector<double>{0.0, 3.0, 6.0});
            buffer_type y(std::vector<double>{1.0, 4.0, 7.0});
            buffer_type z(std::vector<double>{2.0, 5.0, 8.0});

            // *this can alias storage no PointSet owns
            point_set_view of_buffers(x.as_view(), y.as_view(), z.as_view());
            REQUIRE(of_buffers.as_point_set() == ps);

            of_buffers[1].set_x(42.0);
            REQUIRE(x.at(1) == 42.0);
        }

        SECTION("From coordinate arrays of different lengths") {
            using buffer_type = PointSet::buffer_type;
            buffer_type x(std::vector<double>{0.0, 3.0});
            buffer_type y(std::vector<double>{1.0});
            buffer_type z(std::vector<double>{2.0});
            REQUIRE_THROWS_AS(
              point_set_view(x.as_view(), y.as_view(), z.as_view()),
              std::invalid_argument);
        }

        SECTION("Mutable to read-only, implicitly") {
            // Would not compile if the conversion were explicit
            REQUIRE(count(psv) == 3);

            const_point_set_view as_const(psv);
            REQUIRE(as_const.as_point_set() == ps);
        }

        SECTION("Copy ctor is shallow") {
            point_set_view copy(psv);
            copy[0].set_x(42.0);
            REQUIRE(ps[0].get_x() == 42.0);
        }

        SECTION("Move ctor") {
            point_set_view moved(std::move(psv));
            moved[0].set_x(42.0);
            REQUIRE(ps[0].get_x() == 42.0);
        }

        SECTION("Copy assignment writes through") {
            PointSet other{p2, p1, p0};
            point_set_view other_view(other);

            psv = other_view;

            // ps, not psv's binding, is what changed
            REQUIRE(ps == PointSet{p2, p1, p0});
            // ... and other was left alone
            REQUIRE(other == PointSet{p2, p1, p0});
        }

        SECTION("Assigning a differently-sized set throws") {
            PointSet other{p0};
            point_set_view other_view(other);
            REQUIRE_THROWS_AS(psv = other_view, std::runtime_error);
        }
    }

    SECTION("swap rebinds") {
        PointSet other{p2, p1, p0};
        point_set_view other_view(other);

        psv.swap(other_view);

        // The views traded places; neither set changed
        REQUIRE(psv.as_point_set() == other);
        REQUIRE(other_view.as_point_set() == ps);
        REQUIRE(ps == PointSet{p0, p1, p2});
        REQUIRE(other == PointSet{p2, p1, p0});
    }

    SECTION("as_point_set is a deep copy") {
        auto copy = psv.as_point_set();
        REQUIRE(copy == ps);

        copy[0].set_x(42.0);
        REQUIRE(ps[0].get_x() == 0.0);
    }

    SECTION("get_buffer aliases the set's arrays") {
        // get_x/y/z_buffer are PointSetCommon's; what is under test here is
        // that PointSetView's own buffer_ hands back a view into ps's arrays
        // rather than into some internal copy.
        REQUIRE(psv.get_x_buffer().value<double>()[1] == 3.0);
        REQUIRE(psv.get_y_buffer().value<double>()[1] == 4.0);
        REQUIRE(psv.get_z_buffer().value<double>()[1] == 5.0);

        // Writing through the escape hatch writes into the aliased set
        psv.get_x_buffer().value<double>()[1] = 42.0;
        REQUIRE(ps[1].get_x() == 42.0);
    }

    SECTION("Writing through operator[] mutates the aliased set") {
        // operator[] is PointSetCommon's; what is under test here is that
        // PointSetView's own buffer_ hands back views into ps's arrays,
        // rather than into some internal copy.
        psv[1].set_y(42.0);
        REQUIRE(ps[1].get_y() == 42.0);
    }
}
