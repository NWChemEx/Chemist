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

// Only what ContractedGaussian itself implements is tested here: its ctors,
// assignment, swap, and load. Everything ContractedGaussian inherits from
// ContractedGaussianCommon --- get_/set_ accessors, indexing, evaluate,
// normalized_evaluate, normalization_constant, the comparison operators, and
// save --- is tested once in contracted_gaussian_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian_class.hpp>
#include <sstream>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::ContractedGaussian") {
    ContractedGaussian defaulted;

    std::vector<double> cs{1.0, 1.0};
    std::vector<double> es{1.0, 2.0};
    ContractedGaussian s(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(0), 1.0, 2.0, 3.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.get_l() == 0);
            REQUIRE(defaulted.get_center() == Point(0.0, 0.0, 0.0));
        }

        SECTION("coefficients, exponents, l, x, y, z") {
            REQUIRE(s.size() == 2);
            REQUIRE(s.get_l() == 0);
            REQUIRE(s.get_center() == Point(1.0, 2.0, 3.0));
            REQUIRE(as_double(s[0].get_coefficient()) == 1.0);
            REQUIRE(as_double(s[0].get_exponent()) == 1.0);
            REQUIRE(as_double(s[1].get_coefficient()) == 1.0);
            REQUIRE(as_double(s[1].get_exponent()) == 2.0);
        }

        SECTION("coefficients, exponents, l, center") {
            ContractedGaussian from_center(cs.begin(), cs.end(), es.begin(),
                                           es.end(), std::size_t(0),
                                           Point(1.0, 2.0, 3.0));
            REQUIRE(from_center == s);
        }

        SECTION("Mismatched coefficient/exponent lengths throw") {
            std::vector<double> bad_es{1.0};
            REQUIRE_THROWS_AS(ContractedGaussian(cs.begin(), cs.end(),
                                                 bad_es.begin(), bad_es.end(),
                                                 std::size_t(0), 0.0, 0.0, 0.0),
                              std::invalid_argument);
        }

        test_chemist::test_copy_and_move(defaulted, s);
    }

    SECTION("swap") {
        ContractedGaussian lhs(s);
        ContractedGaussian rhs(defaulted);
        lhs.swap(rhs);
        REQUIRE(lhs == defaulted);
        REQUIRE(rhs == s);
    }

    SECTION("save/load round trip") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(s);
        }
        ContractedGaussian deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == s);
    }

    SECTION("save/load round trip on an empty contraction") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(defaulted);
        }
        ContractedGaussian deserialized(cs.begin(), cs.end(), es.begin(),
                                        es.end(), std::size_t(0), 0.0, 0.0,
                                        0.0);
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == defaulted);
    }
}
