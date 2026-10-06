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

// Only what SphericalAOView itself implements is tested here: its ctors,
// swap, and as_spherical_ao, plus the aliasing of the Cartesian shell. The
// shared API is tested in spherical_ao_common.cpp, and what it inherits from
// AOView in ao_and_ao_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
#include <chemist/experimental/basis_set/spherical_ao_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Takes a view by value, to prove the conversion from SphericalAO is
/// implicit.
std::size_t l_of(const_spherical_ao_view ao) { return ao.get_l(); }

} // namespace

TEST_CASE("experimental::SphericalAOView") {
    Point r(0.3, -0.4, 0.5);

    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};

    CCAShell<CartesianAO> shell(cs.begin(), cs.end(), es.begin(), es.end(),
                                std::size_t(2), 1.0, 2.0, 3.0);
    SphericalAO d1(shell, 1);

    SECTION("Ctors and assignment") {
        SECTION("There is no default ctor, and no assignment") {
            STATIC_REQUIRE_FALSE(
              std::is_default_constructible_v<const_spherical_ao_view>);
            STATIC_REQUIRE_FALSE(
              std::is_copy_assignable_v<const_spherical_ao_view>);
            STATIC_REQUIRE_FALSE(
              std::is_move_assignable_v<const_spherical_ao_view>);
        }

        SECTION("From a SphericalAO") {
            const_spherical_ao_view v(d1);
            REQUIRE(v == d1);
            REQUIRE(l_of(d1) == 2);
        }

        SECTION("From a shell aliases the shell") {
            const_spherical_ao_view v(shell, 1);
            REQUIRE(v == d1);

            shell.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
            REQUIRE(v.get_center() == Point(4.0, 5.0, 6.0));

            // ... and so the value tracks edits to the shell.
            SphericalAO edited(shell, 1);
            REQUIRE(as_double(v.normalized_evaluate(r)) ==
                    as_double(edited.normalized_evaluate(r)));
        }

        SECTION("From a shell view aliases what it aliases") {
            const_cartesian_cca_shell_view shell_view(shell);
            const_spherical_ao_view v(shell_view, 1);
            REQUIRE(v == d1);

            shell.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
            REQUIRE(v.get_center() == Point(4.0, 5.0, 6.0));
        }

        SECTION("From an owned shell view keeps that view") {
            auto pshell     = shell.as_view();
            const auto* raw = pshell.get();
            const_spherical_ao_view v(std::move(pshell), 1);
            REQUIRE(v == d1);

            // The view is kept, not cloned
            REQUIRE(&v.get_cartesian_shell() == raw);

            shell.get_contracted_gaussian().set_center(Point(4.0, 5.0, 6.0));
            REQUIRE(v.get_center() == Point(4.0, 5.0, 6.0));
        }

        SECTION("From an owned shell view throws if it is null") {
            using pointer = const_spherical_ao_view::shell_view_pointer;
            REQUIRE_THROWS_AS(const_spherical_ao_view(pointer{}, 1),
                              std::invalid_argument);
        }

        SECTION("From an owned shell view throws if |m| > l") {
            REQUIRE_THROWS_AS(const_spherical_ao_view(shell.as_view(), 3),
                              std::invalid_argument);
        }

        SECTION("Can not be built on a pure shell") {
            // The shell's purity is part of its type, so this is a
            // compile-time property; see the same section for SphericalAO.
            using view_type = const_spherical_ao_view;
            using m_type    = typename view_type::magnetic_index_type;
            STATIC_REQUIRE(
              std::is_constructible_v<view_type, const CCAShell<CartesianAO>&,
                                      m_type>);
            STATIC_REQUIRE(
              std::is_constructible_v<
                view_type, const const_cartesian_cca_shell_view&, m_type>);
            STATIC_REQUIRE_FALSE(
              std::is_constructible_v<view_type, const CCAShell<SphericalAO>&,
                                      m_type>);
            STATIC_REQUIRE_FALSE(
              std::is_constructible_v<
                view_type, const const_spherical_cca_shell_view&, m_type>);
            STATIC_REQUIRE_FALSE(
              std::is_constructible_v<view_type, const AOShellBaseView&,
                                      m_type>);
        }

        SECTION("From a shell throws if |m| > l") {
            REQUIRE_THROWS_AS(const_spherical_ao_view(shell, 3),
                              std::invalid_argument);
            const_cartesian_cca_shell_view shell_view(shell);
            REQUIRE_THROWS_AS(const_spherical_ao_view(shell_view, -3),
                              std::invalid_argument);
        }

        SECTION("Copying a view is shallow") {
            const_spherical_ao_view v(shell, 1);
            const_spherical_ao_view copied(v);
            REQUIRE(copied == v);
            shell.get_contracted_gaussian().set_center(Point(8.0, 8.0, 8.0));
            REQUIRE(copied.get_center() == Point(8.0, 8.0, 8.0));
        }

        SECTION("Move ctor") {
            const_spherical_ao_view v(shell, 1);
            const_spherical_ao_view moved(std::move(v));
            REQUIRE(moved == d1);
        }
    }

    SECTION("as_spherical_ao") {
        const_spherical_ao_view v(shell, 1);
        auto materialized = v.as_spherical_ao();
        REQUIRE(materialized == d1);
        REQUIRE(dynamic_cast<const CCAShell<CartesianAO>*>(
                  &materialized.get_cartesian_shell()) != nullptr);

        // The result owns its state, so it does not observe later writes.
        shell.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(materialized.get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("swap rebinds") {
        CCAShell<CartesianAO> other(cs.begin(), cs.end(), es.begin(), es.end(),
                                    std::size_t(3), 9.0, 9.0, 9.0);
        const_spherical_ao_view v(shell, 1);
        const_spherical_ao_view v2(other, -3);
        v.swap(v2);
        REQUIRE(v.get_l() == 3);
        REQUIRE(v.get_m() == -3);
        REQUIRE(v2 == d1);
    }
}
