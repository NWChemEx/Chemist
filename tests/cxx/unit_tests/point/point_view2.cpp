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
#include <chemist/point/point_view2.hpp>

using namespace chemist;

TEMPLATE_TEST_CASE("PointView2", "", Point<double>, Point<float>) {
    using view_type       = PointView2<TestType>;
    using const_view_type = PointView2<const TestType>;

    TestType r0(0.0, 1.0, 2.0);
    TestType r1(3.0, 4.0, 5.0);

    view_type pr0(r0.x(), r0.y(), r0.z());
    const_view_type pr1(r1.x(), r1.y(), r1.z());

    SECTION("CTors and assignment") {
        SECTION("Point reference ctor") {
            view_type pr2(r0);

            for(std::size_t i = 0; i < 3; ++i)
                REQUIRE(&pr2.coord(i) == &r0.coord(i));
        }

        SECTION("Value ctor") {
            // Check address to verify aliasing
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&pr0.coord(i) == &r0.coord(i));
                REQUIRE(&pr1.coord(i) == &r1.coord(i));
            }
        }

        SECTION("Assign from point") {
            pr0 = r1;
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&pr0.coord(i) == &r0.coord(i)); // Still aliases r0?
                REQUIRE(r0.coord(i) == r1.coord(i));    // Has r1's values?
            }
        }

        SECTION("Copy ctor") {
            // The copy ctor should still alias the original point
            view_type copy0(pr0);
            const_view_type copy1(pr1);

            // Check address to verify aliasing
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&copy0.coord(i) == &r0.coord(i));
                REQUIRE(&copy1.coord(i) == &r1.coord(i));
            }
        }

        SECTION("Move ctor") {
            // The move ctor should still alias the original point
            view_type moved0(std::move(pr0));
            const_view_type moved1(std::move(pr1));

            // Check address to verify aliasing
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&moved0.coord(i) == &r0.coord(i));
                REQUIRE(&moved1.coord(i) == &r1.coord(i));
            }
        }

        SECTION("Copy assignment") {
            // The copy assignment should still alias the original point
            view_type copy0(r1.x(), r1.y(), r1.z());
            auto ppr0 = &(copy0 = pr0);
            const_view_type copy1(r0.x(), r0.y(), r0.z());
            auto ppr1 = &(copy1 = pr1);

            // Return *this for chaining
            REQUIRE(ppr0 == &copy0);
            REQUIRE(ppr1 == &copy1);

            // Check address to verify aliasing
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&copy0.coord(i) == &r0.coord(i));
                REQUIRE(&copy1.coord(i) == &r1.coord(i));
            }
        }

        SECTION("Move assignment") {
            // The move assignment should still alias the original point
            view_type moved0(r1.x(), r1.y(), r1.z());
            auto ppr0 = &(moved0 = std::move(pr0));
            const_view_type moved1(r0.x(), r0.y(), r0.z());
            auto ppr1 = &(moved1 = std::move(pr1));

            // Return *this for chaining
            REQUIRE(ppr0 == &moved0);
            REQUIRE(ppr1 == &moved1);

            // Check address to verify aliasing
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&moved0.coord(i) == &r0.coord(i));
                REQUIRE(&moved1.coord(i) == &r1.coord(i));
            }
        }
    }

    SECTION("coord()") {
        // Check address to verify aliasing
        for(std::size_t i = 0; i < 3; ++i) {
            REQUIRE(&pr0.coord(i) == &r0.coord(i));
            REQUIRE(&pr1.coord(i) == &r1.coord(i));
        }
    }

    SECTION("coord() const") {
        // Check address to verify aliasing
        for(std::size_t i = 0; i < 3; ++i) {
            REQUIRE(&std::as_const(pr0).coord(i) == &r0.coord(i));
            REQUIRE(&std::as_const(pr1).coord(i) == &r1.coord(i));
        }
    }

    SECTION("x()") {
        REQUIRE(&pr0.x() == &r0.x());
        REQUIRE(&pr1.x() == &r1.x());
    }

    SECTION("y()") {
        REQUIRE(&pr0.y() == &r0.y());
        REQUIRE(&pr1.y() == &r1.y());
    }

    SECTION("z()") {
        REQUIRE(&pr0.z() == &r0.z());
        REQUIRE(&pr1.z() == &r1.z());
    }

    SECTION("x() const") {
        REQUIRE(&std::as_const(pr0).x() == &r0.x());
        REQUIRE(&std::as_const(pr1).x() == &r1.x());
    }

    SECTION("y() const") {
        REQUIRE(&std::as_const(pr0).y() == &r0.y());
        REQUIRE(&std::as_const(pr1).y() == &r1.y());
    }

    SECTION("z() const") {
        REQUIRE(&std::as_const(pr0).z() == &r0.z());
        REQUIRE(&std::as_const(pr1).z() == &r1.z());
    }

    SECTION("magnitude") {
        REQUIRE(pr0.magnitude() == r0.magnitude());
        REQUIRE(pr1.magnitude() == r1.magnitude());
    }

    SECTION("Comparisons") {
        // N.B. We test the symmetry of the operator to ensure the view works
        //      seamlessly with Point

        // Compare to same Point
        REQUIRE(pr0 == r0);
        REQUIRE(r0 == pr0);
        REQUIRE_FALSE(pr0 != r0);
        REQUIRE_FALSE(r0 != pr0);
        REQUIRE(pr1 == r1);
        REQUIRE(r1 == pr1);
        REQUIRE_FALSE(pr1 != r1);
        REQUIRE_FALSE(r1 != pr1);

        // Compare to different Point
        REQUIRE_FALSE(pr0 == r1);
        REQUIRE_FALSE(r1 == pr0);
        REQUIRE(pr0 != r1);
        REQUIRE(r1 != pr0);
        REQUIRE_FALSE(pr1 == r0);
        REQUIRE_FALSE(r0 == pr1);
        REQUIRE(pr1 != r0);
        REQUIRE(r0 != pr1);

        // Compare to same PointView
        view_type pr2(r0.x(), r0.y(), r0.z());
        const_view_type pr3(r1.x(), r1.y(), r1.z());
        REQUIRE(pr0 == pr2);
        REQUIRE(pr2 == pr0);
        REQUIRE_FALSE(pr0 != pr2);
        REQUIRE_FALSE(pr2 != pr0);
        REQUIRE(pr1 == pr3);
        REQUIRE(pr3 == pr1);
        REQUIRE_FALSE(pr1 != pr3);
        REQUIRE_FALSE(pr3 != pr1);

        // Compare to different PointView (including different const-ness)
        REQUIRE_FALSE(pr0 == pr3);
        REQUIRE_FALSE(pr3 == pr0);
        REQUIRE(pr0 != pr3);
        REQUIRE(pr3 != pr0);
        REQUIRE_FALSE(pr1 == pr2);
        REQUIRE_FALSE(pr2 == pr1);
        REQUIRE(pr1 != pr2);
        REQUIRE(pr2 != pr1);
    }

    SECTION("as_point") {
        REQUIRE(pr0.as_point() == r0);
        REQUIRE(pr1.as_point() == r1);
    }

    SECTION("swap") {
        view_type copy_pr0(pr0);
        view_type pr2(r1.x(), r1.y(), r1.z());
        view_type copy_pr2(pr2);

        pr0.swap(pr2);

        REQUIRE(pr0 == copy_pr2);
        REQUIRE(pr2 == copy_pr0);
    }
}
