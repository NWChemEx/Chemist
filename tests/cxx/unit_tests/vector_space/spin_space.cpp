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
    using chan_container = std::vector<int>;
    chan_container spin_chan_1{ 1, -1 };
    chan_container spin_chan_2{ 2, 0, -2 };
    chan_container spin_chan_3{ 3, 1, -1,-3 };

    SECTION("CTors") {
        SECTION("Default") {
            const auto s = SpinSpace();
            REQUIRE(s.size() == 0);
	    REQUIRE_THROWS(s.spin_chan(0));
	}

	SECTION("With total spin") {
            auto s = SpinSpace(2);
            REQUIRE(s.size() == 3);
            REQUIRE(s.spin_chan(0) == spin_chan_2[0]);
	    REQUIRE(s.spin_chan(1) == spin_chan_2[1]);
	    REQUIRE(s.spin_chan(2) == spin_chan_2[2]);
        }

	SECTION("Copy constructor") {
            auto s3_1 = SpinSpace(3);
	    auto s3_2 = SpinSpace(s3_1);
	    REQUIRE(s3_2.size() == 4);
            REQUIRE(s3_2.spin_chan(0) == spin_chan_3[0]);
	    REQUIRE(s3_2.spin_chan(1) == spin_chan_3[1]);
	    REQUIRE(s3_2.spin_chan(2) == spin_chan_3[2]);
	    REQUIRE(s3_2.spin_chan(3) == spin_chan_3[3]);
	}

	SECTION("Move constructor") {
            auto s3_1 = SpinSpace(3);
	    auto s3_2 = SpinSpace(std::move(s3_1));
	    REQUIRE(s3_2.size() == 4);
            REQUIRE(s3_2.spin_chan(0) == spin_chan_3[0]);
	    REQUIRE(s3_2.spin_chan(1) == spin_chan_3[1]);
            REQUIRE(s3_2.spin_chan(2) == spin_chan_3[2]);
            REQUIRE(s3_2.spin_chan(3) == spin_chan_3[3]);
	}
    }

    SECTION("Assignment") {
        SECTION("Copy assignment") {
            auto s1_1 = SpinSpace(1);
	    auto copy  = SpinSpace();
            auto ps1_2 = &(copy = s1_1);
	    REQUIRE(ps1_2 == &copy);
	    REQUIRE(ps1_2->spin_chan(0) == spin_chan_1[0]);
	    REQUIRE(ps1_2->spin_chan(1) == spin_chan_1[1]);
	}

	SECTION("Move assignment") {
            auto s1_1 = SpinSpace(1);
	    auto moved = SpinSpace();
	    auto ps1_2 = &(moved = std::move(s1_1));
	    REQUIRE(ps1_2 == &moved);
            REQUIRE(ps1_2->spin_chan(0) == spin_chan_1[0]);
	    REQUIRE(ps1_2->spin_chan(1) == spin_chan_1[1]);
	}
    }

    SECTION("Accessors") {
        SECTION("Total Spin") {
            auto s = SpinSpace(3);
	    REQUIRE(s.TSpin() == 3);
	}

        SECTION("Spin channel: out of range") {
            auto s = SpinSpace(3);
            REQUIRE_THROWS(s.spin_chan(4));
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
