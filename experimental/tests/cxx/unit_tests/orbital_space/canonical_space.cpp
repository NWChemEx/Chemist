/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/orbital_space/canonical_space.hpp"
#include "test_orbital_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist::orbital_space;

/* For testing purposes we assume:
 *
 * - The base class more-or-less passes through the CanonicalSpace class. i.e.,
 *   if the base class works, it'll work when used with CanonicalSpace too.
 *
 *   - The caveats are that we need to make sure it's accounted for in the ctor,
 *     and comparison operations.
 */

TEST_CASE("CanonicalSpaceD") {
    using space_type  = CanonicalSpaceD;
    using from_space  = chemist::orbital_space::AOSpaceD;
    using tensor_type = chemist::type::tensor;

    auto ev_and_c = testing::get_canonical();
    space_type only_ev(ev_and_c.orbital_energies());
    space_type defaulted;

    const auto& c  = ev_and_c.C();
    const auto& ei = ev_and_c.orbital_energies();

    SECTION("Typedefs") {
        using orb_egy_type = typename space_type::orbital_energy_type;
        STATIC_REQUIRE(std::is_same_v<orb_egy_type, tensor_type>);
    }

    SECTION("CTors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") {
            REQUIRE(only_ev.orbital_energies() == ei);
            REQUIRE(ev_and_c.orbital_energies() == ei);
            REQUIRE(ev_and_c.C() == c);
            REQUIRE(ev_and_c.from_space() == from_space{});
        }

        SECTION("Copy") {
            space_type copy(ev_and_c);
            REQUIRE(copy.orbital_energies() == ei);
            REQUIRE(copy.C() == c);
            REQUIRE(copy.from_space() == from_space{});
        }

        SECTION("Move") {
            space_type moved(std::move(ev_and_c));
            REQUIRE(moved.orbital_energies() == ei);
            REQUIRE(moved.C() == c);
            REQUIRE(moved.from_space() == from_space{});
        }
        SECTION("Copy assignment") {
            space_type copy;
            auto pcopy = &(copy = ev_and_c);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.orbital_energies() == ei);
            REQUIRE(copy.C() == c);
            REQUIRE(copy.from_space() == from_space{});
        }

        SECTION("Move assignment") {
            space_type moved;
            auto pmoved = &(moved = std::move(ev_and_c));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.orbital_energies() == ei);
            REQUIRE(moved.C() == c);
            REQUIRE(moved.from_space() == from_space{});
        }
    }

    SECTION("orbital_energies() const") {
        REQUIRE(ev_and_c.orbital_energies() == ei);

        REQUIRE_THROWS_AS(defaulted.orbital_energies(), std::runtime_error);
    }

    SECTION("Comparisons") {
        SECTION("LHS is default") {
            SECTION("Same value") {
                space_type rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("Different orbital energies") {
                REQUIRE(defaulted != only_ev);
                REQUIRE_FALSE(defaulted == only_ev);
            }

            SECTION("Different base instances") {
                space_type rhs(tensor_type{}, c, from_space{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
        }
    }
}
