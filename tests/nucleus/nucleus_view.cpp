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
#include <chemist/nucleus/nucleus_view.hpp>

using namespace chemist;

/* Testing Strategy:
 *
 * We assume that the base class has been tested and that it works (although
 * we do spot check the state of the base in the ctors).
 */

TEST_CASE("NucleusView") {
    using value_type = Nucleus;
    using view_type  = NucleusView<value_type>;
    using const_view = NucleusView<const value_type>;

    value_type n0(1ul, 1.0, 2.0, 3.0, 4.0, 5.0);
    value_type n1(2ul, 2.0, 3.0, 4.0, 5.0, 6.0);

    // This uses ctor 1
    view_type pn0(n0);

    // This uses ctor 2
    typename const_view::point_charge_view_type pq1(n1);
    const_view pn1(n1.Z(), n1.mass(), pq1);

    SECTION("CTors and Assignment") {
        SECTION("Nucleus") {
            REQUIRE(&pn0.Z() == &n0.Z());
            REQUIRE(&pn0.mass() == &n0.mass());
            REQUIRE(&pn0.charge() == &n0.charge());
            REQUIRE(&pn0.x() == &n0.x());
            REQUIRE(&pn0.y() == &n0.y());
            REQUIRE(&pn0.z() == &n0.z());
        }

        SECTION("Z, m, point charge") {
            REQUIRE(&pn1.Z() == &n1.Z());
            REQUIRE(&pn1.mass() == &n1.mass());
            REQUIRE(&pn1.charge() == &n1.charge());
            REQUIRE(&pn1.x() == &n1.x());
            REQUIRE(&pn1.y() == &n1.y());
            REQUIRE(&pn1.z() == &n1.z());
        }

        SECTION("copy") {
            view_type copy0(pq0);
            const_view copy1(pq1);

            // Compare addresses to verify alias
            REQUIRE(&copy0.Z() == &pn0.Z());
            REQUIRE(&copy0.mass() == &pn0.mass());
            REQUIRE(&copy0.charge() == &pn0.charge());
            REQUIRE(&copy0.x() == &pn0.x());
            REQUIRE(&copy0.y() == &pn0.y());
            REQUIRE(&copy0.z() == &pn0.z());

            REQUIRE(&copy1.Z() == &pn1.Z());
            REQUIRE(&copy1.mass() == &pn1.mass());
            REQUIRE(&copy1.charge() == &pn1.charge());
            REQUIRE(&copy1.x() == &pn1.x());
            REQUIRE(&copy1.y() == &pn1.y());
            REQUIRE(&copy1.z() == &pn1.z());
        }

        // TODO: finish changing what's below

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
        //      seamlessly with Nucleus

        // Compare to same Nucleus
        REQUIRE(pq0 == q0);
        REQUIRE(q0 == pq0);
        REQUIRE_FALSE(pq0 != q0);
        REQUIRE_FALSE(q0 != pq0);
        REQUIRE(pq1 == q1);
        REQUIRE(q1 == pq1);
        REQUIRE_FALSE(pq1 != q1);
        REQUIRE_FALSE(q1 != pq1);

        // Compare to different Nucleus
        REQUIRE_FALSE(pq0 == q1);
        REQUIRE_FALSE(q1 == pq0);
        REQUIRE(pq0 != q1);
        REQUIRE(q1 != pq0);
        REQUIRE_FALSE(pq1 == q0);
        REQUIRE_FALSE(q0 == pq1);
        REQUIRE(pq1 != q0);
        REQUIRE(q0 != pq1);

        // Compare to same NucleusView
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

        // Compare to different NucleusView (including different const-ness)
        REQUIRE_FALSE(pq0 == pq3);
        REQUIRE_FALSE(pq3 == pq0);
        REQUIRE(pq0 != pq3);
        REQUIRE(pq3 != pq0);
        REQUIRE_FALSE(pq1 == pq2);
        REQUIRE_FALSE(pq2 == pq1);
        REQUIRE(pq1 != pq2);
        REQUIRE(pq2 != pq1);
    }
}
