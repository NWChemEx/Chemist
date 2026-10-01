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

// Only what CCAShell itself implements is tested here: its ctors, assignment,
// swap, and serialization. Everything CCAShell inherits from CCAShellCommon is
// tested in cca_shell_common.cpp, and what it inherits from AOShell (including
// the polymorphic hooks) in ao_shell_and_ao_shell_view.cpp. CCAShell's members
// do not depend on the purity, so they are tested for both purities at once.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/cca_shell_class.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <sstream>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEMPLATE_TEST_CASE("experimental::CCAShell", "", CartesianAO, SphericalAO) {
    using shell_type = CCAShell<TestType>;

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    shell_type f(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(3),
                 1.0, 2.0, 3.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            shell_type defaulted;
            REQUIRE(defaulted.get_l() == 0);
            REQUIRE(defaulted.size() == 1);
            REQUIRE(defaulted.get_contracted_gaussian().size() == 0);
            REQUIRE(defaulted.get_center() == Point(0.0, 0.0, 0.0));
        }

        SECTION("From ranges and coordinates") {
            REQUIRE(f.get_l() == 3);
            REQUIRE(f.get_center() == Point(1.0, 2.0, 3.0));
            const auto& cg = f.get_contracted_gaussian();
            REQUIRE(cg.size() == 2);
            REQUIRE(as_double(cg[0].get_coefficient()) == 2.0);
            REQUIRE(as_double(cg[1].get_exponent()) == 2.0);
        }

        SECTION("From ranges and a Point") {
            shell_type from_point(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(3), Point(1.0, 2.0, 3.0));
            REQUIRE(from_point == f);
        }

        SECTION("From a ContractedGaussian") {
            ContractedGaussian cg(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(3), 1.0, 2.0, 3.0);
            shell_type from_cg(cg);
            REQUIRE(from_cg == f);
        }

        SECTION("Throws if the coefficient and exponent ranges differ") {
            std::vector<double> short_es{1.0};
            REQUIRE_THROWS_AS(shell_type(cs.begin(), cs.end(), short_es.begin(),
                                         short_es.end(), std::size_t(1), 0.0,
                                         0.0, 0.0),
                              std::invalid_argument);
        }

        SECTION("Copy/move") {
            test_chemist::test_copy_and_move(f, shell_type{});
        }

        SECTION("Copying is deep") {
            shell_type copied(f);
            copied.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
            REQUIRE(f.get_center() == Point(1.0, 2.0, 3.0));
        }
    }

    SECTION("swap") {
        shell_type other;
        shell_type f_copy(f);
        shell_type other_copy(other);

        f.swap(other);
        REQUIRE(f == other_copy);
        REQUIRE(other == f_copy);
    }

    SECTION("save/load") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar(f);
        }
        shell_type deserialized;
        {
            cereal::BinaryInputArchive ar(ss);
            ar(deserialized);
        }
        REQUIRE(deserialized == f);
    }
}
