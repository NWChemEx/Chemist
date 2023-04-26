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
        SECTION("Value ctor") {
            // Check address to verify aliasing
            for(std::size_t i = 0; i < 3; ++i) {
                REQUIRE(&pr0.coord(i) == &r0.coord(i));
                REQUIRE(&pr1.coord(i) == &r1.coord(i));
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

        // Compare to different PointView
    }

    SECTION("To Point") {}
}
