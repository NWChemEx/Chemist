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

// Only what CartesianAO itself implements is tested here: its ctors,
// assignment, swap, and serialization. Everything CartesianAO inherits from
// CartesianAOCommon is tested once, generically, in cartesian_ao_common.cpp,
// and what it inherits from AO is tested in ao_and_ao_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/cartesian_ao_class.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <sstream>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::CartesianAO") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    // f_xyz function: i = j = k = 1, so l = 3.
    CartesianAO fxyz(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(1),
                     std::size_t(1), std::size_t(1), 1.0, 2.0, 3.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            CartesianAO defaulted;
            REQUIRE(defaulted.get_i() == 0);
            REQUIRE(defaulted.get_j() == 0);
            REQUIRE(defaulted.get_k() == 0);
            REQUIRE(defaulted.get_l() == 0);
            REQUIRE(defaulted.get_contracted_gaussian().size() == 0);
            REQUIRE(defaulted.get_center() == Point(0.0, 0.0, 0.0));

            // The zero function: no primitives to sum.
            REQUIRE(as_double(defaulted.evaluate(Point(1.0, 1.0, 1.0))) == 0.0);
        }

        SECTION("A default-constructed AO can be built up afterwards") {
            // The powers are set directly and the contraction is swapped in,
            // which is the same division of labor ContractedGaussian documents
            // for itself.
            CartesianAO built;
            built.set_i(1ul);
            built.set_k(2ul);
            REQUIRE(built.get_l() == 3);
            REQUIRE(built.get_contracted_gaussian().get_l() == 3);

            CartesianAO wanted(cs.begin(), cs.end(), es.begin(), es.end(),
                               std::size_t(1), std::size_t(0), std::size_t(2),
                               1.0, 2.0, 3.0);
            built.swap(wanted);
            REQUIRE(built.get_i() == 1);
            REQUIRE(built.get_k() == 2);
            REQUIRE(built.get_center() == Point(1.0, 2.0, 3.0));
            REQUIRE(built.get_contracted_gaussian().size() == 2);
        }

        SECTION("From ranges and coordinates") {
            REQUIRE(fxyz.get_i() == 1);
            REQUIRE(fxyz.get_j() == 1);
            REQUIRE(fxyz.get_k() == 1);
            REQUIRE(fxyz.get_center() == Point(1.0, 2.0, 3.0));

            const auto& cg = fxyz.get_contracted_gaussian();
            REQUIRE(cg.size() == 2);
            REQUIRE(as_double(cg[0].get_coefficient()) == 2.0);
            REQUIRE(as_double(cg[1].get_exponent()) == 2.0);
        }

        SECTION("The contraction's l comes from the powers") {
            // The powers are given instead of l, not in addition to it, so
            // there is nothing which could disagree.
            REQUIRE(fxyz.get_contracted_gaussian().get_l() == 3);

            CartesianAO s(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(0), std::size_t(0), std::size_t(0), 0.0,
                          0.0, 0.0);
            REQUIRE(s.get_contracted_gaussian().get_l() == 0);
        }

        SECTION("From ranges and a Point") {
            CartesianAO from_point(cs.begin(), cs.end(), es.begin(), es.end(),
                                   std::size_t(1), std::size_t(1),
                                   std::size_t(1), Point(1.0, 2.0, 3.0));
            REQUIRE(from_point == fxyz);
        }

        SECTION("Throws if the coefficient and exponent ranges differ") {
            std::vector<double> short_es{1.0};
            REQUIRE_THROWS_AS(CartesianAO(cs.begin(), cs.end(),
                                          short_es.begin(), short_es.end(),
                                          std::size_t(1), std::size_t(1),
                                          std::size_t(1), 0.0, 0.0, 0.0),
                              std::invalid_argument);
        }

        SECTION("Copy/move") {
            test_chemist::test_copy_and_move(fxyz, CartesianAO{});
        }

        SECTION("Copying is deep") {
            CartesianAO copied(fxyz);
            copied.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
            REQUIRE(fxyz.get_center() == Point(1.0, 2.0, 3.0));
        }
    }

    SECTION("swap") {
        CartesianAO other;
        CartesianAO fxyz_copy(fxyz);
        CartesianAO other_copy(other);

        fxyz.swap(other);
        REQUIRE(fxyz == other_copy);
        REQUIRE(other == fxyz_copy);
    }

    SECTION("save/load") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(fxyz);
        }
        CartesianAO deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == fxyz);
    }

    SECTION("save/load round-trips a default-constructed AO") {
        CartesianAO defaulted;
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(defaulted);
        }
        CartesianAO deserialized(fxyz);
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == defaulted);
    }
}
