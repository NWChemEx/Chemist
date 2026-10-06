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

// Only what SphericalAO itself implements is tested here: its ctors,
// assignment, and swap. Everything it inherits from SphericalAOCommon is
// tested in spherical_ao_common.cpp, and what it inherits from AO in
// ao_and_ao_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cca_shell_class.hpp>
#include <chemist/experimental/basis_set/spherical_ao_class.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <type_traits>
#include <vector>

using namespace chemist::experimental;

TEST_CASE("experimental::SphericalAO") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    SphericalAO f(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(3),
                  -2, 1.0, 2.0, 3.0);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            SphericalAO defaulted;
            REQUIRE(defaulted.get_l() == 0);
            REQUIRE(defaulted.get_m() == 0);
            REQUIRE(defaulted.get_contracted_gaussian().size() == 0);
            REQUIRE(defaulted.get_center() == Point(0.0, 0.0, 0.0));
            REQUIRE(dynamic_cast<const CCAShell<CartesianAO>*>(
                      &defaulted.get_cartesian_shell()) != nullptr);
        }

        SECTION("From ranges and coordinates") {
            REQUIRE(f.get_l() == 3);
            REQUIRE(f.get_m() == -2);
            REQUIRE(f.get_center() == Point(1.0, 2.0, 3.0));
            REQUIRE(f.get_contracted_gaussian().size() == 2);
            REQUIRE(dynamic_cast<const CCAShell<CartesianAO>*>(
                      &f.get_cartesian_shell()) != nullptr);
        }

        SECTION("From ranges and a Point") {
            SphericalAO from_point(cs.begin(), cs.end(), es.begin(), es.end(),
                                   std::size_t(3), -2, Point(1.0, 2.0, 3.0));
            REQUIRE(from_point == f);
        }

        SECTION("From ranges throws if |m| > l") {
            REQUIRE_THROWS_AS(SphericalAO(cs.begin(), cs.end(), es.begin(),
                                          es.end(), std::size_t(1), 2, 0.0, 0.0,
                                          0.0),
                              std::invalid_argument);
        }

        SECTION("From a shell") {
            CCAShell<CartesianAO> shell(cs.begin(), cs.end(), es.begin(),
                                        es.end(), std::size_t(3), 1.0, 2.0,
                                        3.0);
            SphericalAO from_shell(shell, -2);
            REQUIRE(from_shell == f);

            // The shell was copied, not aliased.
            shell.get_contracted_gaussian().set_center(Point(9.0, 9.0, 9.0));
            REQUIRE(from_shell.get_center() == Point(1.0, 2.0, 3.0));
        }

        SECTION("From a ContractedGaussian") {
            ContractedGaussian cg(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(3), 1.0, 2.0, 3.0);
            SphericalAO from_cg(cg, -2);
            REQUIRE(from_cg == f);
            REQUIRE_THROWS_AS(SphericalAO(cg, 4), std::invalid_argument);
        }

        SECTION("Can not be built on a pure shell") {
            // A spherical AO is a combination of Cartesian AOs, so it can not
            // be built on a shell of spherical ones. The shell's purity is
            // part of its type, so this is a compile-time property.
            using m_type = SphericalAO::magnetic_index_type;
            STATIC_REQUIRE(
              std::is_constructible_v<SphericalAO, const CCAShell<CartesianAO>&,
                                      m_type>);
            STATIC_REQUIRE_FALSE(
              std::is_constructible_v<SphericalAO, const CCAShell<SphericalAO>&,
                                      m_type>);
            STATIC_REQUIRE_FALSE(
              std::is_constructible_v<SphericalAO, const AOShellBase&, m_type>);
        }

        SECTION("From a shell throws if |m| > l") {
            CCAShell<CartesianAO> shell(cs.begin(), cs.end(), es.begin(),
                                        es.end(), std::size_t(1), 0.0, 0.0,
                                        0.0);
            REQUIRE_THROWS_AS(SphericalAO(shell, -2), std::invalid_argument);
        }

        SECTION("Copy/move") {
            test_chemist::test_copy_and_move(f, SphericalAO{});
        }

        SECTION("Copying is deep") {
            SphericalAO copied(f);
            REQUIRE(&copied.get_cartesian_shell() != &f.get_cartesian_shell());
        }

        SECTION("Copy assignment is deep") {
            SphericalAO copied;
            copied = f;
            REQUIRE(copied == f);
            REQUIRE(&copied.get_cartesian_shell() != &f.get_cartesian_shell());
        }
    }

    SECTION("swap") {
        SphericalAO other;
        SphericalAO f_copy(f);
        SphericalAO other_copy(other);

        f.swap(other);
        REQUIRE(f == other_copy);
        REQUIRE(other == f_copy);
    }
}
