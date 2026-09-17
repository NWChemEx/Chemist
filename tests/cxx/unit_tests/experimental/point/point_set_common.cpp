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

// PointSetCommon implements the API shared by PointSet and PointSetView (the
// container API inherited from IndexableContainerBase included, since both
// derived classes get it through PointSetCommon in the same way). It has no
// state of its own to construct, so these tests drive it through PointSet ---
// the simplest concrete derived class --- and pull in PointSetView only where
// a property genuinely spans two different derived types (the
// const-qualification which gates whether indexing hands out a mutable or
// read-only PointView, and reachability of save through a view). PointSet's
// own ctors, assignment, push_back, swap, and load; and PointSetView's own
// ctors, assignment, aliasing, and conversions, are each tested in their own
// namesake file.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/point/point_set_view.hpp>
#include <sstream>
#include <utility>

using namespace chemist::experimental;

TEST_CASE("experimental::PointSetCommon") {
    Point p0(0.0, 1.0, 2.0);
    Point p1(3.0, 4.0, 5.0);
    Point p2(6.0, 7.0, 8.0);

    PointSet defaulted;
    PointSet ps{p0, p1, p2};

    SECTION("size/empty") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(defaulted.empty());
        REQUIRE(ps.size() == 3);
        REQUIRE_FALSE(ps.empty());
    }

    SECTION("operator[] and at return views gated by constness") {
        ps[1].set_y(42.0);
        REQUIRE(ps[1].get_y() == 42.0);

        REQUIRE(ps.at(0) == p0);
        REQUIRE(std::as_const(ps).at(2) == p2);
        REQUIRE_THROWS_AS(ps.at(3), std::out_of_range);

        // A read-only set hands out read-only views; whether *this itself is
        // read-only is what gates this, not which derived class it is.
        STATIC_REQUIRE(test_chemist::has_setters_v<decltype(ps[0])>);
        STATIC_REQUIRE_FALSE(
          test_chemist::has_setters_v<decltype(std::as_const(ps)[0])>);

        // A PointSetView<const PointSet> bakes the read-only-ness into its
        // own type, rather than depending on the constness of the variable
        // holding it, so the same gating is confirmed through a view too.
        point_set_view psv(ps);
        const_point_set_view cpsv(ps);
        STATIC_REQUIRE(test_chemist::has_setters_v<decltype(psv[0])>);
        STATIC_REQUIRE_FALSE(test_chemist::has_setters_v<decltype(cpsv[0])>);
    }

    SECTION("Iteration") {
        std::vector<Point> seen;
        for(const auto& point : ps) seen.push_back(point.as_point());
        REQUIRE(seen == std::vector<Point>{p0, p1, p2});
    }

    SECTION("Writing through a view is visible in the coordinate array") {
        // This is the load-bearing property of the whole design: the view
        // handed out by operator[] aliases the set's storage, and the
        // storage is a structure of arrays. If the view copied, or if the
        // set interleaved, this would read the wrong value.
        ps[1].set_x(42.0);
        REQUIRE(ps.get_x_buffer().value<double>()[1] == 42.0);

        // ... and nothing else moved
        REQUIRE(ps.get_x_buffer().value<double>()[0] == 0.0);
        REQUIRE(ps.get_x_buffer().value<double>()[2] == 6.0);
        REQUIRE(ps[1].get_y() == 4.0);
    }

    SECTION("get_buffer") {
        // The set is a structure of arrays: all three x-coordinates live in
        // one contiguous array, NOT interleaved with y and z.
        auto x = ps.get_x_buffer();
        REQUIRE(x.size() == 3);
        REQUIRE(x.is_contiguous());
        auto span = x.value<double>();
        REQUIRE(span[0] == 0.0);
        REQUIRE(span[1] == 3.0);
        REQUIRE(span[2] == 6.0);

        REQUIRE(ps.get_y_buffer().value<double>()[1] == 4.0);
        REQUIRE(ps.get_z_buffer().value<double>()[1] == 5.0);

        REQUIRE(ps.get_buffer(0) == ps.get_x_buffer());
        REQUIRE(ps.get_buffer(1) == ps.get_y_buffer());
        REQUIRE(ps.get_buffer(2) == ps.get_z_buffer());

        // This out-of-range check exercises assert_coord_in_range_, which is
        // shared by every derived class; PointSet is enough to cover it, so
        // it is not repeated in point_set_class.cpp or point_set_view.cpp.
        REQUIRE_THROWS_AS(ps.get_buffer(3), std::out_of_range);
        REQUIRE_THROWS_AS(std::as_const(ps).get_buffer(3), std::out_of_range);
    }

    SECTION("get_buffer is an escape hatch, not a copy") {
        auto span = ps.get_x_buffer().value<double>();
        span[2]   = 42.0;
        REQUIRE(ps[2].get_x() == 42.0);
    }

    SECTION("operator==/operator!=") {
        REQUIRE(ps == PointSet{p0, p1, p2});
        REQUIRE(defaulted == PointSet{});

        REQUIRE(ps != defaulted);
        REQUIRE(ps != PointSet{p0, p1});
        REQUIRE(ps != PointSet{p0, p1, p0});
    }

    SECTION("save") {
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

    SECTION("save works the same way through a view") {
        // save() is inherited verbatim; the only thing worth confirming here
        // is that it is actually reachable through a PointSetView, not that
        // its logic changes (it does not). Loading it back uses PointSet's
        // own load purely as an instrument.
        const_point_set_view cpsv(ps);
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(cpsv);
        }
        PointSet deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == ps);
    }
}
