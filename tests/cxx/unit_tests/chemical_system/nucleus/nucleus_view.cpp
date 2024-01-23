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
#include <chemist/chemical_system/nucleus/nucleus_view.hpp>

using namespace chemist;

/* Testing Strategy:
 *
 * We assume that the base class has been tested and that it works (although
 * we do spot check the state of the base in the ctors).
 */

/// Code factorization for checking addresses match for all state
template<typename T, typename U>
void check_addresses(T&& lhs, U&& rhs) {
    REQUIRE(&lhs.name() == &rhs.name());
    REQUIRE(&lhs.Z() == &rhs.Z());
    REQUIRE(&lhs.mass() == &rhs.mass());
    REQUIRE(&lhs.charge() == &rhs.charge());
    REQUIRE(&lhs.x() == &rhs.x());
    REQUIRE(&lhs.y() == &rhs.y());
    REQUIRE(&lhs.z() == &rhs.z());
}

TEST_CASE("NucleusView") {
    using value_type = Nucleus;
    using view_type  = NucleusView<value_type>;
    using const_view = NucleusView<const value_type>;

    value_type n0("H", 1ul, 1.0, 2.0, 3.0, 4.0, 5.0);
    value_type n1("He", 2ul, 2.0, 3.0, 4.0, 5.0, 6.0);

    // This uses ctor 1
    view_type pn0(n0);

    // This uses ctor 2
    typename const_view::charge_view_type pq1(n1);
    const_view pn1(n1.name(), n1.Z(), n1.mass(), pq1);

    SECTION("CTors and Assignment") {
        SECTION("Nucleus") { check_addresses(pn0, n0); }

        SECTION("Z, m, point charge") { check_addresses(pn1, n1); }

        SECTION("copy") {
            view_type copy0(pn0);
            const_view copy1(pn1);

            // Compare addresses to verify alias
            check_addresses(copy0, pn0);
            check_addresses(copy1, pn1);
        }

        SECTION("move") {
            view_type copy0(pn0);
            view_type move0(std::move(pn0));
            const_view copy1(pn1);
            const_view move1(std::move(pn1));

            // Compare addresses to verify alias
            check_addresses(copy0, move0);
            check_addresses(copy1, move1);
        }

        SECTION("Assign from Nucleus") {
            SECTION("Assign from a value") {
                pn0 = n1;

                // Addresses should still match that of n0
                check_addresses(pn0, n0);

                // State of pn0 and n0 should now compare equal to that of n1
                REQUIRE(pn0 == n1);
                REQUIRE(n0 == n1);
            }
            SECTION("Assign from a temporary") {
                pn0 = value_type(n1);

                // Addresses should still match that of n0
                check_addresses(pn0, n0);

                // State of pn0 and n0 should now compare equal to that of n1
                REQUIRE(pn0 == n1);
                REQUIRE(n0 == n1);
            }
        }

        SECTION("copy assignment") {
            // No default ctor, so we initialize to the opposite state we
            // expect
            view_type copy0(n1);
            auto pcopy0 = &(copy0 = pn0);
            const_view copy1(n0);
            auto pcopy1 = &(copy1 = pn1);

            // Check value (comparing addresses to verify alias)
            check_addresses(copy0, pn0);
            check_addresses(copy1, pn1);

            // Check returns *this
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("move assignment") {
            // No default ctor so we initialize to the opposite state we
            // expect
            view_type copy0(pn0);
            view_type move0(n1);
            auto pmove0 = &(move0 = std::move(pn0));
            const_view copy1(pn1);
            const_view move1(n0);
            auto pmove1 = &(move1 = std::move(pn1));

            // Check value (comparing addresses to verify alias)
            check_addresses(copy0, move0);
            check_addresses(copy1, move1);

            // Check returns *this
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);
        }
    }

    SECTION("name()") {
        REQUIRE(&pn0.name() == &n0.name());
        REQUIRE(&pn1.name() == &n1.name());
    }

    SECTION("name() const") {
        REQUIRE(&std::as_const(pn0).name() == &n0.name());
        REQUIRE(&std::as_const(pn1).name() == &n1.name());
    }

    SECTION("Z()") {
        REQUIRE(&pn0.Z() == &n0.Z());
        REQUIRE(&pn1.Z() == &n1.Z());
    }

    SECTION("Z() const") {
        REQUIRE(&std::as_const(pn0).Z() == &n0.Z());
        REQUIRE(&std::as_const(pn1).Z() == &n1.Z());
    }

    SECTION("mass()") {
        REQUIRE(&pn0.Z() == &n0.Z());
        REQUIRE(&pn1.Z() == &n1.Z());
    }

    SECTION("mass() const") {
        REQUIRE(&std::as_const(pn0).Z() == &n0.Z());
        REQUIRE(&std::as_const(pn1).Z() == &n1.Z());
    }

    SECTION("Comparisons") {
        // N.B. We test the symmetry of the operator to ensure the view works
        //      seamlessly with Nucleus

        // Compare to same Nucleus
        REQUIRE(pn0 == n0);
        REQUIRE(n0 == pn0);
        REQUIRE_FALSE(pn0 != n0);
        REQUIRE_FALSE(n0 != pn0);
        REQUIRE(pn1 == n1);
        REQUIRE(n1 == pn1);
        REQUIRE_FALSE(pn1 != n1);
        REQUIRE_FALSE(n1 != pn1);

        // Compare to different Nucleus
        REQUIRE_FALSE(pn0 == n1);
        REQUIRE_FALSE(n1 == pn0);
        REQUIRE(pn0 != n1);
        REQUIRE(n1 != pn0);
        REQUIRE_FALSE(pn1 == n0);
        REQUIRE_FALSE(n0 == pn1);
        REQUIRE(pn1 != n0);
        REQUIRE(n0 != pn1);

        // Compare to same NucleusView
        view_type pq2(n0);
        const_view pq3(n1);
        REQUIRE(pn0 == pq2);
        REQUIRE(pq2 == pn0);
        REQUIRE_FALSE(pn0 != pq2);
        REQUIRE_FALSE(pq2 != pn0);
        REQUIRE(pn1 == pq3);
        REQUIRE(pq3 == pn1);
        REQUIRE_FALSE(pn1 != pq3);
        REQUIRE_FALSE(pq3 != pn1);

        // Compare to different NucleusView (including different const-ness)
        REQUIRE_FALSE(pn0 == pq3);
        REQUIRE_FALSE(pq3 == pn0);
        REQUIRE(pn0 != pq3);
        REQUIRE(pq3 != pn0);
        REQUIRE_FALSE(pn1 == pq2);
        REQUIRE_FALSE(pq2 == pn1);
        REQUIRE(pn1 != pq2);
        REQUIRE(pq2 != pn1);
    }

    SECTION("Printing") {
        std::stringstream ss;
        ss << pn0;

        std::string corr =
          "H 2.000000000000000 3.000000000000000 4.000000000000000";

        REQUIRE(ss.str() == corr);
    }

    SECTION("swap") {
        view_type pn1_non_const(n1);
        pn0.swap(pn1_non_const);

        REQUIRE(pn0 == view_type(n1));
        REQUIRE(pn1_non_const == view_type(n0));
    }
}
