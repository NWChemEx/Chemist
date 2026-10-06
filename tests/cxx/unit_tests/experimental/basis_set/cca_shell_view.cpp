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

// Only what CCAShellView itself implements is tested here: its ctors
// (including the implicit conversions), assignment (which writes through the
// alias), swap, and as_cca_shell. None of these depend on the purity, so they
// are tested for both purities at once. The shared API is tested in
// cca_shell_common.cpp, and what it inherits from AOShellView<AOType> and
// AOShellBaseView in ao_shell_and_ao_shell_view.cpp and
// ao_shell_base_and_ao_shell_base_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEMPLATE_TEST_CASE("experimental::CCAShellView", "", CartesianAO, SphericalAO) {
    using shell_type      = CCAShell<TestType>;
    using view_type       = CCAShellView<shell_type>;
    using const_view_type = CCAShellView<const shell_type>;

    // Takes a read-only view by value, to prove the conversions are implicit.
    auto l_of = [](const_view_type shell) { return shell.get_l(); };

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    shell_type d(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(2),
                 1.0, 2.0, 3.0);
    shell_type p(cs.begin(), cs.end(), es.begin(), es.end(), std::size_t(1),
                 9.0, 9.0, 9.0);

    view_type v(d);
    const_view_type cv(p);

    SECTION("Ctors and assignment") {
        SECTION("There is no default ctor") {
            STATIC_REQUIRE_FALSE(std::is_default_constructible_v<view_type>);
        }

        SECTION("From a CCAShell") {
            REQUIRE(v == d);
            v.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
            REQUIRE(d.get_center() == Point(4.0, 5.0, 6.0));
        }

        SECTION("From a CCAShell, implicitly") { REQUIRE(l_of(d) == 2); }

        SECTION("From an aliased contracted Gaussian") {
            ContractedGaussian cg(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(2), 1.0, 2.0, 3.0);
            view_type from_cg(contracted_gaussian_view{cg});
            REQUIRE(from_cg == d);

            cg.set_center(Point(7.0, 7.0, 7.0));
            REQUIRE(from_cg.get_center() == Point(7.0, 7.0, 7.0));
        }

        SECTION("Mutable to read-only conversion") {
            const_view_type converted(v);
            REQUIRE(converted == d);
            REQUIRE(l_of(v) == 2);
        }

        SECTION("Copying a view is shallow") {
            view_type copied(v);
            copied.get_contracted_gaussian().set_center(Point(8.0, 8.0, 8.0));
            REQUIRE(d.get_center() == Point(8.0, 8.0, 8.0));
        }

        SECTION("Move ctor") {
            view_type moved(std::move(v));
            REQUIRE(moved == d);
        }

        SECTION("Assignment writes the contraction through the alias") {
            std::vector<double> rhs_cs{7.0, 8.0};
            std::vector<double> rhs_es{3.0, 4.0};
            shell_type rhs(rhs_cs.begin(), rhs_cs.end(), rhs_es.begin(),
                           rhs_es.end(), std::size_t(3), 5.0, 5.0, 5.0);
            v = rhs;

            // d itself was overwritten, l included; v was not rebound.
            REQUIRE(d == rhs);
            REQUIRE(d.get_l() == 3);
        }

        SECTION("Assignment from another view writes through too") {
            view_type rhs_view(p);
            v = rhs_view;
            REQUIRE(d == p);
        }

        SECTION("Assignment throws if the number of primitives differs") {
            std::vector<double> one{1.0};
            shell_type rhs(one.begin(), one.end(), one.begin(), one.end(),
                           std::size_t(2), 0.0, 0.0, 0.0);
            REQUIRE_THROWS_AS(v = rhs, std::runtime_error);
        }
    }

    SECTION("as_cca_shell") {
        auto materialized = v.as_cca_shell();
        STATIC_REQUIRE(std::is_same_v<decltype(materialized), shell_type>);
        REQUIRE(materialized == d);

        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(materialized.get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("as_cca_shell works through a read-only view") {
        REQUIRE(cv.as_cca_shell() == p);
    }

    SECTION("swap rebinds, rather than writing through") {
        view_type v2(p);
        v.swap(v2);
        REQUIRE(v == p);
        REQUIRE(v2 == d);
        REQUIRE(d.get_l() == 2);
        REQUIRE(p.get_l() == 1);
    }
}
