/*
 * Copyright 2026 NWChemEx-Project
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

// Only what Primitive itself implements is tested here: its ctors,
// assignment, swap, and load. Everything Primitive inherits from
// PrimitiveCommon --- get_/set_ accessors, evaluate, normalized_evaluate,
// the comparison operators, and save --- is tested once in
// primitive_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/primitive_class.hpp>
#include <sstream>
#include <utility>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::Primitive") {
    Primitive defaulted;
    Primitive s(2.0, 1.0, std::size_t(1), 1.0, 2.0, 3.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(as_double(defaulted.get_coefficient()) == 0.0);
            REQUIRE(as_double(defaulted.get_exponent()) == 0.0);
            REQUIRE(defaulted.get_l() == 0);
            REQUIRE(defaulted.get_center() == Point(0.0, 0.0, 0.0));
        }

        SECTION("coefficient, exponent, l, x, y, z") {
            REQUIRE(as_double(s.get_coefficient()) == 2.0);
            REQUIRE(as_double(s.get_exponent()) == 1.0);
            REQUIRE(s.get_l() == 1);
            REQUIRE(s.get_center() == Point(1.0, 2.0, 3.0));

            // The concrete type is deduced from the arguments, not fixed
            Primitive as_floats(2.0f, 1.0f, std::size_t(1), 1.0f, 2.0f, 3.0f);
            REQUIRE(as_floats.get_coefficient() == 2.0f);

            // ... and a float-valued primitive is NOT a double-valued one
            REQUIRE(as_floats != s);
        }

        SECTION("Type-erased coefficient, exponent, x, y, z") {
            using coord_type = Primitive::coord_type;
            Primitive erased(coord_type(2.0), coord_type(1.0), std::size_t(1),
                             coord_type(1.0), coord_type(2.0), coord_type(3.0));
            REQUIRE(erased == s);
        }

        SECTION("coefficient, exponent, l, center") {
            Primitive from_center(Primitive::coefficient_type(2.0),
                                  Primitive::exponent_type(1.0), std::size_t(1),
                                  Point(1.0, 2.0, 3.0));
            REQUIRE(from_center == s);
        }

        test_chemist::test_copy_and_move(defaulted, s);
    }

    SECTION("swap") {
        Primitive lhs(s);
        Primitive rhs(defaulted);
        lhs.swap(rhs);
        REQUIRE(lhs == defaulted);
        REQUIRE(rhs == s);
    }

    SECTION("load") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(s);
        }
        Primitive deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == s);
    }

    SECTION("load restores the concrete type") {
        Primitive as_floats(2.0f, 1.0f, std::size_t(1), 1.0f, 2.0f, 3.0f);
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(as_floats);
        }
        Primitive deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == as_floats);
        REQUIRE(deserialized != s);
    }
}
