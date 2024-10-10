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

#include "../../catch.hpp"
#include <chemist/point_charge/detail_/charges_pimpl.hpp>

using namespace chemist::detail_;

TEMPLATE_TEST_CASE("ChargesPIMPL", "", float, double) {
    using pimpl_type     = ChargesPIMPL<TestType>;
    using value_type     = typename pimpl_type::value_type;
    using point_set_type = typename pimpl_type::point_set_type;
    using charge_type    = typename pimpl_type::charge_type;

    value_type q0(1.0, 2.0, 3.0, 4.0);
    value_type q1(5.0, 6.0, 7.0, 8.0);

    pimpl_type defaulted;
    pimpl_type qs;
    qs.push_back(q0);
    qs.push_back(q1);

    SECTION("Ctors") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }

        SECTION("PointSet and charge vector") {
            point_set_type ps{q0, q1};
            std::vector<charge_type> charges{1.0, 5.0};
            pimpl_type qs2(ps, charges);
            REQUIRE(qs2.size() == 2);
            REQUIRE(qs2[0] == q0);
            REQUIRE(qs2[1] == q1);
        }

        SECTION("copy") {
            pimpl_type copy0(defaulted);
            pimpl_type copy1(qs);

            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == qs);
        }

        SECTION("move") {
            pimpl_type copy0(defaulted);
            pimpl_type move0(std::move(defaulted));
            pimpl_type copy1(qs);
            pimpl_type move1(std::move(qs));

            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);
        }

        SECTION("copy assignment") {
            pimpl_type copy0;
            auto pcopy0 = &(copy0 = defaulted);
            pimpl_type copy1;
            auto pcopy1 = &(copy1 = qs);

            // Compares the values
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == qs);

            // Returns *this
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("move assignment") {
            pimpl_type copy0(defaulted);
            pimpl_type move0;
            auto pmove0 = &(move0 = std::move(defaulted));
            pimpl_type copy1(qs);
            pimpl_type move1;
            auto pmove1 = &(move1 = std::move(qs));

            // Check value
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);

            // Check returns *this
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);
        }
    }

    SECTION("push_back") {
        defaulted.push_back(q0);
        defaulted.push_back(q1);
        REQUIRE(defaulted == qs);
    }

    SECTION("operator[]") {
        REQUIRE(qs[0] == q0);
        REQUIRE(qs[1] == q1);
    }

    SECTION("operator[] const") {
        REQUIRE(std::as_const(qs)[0] == q0);
        REQUIRE(std::as_const(qs)[1] == q1);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(qs.size() == 2);
    }

    SECTION("as_point_set()") {
        using point_set_reference = typename pimpl_type::point_set_reference;
        point_set_type points{q0, q1};

        REQUIRE(defaulted.as_point_set() == point_set_reference{});
        REQUIRE(qs.as_point_set() == point_set_reference(points));
    }

    SECTION("as_point_set() const") {
        using const_point_set_reference =
          typename pimpl_type::const_point_set_reference;

        point_set_type points{q0, q1};
        const_point_set_reference cdefaulted;
        const_point_set_reference cpoints(points);

        REQUIRE(std::as_const(defaulted).as_point_set() == cdefaulted);
        REQUIRE(std::as_const(qs).as_point_set() == cpoints);
    }

    SECTION("charge_data()") {
        REQUIRE(defaulted.charge_data() == nullptr);
        REQUIRE(qs.charge_data() == &qs[0].charge());
    }

    SECTION("Comparisons") {
        // Default equals default
        REQUIRE(defaulted == pimpl_type{});

        // Default does not equal non-default
        REQUIRE_FALSE(defaulted == qs);

        SECTION("Filled with same state") {
            pimpl_type qs1;
            qs1.push_back(q0);
            qs1.push_back(q1);
            REQUIRE(qs == qs1);
        }

        SECTION("Filled with different state") {
            pimpl_type qs1;
            qs1.push_back(q1);
            qs1.push_back(q0);
            REQUIRE_FALSE(qs == qs1);
        }
    }
}
