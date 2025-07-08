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
#include <chemist/point_charge/point_charge_view.hpp>
#include <utility>

using namespace chemist;

/* Testing Strategy:
 *
 * We assume that the base class has been tested and that it works (although
 * we do spot check the state of the base in the ctors).
 */

TEMPLATE_TEST_CASE("PointChargeView", "", float, double) {
    using value_type = PointCharge<TestType>;
    using view_type  = PointChargeView<value_type>;
    using const_view = PointChargeView<const value_type>;

    value_type q0(1.0, 2.0, 3.0, 4.0);
    value_type q1(-1.0, 3.0, 4.0, 5.0);

    // This uses ctor 1
    view_type pq0(q0.charge(), q0.x(), q0.y(), q0.z());

    // This uses ctor 2
    typename const_view::point_view_type pr1(q1.x(), q1.y(), q1.z());
    const_view pq1(q1.charge(), pr1);

    SECTION("CTors and Assignment") {
        SECTION("PointCharge") {
            view_type pq2(q0);
            REQUIRE(&pq2.charge() == &q0.charge());
            REQUIRE(&pq2.x() == &q0.x());
            REQUIRE(&pq2.y() == &q0.y());
            REQUIRE(&pq2.z() == &q0.z());
        }

        SECTION("charge, point") {
            REQUIRE(&pq0.charge() == &q0.charge());
            REQUIRE(&pq0.x() == &q0.x());
            REQUIRE(&pq0.y() == &q0.y());
            REQUIRE(&pq0.z() == &q0.z());
        }

        SECTION("charge, x, y, z") {
            REQUIRE(&pq1.charge() == &q1.charge());
            REQUIRE(&pq1.x() == &q1.x());
            REQUIRE(&pq1.y() == &q1.y());
            REQUIRE(&pq1.z() == &q1.z());
        }

        SECTION("Assign from PointCharge") {
            SECTION("From a reference") {
                pq0 = q1;

                // Still aliases q0
                REQUIRE(&pq0.charge() == &q0.charge());
                REQUIRE(&pq0.x() == &q0.x());
                REQUIRE(&pq0.y() == &q0.y());
                REQUIRE(&pq0.z() == &q0.z());

                // Value is q1 though
                REQUIRE(pq0 == q1);
                REQUIRE(q0 == q1);
            }
            SECTION("From a temporary") {
                pq0 = value_type(q1);

                // Still aliases q0
                REQUIRE(&pq0.charge() == &q0.charge());
                REQUIRE(&pq0.x() == &q0.x());
                REQUIRE(&pq0.y() == &q0.y());
                REQUIRE(&pq0.z() == &q0.z());

                // Value is q1 though
                REQUIRE(pq0 == q1);
                REQUIRE(q0 == q1);
            }
        }

        SECTION("copy") {
            view_type copy0(pq0);
            const_view copy1(pq1);

            // Compare addresses to verify alias
            REQUIRE(&copy0.charge() == &pq0.charge());
            REQUIRE(&copy0.x() == &pq0.x());
            REQUIRE(&copy0.y() == &pq0.y());
            REQUIRE(&copy0.z() == &pq0.z());

            REQUIRE(&copy1.charge() == &pq1.charge());
            REQUIRE(&copy1.x() == &pq1.x());
            REQUIRE(&copy1.y() == &pq1.y());
            REQUIRE(&copy1.z() == &pq1.z());
        }

        SECTION("move") {
            view_type copy0(pq0);
            view_type move0(std::move(pq0));
            const_view copy1(pq1);
            const_view move1(std::move(pq1));

            // Compare addresses to verify alias
            REQUIRE(&copy0.charge() == &move0.charge());
            REQUIRE(&copy0.x() == &move0.x());
            REQUIRE(&copy0.y() == &move0.y());
            REQUIRE(&copy0.z() == &move0.z());

            REQUIRE(&copy1.charge() == &move1.charge());
            REQUIRE(&copy1.x() == &move1.x());
            REQUIRE(&copy1.y() == &move1.y());
            REQUIRE(&copy1.z() == &move1.z());
        }

        SECTION("copy assignment") {
            // No default ctor, so we initialize to the opposite state we
            // expect
            view_type copy0(q1.charge(), q1.x(), q1.y(), q1.z());
            auto pcopy0 = &(copy0 = pq0);
            const_view copy1(q0.charge(), q0.x(), q0.y(), q0.z());
            auto pcopy1 = &(copy1 = pq1);

            // Check value (comparing addresses to verify alias)
            REQUIRE(&copy0.charge() == &pq0.charge());
            REQUIRE(&copy0.x() == &pq0.x());
            REQUIRE(&copy0.y() == &pq0.y());
            REQUIRE(&copy0.z() == &pq0.z());

            REQUIRE(&copy1.charge() == &pq1.charge());
            REQUIRE(&copy1.x() == &pq1.x());
            REQUIRE(&copy1.y() == &pq1.y());
            REQUIRE(&copy1.z() == &pq1.z());

            // Check returns *this
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("move") {
            // No default ctor so we initialize to the opposite state we
            // expect
            view_type copy0(pq0);
            view_type move0(q1.charge(), q1.x(), q1.y(), q1.z());
            auto pmove0 = &(move0 = std::move(pq0));
            const_view copy1(pq1);
            const_view move1(q0.charge(), q0.x(), q0.y(), q0.z());
            auto pmove1 = &(move1 = std::move(pq1));

            // Check value (comparing addresses to verify alias)
            REQUIRE(&copy0.charge() == &move0.charge());
            REQUIRE(&copy0.x() == &move0.x());
            REQUIRE(&copy0.y() == &move0.y());
            REQUIRE(&copy0.z() == &move0.z());

            REQUIRE(&copy1.charge() == &move1.charge());
            REQUIRE(&copy1.x() == &move1.x());
            REQUIRE(&copy1.y() == &move1.y());
            REQUIRE(&copy1.z() == &move1.z());

            // Check returns *this
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);
        }
    }

    SECTION("charge()") {
        REQUIRE(&pq0.charge() == &q0.charge());
        REQUIRE(&pq1.charge() == &q1.charge());
    }

    SECTION("charge() const") {
        REQUIRE(&std::as_const(pq0).charge() == &q0.charge());
        REQUIRE(&std::as_const(pq1).charge() == &q1.charge());
    }

    SECTION("Comparisons") {
        // N.B. We test the symmetry of the operator to ensure the view works
        //      seamlessly with PointCharge

        // Compare to same PointCharge
        REQUIRE(pq0 == q0);
        REQUIRE(q0 == pq0);
        REQUIRE_FALSE(pq0 != q0);
        REQUIRE_FALSE(q0 != pq0);
        REQUIRE(pq1 == q1);
        REQUIRE(q1 == pq1);
        REQUIRE_FALSE(pq1 != q1);
        REQUIRE_FALSE(q1 != pq1);

        // Compare to different PointCharge
        REQUIRE_FALSE(pq0 == q1);
        REQUIRE_FALSE(q1 == pq0);
        REQUIRE(pq0 != q1);
        REQUIRE(q1 != pq0);
        REQUIRE_FALSE(pq1 == q0);
        REQUIRE_FALSE(q0 == pq1);
        REQUIRE(pq1 != q0);
        REQUIRE(q0 != pq1);

        // Compare to same PointChargeView
        view_type pq2(q0.charge(), q0.x(), q0.y(), q0.z());
        const_view pq3(q1.charge(), q1.x(), q1.y(), q1.z());
        REQUIRE(pq0 == pq2);
        REQUIRE(pq2 == pq0);
        REQUIRE_FALSE(pq0 != pq2);
        REQUIRE_FALSE(pq2 != pq0);
        REQUIRE(pq1 == pq3);
        REQUIRE(pq3 == pq1);
        REQUIRE_FALSE(pq1 != pq3);
        REQUIRE_FALSE(pq3 != pq1);

        // Compare to different PointChargeView (including different const-ness)
        REQUIRE_FALSE(pq0 == pq3);
        REQUIRE_FALSE(pq3 == pq0);
        REQUIRE(pq0 != pq3);
        REQUIRE(pq3 != pq0);
        REQUIRE_FALSE(pq1 == pq2);
        REQUIRE_FALSE(pq2 == pq1);
        REQUIRE(pq1 != pq2);
        REQUIRE(pq2 != pq1);
    }

    SECTION("as_point_charge") {
        REQUIRE(pq0.as_point_charge() == q0);
        REQUIRE(pq1.as_point_charge() == q1);
    }

    SECTION("swap") {
        view_type pq1_non_const(q1);
        pq0.swap(pq1_non_const);

        REQUIRE(pq0 == view_type(q1));
        REQUIRE(pq1_non_const == view_type(q0));
    }
}
