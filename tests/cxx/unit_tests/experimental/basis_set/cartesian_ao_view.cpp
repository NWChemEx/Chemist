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

// Only what CartesianAOView itself implements is tested here: its ctors
// (including the implicit conversions), assignment (which, unlike a normal
// copy, writes through the alias), swap, as_cartesian_ao, and the aliasing
// behavior of the contracted Gaussian it holds. Everything CartesianAOView
// inherits from CartesianAOCommon is tested once, generically, in
// cartesian_ao_common.cpp, and what it inherits from AOView is tested in
// ao_and_ao_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cartesian_ao_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Takes a read-only view by value, to prove the conversions are implicit.
std::size_t l_of(const_cartesian_ao_view ao) { return ao.get_l(); }

} // namespace

TEST_CASE("experimental::CartesianAOView") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    // d_xy function: i = 1, j = 1, k = 0, so l = 2.
    CartesianAO dxy(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(1),
                    std::size_t(1), std::size_t(0), 1.0, 2.0, 3.0);

    CartesianAO other(cs.begin(), cs.end(), es.begin(), es.end(),
                      std::size_t(0), std::size_t(0), std::size_t(2), 9.0, 9.0,
                      9.0);

    cartesian_ao_view v(dxy);
    const_cartesian_ao_view cv(other);

    SECTION("Ctors and assignment") {
        SECTION("There is no default ctor") {
            // Like ContractedGaussianView, *this can not be default
            // constructed: a view has to alias something. The ctor is declared
            // but implicitly deleted; see the note on it.
            STATIC_REQUIRE_FALSE(
              std::is_default_constructible_v<cartesian_ao_view>);
        }

        SECTION("From a CartesianAO") {
            REQUIRE(v.get_i() == 1);
            REQUIRE(v.get_j() == 1);
            REQUIRE(v.get_k() == 0);
            REQUIRE(v == dxy);

            // *this aliases dxy's contraction rather than copying it.
            v.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
            REQUIRE(dxy.get_center() == Point(4.0, 5.0, 6.0));
        }

        SECTION("From a CartesianAO, implicitly") { REQUIRE(l_of(dxy) == 2); }

        SECTION("From an aliased contracted Gaussian") {
            ContractedGaussian cg(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(2), 1.0, 2.0, 3.0);
            cartesian_ao_view from_parts(contracted_gaussian_view(cg),
                                         std::size_t(1), std::size_t(1),
                                         std::size_t(0));
            REQUIRE(from_parts == dxy);

            // The contraction is aliased, not copied, which is what lets a
            // whole shell of AOs share one.
            cg.set_center(Point(7.0, 7.0, 7.0));
            REQUIRE(from_parts.get_center() == Point(7.0, 7.0, 7.0));
        }

        SECTION("From an aliased contracted Gaussian, powers must match l") {
            ContractedGaussian cg(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(2), 1.0, 2.0, 3.0);
            // The contraction already has an l, unlike in CartesianAO's ctor,
            // so a disagreement is possible and is caught.
            REQUIRE_THROWS_AS(cartesian_ao_view(contracted_gaussian_view(cg),
                                                std::size_t(1), std::size_t(1),
                                                std::size_t(1)),
                              std::invalid_argument);
        }

        SECTION("Mutable to read-only conversion") {
            const_cartesian_ao_view converted(v);
            REQUIRE(converted == dxy);

            // Implicitly, too.
            REQUIRE(l_of(v) == 2);
        }

        SECTION("Copying a view is shallow") {
            cartesian_ao_view copied(v);
            copied.get_contracted_gaussian().set_center(Point(8.0, 8.0, 8.0));
            // The copy aliases the same contraction, so dxy sees the write.
            REQUIRE(dxy.get_center() == Point(8.0, 8.0, 8.0));
        }

        SECTION("Move ctor") {
            cartesian_ao_view moved(std::move(v));
            REQUIRE(moved == dxy);
        }

        SECTION("Assignment writes the contraction through the alias") {
            // Same powers as dxy, different contraction parameters.
            std::vector<double> rhs_cs{7.0, 8.0};
            std::vector<double> rhs_es{3.0, 4.0};
            CartesianAO rhs(rhs_cs.begin(), rhs_cs.end(), rhs_es.begin(),
                            rhs_es.end(), std::size_t(1), std::size_t(1),
                            std::size_t(0), 5.0, 5.0, 5.0);
            v = rhs;

            // dxy itself was overwritten; v was not rebound.
            REQUIRE(dxy == rhs);
            REQUIRE(dxy.get_center() == Point(5.0, 5.0, 5.0));
            REQUIRE(
              as_double(dxy.get_contracted_gaussian()[0].get_coefficient()) ==
              7.0);
        }

        SECTION("Assignment from another view writes through too") {
            CartesianAO rhs(cs.begin(), cs.end(), es.begin(), es.end(),
                            std::size_t(1), std::size_t(1), std::size_t(0), 5.0,
                            5.0, 5.0);
            cartesian_ao_view rhs_view(rhs);
            v = rhs_view;
            REQUIRE(dxy == rhs);
        }

        SECTION("Assignment throws if the powers differ") {
            // The powers are not aliased, so they can not be written through;
            // silently updating only *this would leave dxy disagreeing with v.
            REQUIRE_THROWS_AS(v = other, std::runtime_error);

            // dxy was left alone.
            REQUIRE(dxy.get_i() == 1);
            REQUIRE(dxy.get_center() == Point(1.0, 2.0, 3.0));
        }
    }

    SECTION("as_cartesian_ao") {
        auto materialized = v.as_cartesian_ao();
        REQUIRE(materialized == dxy);

        // The result owns its state, so it does not observe later writes.
        dxy.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(materialized.get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("as_cartesian_ao works through a read-only view") {
        auto materialized = cv.as_cartesian_ao();
        REQUIRE(materialized == other);
    }

    SECTION("swap rebinds, rather than writing through") {
        cartesian_ao_view v2(other);
        v.swap(v2);

        // The views now alias each other's Cartesian AOs; the AOs themselves
        // were not touched.
        REQUIRE(v == other);
        REQUIRE(v2 == dxy);
        REQUIRE(dxy.get_center() == Point(1.0, 2.0, 3.0));
        REQUIRE(other.get_center() == Point(9.0, 9.0, 9.0));
    }
}
