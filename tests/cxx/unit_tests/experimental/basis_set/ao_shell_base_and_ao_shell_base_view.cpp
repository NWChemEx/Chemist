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

// AOShellBase and AOShellBaseView are abstract, so these tests reach them
// through CCAShell and CCAShellView, of both purities. What is tested is the
// polymorphic path: that asking through a base reference gets the same answer
// as asking the derived class, that indexing hands out the right kind of AO
// view in the right order, and that clone/as_view/as_shell/are_equal behave as
// documented. AOShellBaseCommon is tested here too, since it can only be
// reached through one of the two bases. The layer above these bases, which
// fixes the purity, is tested in ao_shell_and_ao_shell_view.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
#include <chemist/experimental/point/point_class.hpp>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Detects whether a shell type has cartesian_powers / magnetic_index
///@{
template<typename T, typename = void>
struct has_cartesian_powers : std::false_type {};

template<typename T>
struct has_cartesian_powers<
  T, std::void_t<decltype(std::declval<const T&>().cartesian_powers(0))>>
  : std::true_type {};

template<typename T, typename = void>
struct has_magnetic_index : std::false_type {};

template<typename T>
struct has_magnetic_index<
  T, std::void_t<decltype(std::declval<const T&>().magnetic_index(0))>>
  : std::true_type {};
///@}

} // namespace

TEST_CASE("experimental::AOShellBase") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    cartesian_cca_shell d(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(2), 1.0, 2.0, 3.0);
    spherical_cca_shell pd(cs.begin(), cs.end(), es.begin(), es.end(),
                           std::size_t(2), 1.0, 2.0, 3.0);
    const AOShellBase& base      = d;
    const AOShellBase& pure_base = pd;

    SECTION("The shared API agrees with the derived class") {
        for(const AOShellBase* b : {&base, &pure_base}) {
            REQUIRE(b->get_l() == 2);
            REQUIRE(b->get_center() == Point(1.0, 2.0, 3.0));
            REQUIRE(b->get_contracted_gaussian() ==
                    d.get_contracted_gaussian());
            REQUIRE(as_double(b->normalization_constant()) ==
                    as_double(d.normalization_constant()));
        }
    }

    SECTION("is_pure/is_cartesian") {
        REQUIRE_FALSE(base.is_pure());
        REQUIRE(base.is_cartesian());
        REQUIRE(pure_base.is_pure());
        REQUIRE_FALSE(pure_base.is_cartesian());
    }

    SECTION("size accounts for the purity") {
        REQUIRE(base.size() == 6);
        REQUIRE(pure_base.size() == 5);
    }

    SECTION("The angular indices are not part of the base") {
        // Which angular index a shell has depends on its purity, so only
        // AOShell<AOType> and below, which know their purity statically,
        // provide one.
        STATIC_REQUIRE_FALSE(has_cartesian_powers<AOShellBase>::value);
        STATIC_REQUIRE_FALSE(has_magnetic_index<AOShellBase>::value);
        STATIC_REQUIRE_FALSE(has_cartesian_powers<AOShellBaseView>::value);
        STATIC_REQUIRE_FALSE(has_magnetic_index<AOShellBaseView>::value);
    }

    SECTION("Indexing through the base follows the shell's ordering") {
        // What the base gives up is only asking an offset for its angular
        // index; asking the AO at that offset still works.
        for(std::size_t a = 0; a < d.size(); ++a) {
            auto pao       = base.at(a);
            const auto& ao = dynamic_cast<const const_cartesian_ao_view&>(*pao);
            const auto [i, j, k] = d.cartesian_powers(a);
            REQUIRE(ao.get_i() == i);
            REQUIRE(ao.get_j() == j);
            REQUIRE(ao.get_k() == k);
        }
        for(std::size_t a = 0; a < pd.size(); ++a) {
            auto pao       = pure_base.at(a);
            const auto& ao = dynamic_cast<const const_spherical_ao_view&>(*pao);
            REQUIRE(ao.get_m() == pd.magnetic_index(a));
        }
    }

    SECTION("at of a Cartesian shell hands out Cartesian AO views") {
        for(std::size_t a = 0; a < d.size(); ++a) {
            auto pao = base.at(a);
            REQUIRE(dynamic_cast<const const_cartesian_ao_view*>(pao.get()) !=
                    nullptr);
            REQUIRE(pao->are_equal(*d.at(a)));
            REQUIRE(base[a]->are_equal(*pao));

            // Each call builds a new view
            REQUIRE(base.at(a).get() != pao.get());
        }
        REQUIRE_THROWS_AS(base.at(6), std::out_of_range);
    }

    SECTION("at of a pure shell hands out spherical AO views") {
        for(std::size_t a = 0; a < pd.size(); ++a) {
            auto pao = pure_base.at(a);
            REQUIRE(dynamic_cast<const const_spherical_ao_view*>(pao.get()) !=
                    nullptr);
            REQUIRE(pao->are_equal(*pd.at(a)));
        }
        REQUIRE_THROWS_AS(pure_base.at(5), std::out_of_range);
    }

    SECTION("The AOs from at alias the shell") {
        auto ao      = base.at(1);
        auto pure_ao = pure_base.at(1);
        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        pd.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(ao->get_center() == Point(0.0, 0.0, 0.0));
        REQUIRE(pure_ao->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("clone is a deep copy of the same kind") {
        auto cloned = base.clone();
        REQUIRE(cloned->are_equal(base));
        REQUIRE(dynamic_cast<cartesian_cca_shell*>(cloned.get()) != nullptr);

        auto pure_cloned = pure_base.clone();
        REQUIRE(pure_cloned->are_equal(pure_base));
        REQUIRE(dynamic_cast<spherical_cca_shell*>(pure_cloned.get()) !=
                nullptr);

        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->get_center() == Point(1.0, 2.0, 3.0));
        REQUIRE(cloned->are_different(base));
    }

    SECTION("as_view is a shallow view of the same kind") {
        auto v = base.as_view();
        REQUIRE(dynamic_cast<const_cartesian_cca_shell_view*>(v.get()) !=
                nullptr);

        auto pv = pure_base.as_view();
        REQUIRE(dynamic_cast<const_spherical_cca_shell_view*>(pv.get()) !=
                nullptr);
        REQUIRE(pv->is_pure());

        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(v->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("are_equal/are_different") {
        cartesian_cca_shell same(cs.begin(), cs.end(), es.begin(), es.end(),
                                 std::size_t(2), 1.0, 2.0, 3.0);
        REQUIRE(base.are_equal(same));
        REQUIRE_FALSE(base.are_different(same));

        cartesian_cca_shell diff(cs.begin(), cs.end(), es.begin(), es.end(),
                                 std::size_t(1), 1.0, 2.0, 3.0);
        REQUIRE(base.are_different(diff));

        // Same contracted Gaussian, different purity
        REQUIRE(base.are_different(pure_base));
        REQUIRE(pure_base.are_different(base));
    }
}

TEST_CASE("experimental::AOShellBaseView") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    cartesian_cca_shell d(cs.begin(), cs.end(), es.begin(), es.end(),
                          std::size_t(2), 1.0, 2.0, 3.0);
    spherical_cca_shell pd(cs.begin(), cs.end(), es.begin(), es.end(),
                           std::size_t(2), 1.0, 2.0, 3.0);
    const_cartesian_cca_shell_view v(d);
    const_spherical_cca_shell_view pv(pd);
    const AOShellBaseView& base      = v;
    const AOShellBaseView& pure_base = pv;

    SECTION("The shared API agrees with the derived class") {
        REQUIRE(base.get_l() == v.get_l());
        REQUIRE(base.size() == 6);
        REQUIRE(pure_base.size() == 5);
        REQUIRE(base.is_cartesian());
        REQUIRE(pure_base.is_pure());
        REQUIRE(base.get_center() == v.get_center());
        REQUIRE(base.at(3)->are_equal(*v.at(3)));
        REQUIRE(pure_base.at(3)->are_equal(*pv.at(3)));
    }

    SECTION("clone is a shallow copy") {
        auto cloned = base.clone();
        REQUIRE(cloned->are_equal(base));
        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(cloned->get_center() == Point(0.0, 0.0, 0.0));
    }

    SECTION("as_shell materializes an owning shell of the same kind") {
        auto materialized = base.as_shell();
        REQUIRE(dynamic_cast<cartesian_cca_shell*>(materialized.get()) !=
                nullptr);
        REQUIRE(materialized->are_equal(d));

        auto pure_materialized = pure_base.as_shell();
        REQUIRE(dynamic_cast<spherical_cca_shell*>(pure_materialized.get()) !=
                nullptr);
        REQUIRE(pure_materialized->are_equal(pd));

        d.get_contracted_gaussian().set_center(Point(0.0, 0.0, 0.0));
        REQUIRE(materialized->get_center() == Point(1.0, 2.0, 3.0));
    }

    SECTION("are_equal/are_different") {
        cartesian_cca_shell other(cs.begin(), cs.end(), es.begin(), es.end(),
                                  std::size_t(1), 1.0, 2.0, 3.0);
        const_cartesian_cca_shell_view other_view(other);
        REQUIRE(base.are_different(other_view));

        cartesian_cca_shell same(d);
        const_cartesian_cca_shell_view same_view(same);
        REQUIRE(base.are_equal(same_view));

        REQUIRE(base.are_different(pure_base));
    }
}
