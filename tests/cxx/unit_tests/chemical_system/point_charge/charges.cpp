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
#include <cereal/archives/binary.hpp>
#include <chemist/chemical_system/point_charge/charges.hpp>
#include <sstream>

using namespace chemist;

/* Testing Notes:
 *
 * We assume the base class has been tested and works correctly. Subject to
 * that assumption, we need to test the ability to access elements by reference
 * (since we wrote the implementation), and obtaining the size, but do not need
 * to retest all the ways one can access the elements (i.e., operator[], at, and
 * iterators).
 */
TEMPLATE_TEST_CASE("Charges", "", float, double) {
    using set_type   = Charges<TestType>;
    using value_type = typename set_type::value_type;

    value_type q0(0.0, 1.0, 2.0, 3.0);
    value_type q1(4.0, 5.0, 6.0, 7.0);

    set_type defaulted;
    set_type charges{q0, q1, q1}; // <- ensure we can add same charge 2x

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }

        SECTION("initializer_list") {
            REQUIRE(charges.size() == 3);
            REQUIRE(charges[0] == q0);
            REQUIRE(charges[1] == q1);
            REQUIRE(charges[2] == q1);
        }

        SECTION("Copy") {
            set_type copy0(defaulted);
            set_type copy1(charges);
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == charges);
        }

        SECTION("Move") {
            set_type copy0(defaulted);
            set_type move0(std::move(defaulted));
            set_type copy1(charges);

            // Do the move
            set_type move1(std::move(charges));

            // Check the values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);
        }

        SECTION("Copy assignment") {
            set_type copy0;
            auto pcopy0 = &(copy0 = defaulted);
            set_type copy1;
            auto pcopy1 = &(copy1 = charges);

            // Check values
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == charges);

            // Check returns *this for chaining
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("Move assignment") {
            set_type copy0(defaulted);
            set_type move0;
            auto pmove0 = &(move0 = std::move(defaulted));
            set_type copy1(charges);
            set_type move1;

            // Do the move
            auto pmove1 = &(move1 = std::move(charges));

            // Check values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);

            // Check returns *this for chaining
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);
        }
    }

    SECTION("push_back") {
        defaulted.push_back(q0);
        defaulted.push_back(q1);
        defaulted.push_back(q1);
        REQUIRE(defaulted == charges);
    }

    SECTION("at_()") {
        using rtype = decltype(charges[0]);
        STATIC_REQUIRE(std::is_same_v<rtype, typename set_type::reference>);

        for(std::size_t charge_i = 0; charge_i < 3; ++charge_i) {
            const auto corr = charge_i == 0 ? q0 : q1;
            auto qi         = charges[charge_i];

            REQUIRE(qi.charge() == corr.charge());

            for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                auto& coord = qi.coord(coord_i);

                REQUIRE(coord == corr.coord(coord_i));
            }
        }
    }

    SECTION("at_() const") {
        using rtype = decltype(std::as_const(charges)[0]);
        using corr  = typename set_type::const_reference;
        STATIC_REQUIRE(std::is_same_v<rtype, corr>);

        for(std::size_t charge_i = 0; charge_i < 3; ++charge_i) {
            const auto corr = charge_i == 0 ? q0 : q1;
            auto qi         = std::as_const(charges)[charge_i];

            REQUIRE(qi.charge() == corr.charge());

            for(std::size_t coord_i = 0; coord_i < 3; ++coord_i) {
                auto& coord = std::as_const(charges)[charge_i].coord(coord_i);

                REQUIRE(coord == corr.coord(coord_i));
            }
        }
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(charges.size() == 3);
    }

    SECTION("serialization") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive oarchive(ss);
            oarchive(charges);
        }
        {
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(defaulted);
        }
        REQUIRE(defaulted == charges);
    }
}
