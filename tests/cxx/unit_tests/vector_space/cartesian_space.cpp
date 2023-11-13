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
#include <chemist/vector_space/cartesian_space.hpp>
#include <string>

using namespace chemist;
using namespace chemist::vector_space;
using namespace Catch::Matchers;

TEST_CASE("CartesianSpace") {
    using label_container = std::vector<std::string>;
    label_container val_r3{"x", "y", "z"};
    label_container val_r4{"z", "x", "y", "z"};

    SECTION("CTors") {
        SECTION("Axes not set") {
            auto s = CartesianSpace(3);
            REQUIRE(s.size() == 3);
	    REQUIRE(s.const_label(0) == "");
	    REQUIRE(s.const_label(1) == "");
	    REQUIRE(s.const_label(2) == "");
        }

        SECTION("Axes set") {
	    auto s4 = CartesianSpace(4, val_r4.begin(), val_r4.end());
            REQUIRE(s4.const_label(0) == "z");
	    REQUIRE(s4.const_label(1) == "x");
	    REQUIRE(s4.const_label(2) == "y");
	    REQUIRE(s4.const_label(3) == "z");
            REQUIRE_THROWS(CartesianSpace(3, val_r4.begin(), val_r4.end()));
        }

        SECTION("Copy constructor") {
            auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
            auto s3_2 = CartesianSpace(s3_1);
	    for (int i=0;i<3;i++) {
                REQUIRE(s3_1.const_label(i) == s3_2.const_label(i));
	    }
        }

        SECTION("Move constructor") {
            auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
            auto s3_2 = CartesianSpace(std::move(s3_1));
	    REQUIRE(s3_2.const_label(0) == "x");
            REQUIRE(s3_2.const_label(1) == "y");
	    REQUIRE(s3_2.const_label(2) == "z");
        }
    }

    SECTION("Clone") {
        auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
        auto ps3_2 = s3_1.clone();
        REQUIRE(ps3_2->size() == 3);
    }

    SECTION("Accessors") {
        SECTION("Get one label") {
            auto s = CartesianSpace(4, val_r4.begin(), val_r4.end());
            REQUIRE(s.const_label(2) == "y");
        }

	SECTION("Get and set one label") {
            auto s = CartesianSpace(4, val_r4.begin(), val_r4.end());
            s.label(3) = "not z";
            REQUIRE(s.const_label(3) == "not z");
        }

        SECTION("Tensor labels") {
            label_container val{"xx", "yy", "zz", "xy", "yz", "zx"};
            auto s = CartesianSpace(6, val.begin(), val.end());
            REQUIRE(s.const_label(0) == "xx");
            REQUIRE(s.const_label(1) == "yy");
            REQUIRE(s.const_label(2) == "zz");
            REQUIRE(s.const_label(3) == "xy");
	    REQUIRE(s.const_label(4) == "yz");
	    REQUIRE(s.const_label(5) == "zx");
        }
    }

    SECTION("Assignment") {
        SECTION("Copy assignment") {
            auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
	    auto copy = CartesianSpace(3);
            auto ps3_2 = &(copy = s3_1);
	    REQUIRE(ps3_2 == &copy);
	    REQUIRE(ps3_2->const_label(0) == "x");
            REQUIRE(ps3_2->const_label(1) == "y");
	    REQUIRE(ps3_2->const_label(2) == "z");
	}

	SECTION("Move assignment") {
            auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
	    auto moved = CartesianSpace(3);
            auto ps3_2 = &(moved = std::move(s3_1));
	    REQUIRE(ps3_2 == &moved);
	    REQUIRE(ps3_2->const_label(0) == "x");
            REQUIRE(ps3_2->const_label(1) == "y");
            REQUIRE(ps3_2->const_label(2) == "z");
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
            label_container val2{"x", "y"};
            auto s2_2 = CartesianSpace(2, val2.begin(), val2.end());
            auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
            auto s3_2 = CartesianSpace(3);
            REQUIRE_FALSE(s2_1 == s3_1);
            REQUIRE_FALSE(s2_1 == s2_2);
            REQUIRE_FALSE(s3_1 == s3_2);
        }

        SECTION("Axes set for both spaces") {
            auto s2_1 = CartesianSpace(2);
            label_container val3_2{"x", "y", "z1"};
            auto s3_1 = CartesianSpace(3, val_r3.begin(), val_r3.end());
            auto s3_2 = CartesianSpace(3, val_r3.begin(), val_r3.end());
            auto s3_3 = CartesianSpace(3, val3_2.begin(), val3_2.end());
            REQUIRE(s3_1 == s3_2);
            REQUIRE_FALSE(s3_1 == s3_3);
        }
    }
}
