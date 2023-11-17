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

#include <catch2/catch.hpp>
#include <chemist/vector_space/spin_space.hpp>

using namespace chemist;
using namespace chemist::vector_space;
using namespace Catch::Matchers;

TEST_CASE("SpinSpace") {
    SECTION("CTors") {
        SECTION("Default") {
            const auto s = SpinSpace();
            REQUIRE(s.size() == 0);
	    REQUIRE_THROWS(s.spin_channel(0));
	}

	SECTION("With total spin") {
            auto s = SpinSpace(3);
            REQUIRE(s.size() == 3);
            REQUIRE_THAT(s.spin_channel(0), WithinAbs(1.0, 0.00001));
	    REQUIRE_THAT(s.spin_channel(1), WithinAbs(0.0, 0.00001));
	    REQUIRE_THAT(s.spin_channel(2), WithinAbs(-1.0, 0.00001));
        }

	SECTION("Copy constructor") {
            auto s3_1 = SpinSpace(4);
	    auto s3_2 = SpinSpace(s3_1);
	    REQUIRE(s3_2.size() == 4);
            REQUIRE_THAT(s3_2.spin_channel(0), WithinAbs(1.5, 0.00001));
	    REQUIRE_THAT(s3_2.spin_channel(1), WithinAbs(0.5, 0.00001));
	    REQUIRE_THAT(s3_2.spin_channel(2), WithinAbs(-0.5, 0.00001));
	    REQUIRE_THAT(s3_2.spin_channel(3), WithinAbs(-1.5, 0.00001));
	}

	SECTION("Move constructor") {
            auto s3_1 = SpinSpace(4);
	    auto s3_2 = SpinSpace(std::move(s3_1));
	    REQUIRE(s3_2.size() == 4);
	    REQUIRE_THAT(s3_2.spin_channel(0), WithinAbs(1.5, 0.00001));
            REQUIRE_THAT(s3_2.spin_channel(1), WithinAbs(0.5, 0.00001));
            REQUIRE_THAT(s3_2.spin_channel(2), WithinAbs(-0.5, 0.00001));
            REQUIRE_THAT(s3_2.spin_channel(3), WithinAbs(-1.5, 0.00001));
	}
    }

    SECTION("Assignment") {
        SECTION("Copy assignment") {
            auto s1_1 = SpinSpace(2);
	    auto copy  = SpinSpace();
            auto ps1_2 = &(copy = s1_1);
	    REQUIRE(ps1_2 == &copy);
	    REQUIRE(ps1_2->size() == 2);
	    REQUIRE_THAT(ps1_2->spin_channel(0), WithinAbs(0.5, 0.00001));
	    REQUIRE_THAT(ps1_2->spin_channel(1), WithinAbs(-0.5, 0.00001));
	}

	SECTION("Move assignment") {
            auto s1_1 = SpinSpace(2);
	    auto moved = SpinSpace();
	    auto ps1_2 = &(moved = std::move(s1_1));
	    REQUIRE(ps1_2 == &moved);
	    REQUIRE(ps1_2->size() == 2);
	    REQUIRE_THAT(ps1_2->spin_channel(0), WithinAbs(0.5, 0.00001));
            REQUIRE_THAT(ps1_2->spin_channel(1), WithinAbs(-0.5, 0.00001));
	}
    }

    SECTION("Accessors") {
        SECTION("Total Spin") {
            auto s = SpinSpace(3);
	    REQUIRE_THAT(s.total_spin(), WithinAbs(1.0, 0.00001));
	}

        SECTION("Spin channel: out of range") {
            auto s = SpinSpace(3);
            REQUIRE_THROWS(s.spin_channel(4));
        }
    }

    SECTION("Clone") {
        auto s3_1  = SpinSpace(3);
        auto ps3_2 = s3_1.clone();
        REQUIRE(ps3_2->equal(s3_1));
    }

    SECTION("Comparison") {
        SECTION("Operator ==") {
            auto s2_1 = SpinSpace(2);
            auto s2_2 = SpinSpace(2);
            auto s3   = SpinSpace(3);
            REQUIRE(s2_1 == s2_2);
            REQUIRE_FALSE(s2_1 == s3);
        }

        SECTION("Operator !=") {
            auto s2_1 = SpinSpace(2);
            auto s2_2 = SpinSpace(2);
            auto s3   = SpinSpace(3);
            REQUIRE_FALSE(s2_1 != s2_2);
            REQUIRE(s2_1 != s3);
        }
    }
}
