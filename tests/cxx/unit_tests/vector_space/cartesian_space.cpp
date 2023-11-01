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

#include "chemist/vector_space/cartesian_space.hpp"
#include <catch2/catch.hpp>
#include <string>

using namespace chemist;
using namespace chemist::vector_space;
using namespace Catch::Matchers;

TEST_CASE("CartesianSpace") {
    SECTION("CTors") {
        SECTION("Axes not set") {
            auto s = CartesianSpace(3);
            REQUIRE(s.size() == 3);
	    REQUIRE(s.axis_vec == decltype(s.axis_vec){});
        }

	SECTION("Axes set") {
            std::vector<std::string> val{"z","x","y","z"};
            REQUIRE_THROWS(CartesianSpace(3, val));
        }

	SECTION("Copy constructor") {
            std::vector<std::string> val{"x","y","z"};
	    auto s3_1 = CartesianSpace(3,val);
	    auto s3_2 = CartesianSpace(s3_1);
            REQUIRE(s3_2.axis_vec[1] == "y");
        }
    }

    SECTION("Accessors") {
        SECTION("Axis labels") {
            std::vector<std::string> val{"z","x","y","z"};
            auto s = CartesianSpace(4, val);
            REQUIRE(s.axis_vec[0] == "z");
            REQUIRE(s.axis_vec[1] == "x");
            REQUIRE(s.axis_vec[2] == "y");
            REQUIRE(s.axis_vec[3] == "z");
        }

	SECTION("Tensor labels") {
            std::vector<std::string> val{"xx","yy","zz","xy","yz","zx"};
            auto s = CartesianSpace(6, val);
            REQUIRE(s.axis_vec[0] == "xx");
            REQUIRE(s.axis_vec[1] == "yy");
            REQUIRE(s.axis_vec[2] == "zz");
            REQUIRE(s.axis_vec[3] == "xy");
	    REQUIRE(s.axis_vec[4] == "yz");
	    REQUIRE(s.axis_vec[5] == "zx");
        }
    }

    SECTION("Comparison") {
	SECTION("Axes not set") {
            auto s2_1 = CartesianSpace(2);
            auto s2_2 = CartesianSpace(2);
            auto s3   = CartesianSpace(3);
            REQUIRE(s2_1 == s2_2);
            REQUIRE_FALSE(s2_1 == s3);
	}

	SECTION("Axes set for one space") {
            auto s2_1 = CartesianSpace(2);
	    std::vector<std::string> val2{"x","y"};
	    std::vector<std::string> val3{"x","y","z"};
            auto s2_2 = CartesianSpace(2,val2);
            auto s3_1 = CartesianSpace(3,val3);
	    auto s3_2 = CartesianSpace(3);
	    REQUIRE_FALSE(s2_1 == s3_1);
            REQUIRE_FALSE(s2_1 == s2_2);
            REQUIRE_FALSE(s3_1 == s3_2);
        }

	SECTION("Axes set for both spaces") {
            auto s2_1 = CartesianSpace(2);
            std::vector<std::string> val3_1{"x","y","z"};
            std::vector<std::string> val3_2{"x","y","z1"};
            auto s3_1 = CartesianSpace(3,val3_1);
            auto s3_2 = CartesianSpace(3,val3_1);
	    auto s3_3 = CartesianSpace(3,val3_2);
            REQUIRE(s3_1 == s3_2);
            REQUIRE_FALSE(s3_1 == s3_3);
        }
    }
}
